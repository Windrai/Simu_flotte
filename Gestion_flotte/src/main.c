#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "drone.h"
#include "flotte.h"

#include <string.h>
#include <stdio.h>  // Pour snprintf() en C
#include <stdlib.h>



/**
 * @brief Déclaration des entêtes de fonctions
 */
void SystemClock_Config(void);
void UART_Init();
void Error_Handler();
void LED_Init(void);
void Task_UART_Transmit(void *pvParameters);
void Task_UART_Receive(void *pv_parameters);
void Task_Fleet_Manager(void *pvParameters);
void Task_Battery(void *pvParameters);

/**
 * @brief Déclaration des variables globales et des handlers UART 
 * et de files d'attente RX et TX
 * et Mutex
 */
UART_HandleTypeDef huart2;
QueueHandle_t UART_RXQueue;
QueueHandle_t UART_TXQueue;
uint8_t rxBuffer[TX_BUFFER_SIZE];
Fleet fleet;
SemaphoreHandle_t xFleetMutex; //j'utilise un mutex sur la flotte mais je pourrais le faire sur les drones pour être plus précis



int main(void) {
    HAL_Init();
    SystemClock_Config();
    UART_Init();

    // Initialisation de la flotte
    init_fleet(&fleet);

    // Initialisation des files de messages
    UART_RXQueue = xQueueCreate(QUEUE_SIZE, TX_BUFFER_SIZE);
    UART_TXQueue = xQueueCreate(QUEUE_SIZE, TX_BUFFER_SIZE);

    if (UART_RXQueue == NULL || UART_TXQueue == NULL) {
        UART2_SendString("Erreur: Files UART non créées!\r\n");
        Error_Handler();
    }
    //SendMessageToQueue("Test complet de l'UART\r\n");

    //Drone drone1;
    //init_drone(&drone1, 4, 40,40,40);
    //print_drone(&drone1,&huart2);

    // Ajout de quelques drones de test
    add_drone_to_fleet(&fleet, 1, 100, 100, 100);
    add_drone_to_fleet(&fleet, 2, 5000, 5000, 5000);
    add_drone_to_fleet(&fleet, 3, 30000, 40000, 40000);
    add_drone_to_fleet(&fleet, 4, 35000, 40000, 40000);
    add_drone_to_fleet(&fleet, 5, 40000, 40000, 40000);
    add_drone_to_fleet(&fleet, 6, 45000, 40000, 40000);

    xFleetMutex = xSemaphoreCreateMutex();
    if (xFleetMutex == NULL) {
        // Gestion d'erreur
        UART2_SendString("Erreur: mutex fleet non créé!\r\n");
        Error_Handler();
    }

    // Création des tâches FreeRTOS
    xTaskCreate(Task_Fleet_Manager, "Fleet_Manager", 256, NULL, 2, NULL);//Si on la fait tourner plus fréquemment pour des calculs plus fins alors on peut la monter à une prio critique
    xTaskCreate(Task_UART_Transmit, "UART_Send", 128, NULL, 1, NULL);
    xTaskCreate(Task_UART_Receive, "UART_Receive", 128, NULL, 3, NULL); //Prio élevée car entrée TR
    xTaskCreate(Task_Battery, "Battery_Cons", 128, NULL, 1, NULL); //faible prio
  //  UART2_SendString("Démarrage FreeRTOS...\r\n");

    vTaskStartScheduler();

    while(1) {}
}


/**
 * @brief Tâche FreeRTOS pour gérer la flotte (Représente le déplacement du drone).
 */
void Task_Fleet_Manager(void *pvParameters) {
    
    while (1) {
        if (xSemaphoreTake(xFleetMutex, portMAX_DELAY)){
            update_fleet_positions(&fleet);
            print_fleet(&fleet, &huart2);  // Affichage des positions
            xSemaphoreGive(xFleetMutex);
        }
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}



/**
 * @brief Tâche FreeRTOS pour gérer l'envoie des messages sur l'UART
 */
//Attends qu'il y ait un msg dans la pile et l'envoie les messages sur l'UART
void Task_UART_Transmit(void *pvParameters) {
    uint8_t txBuffer[TX_BUFFER_SIZE];
    while(1){        
        if (xQueueReceive(UART_TXQueue, txBuffer, pdMS_TO_TICKS(1000))){
            // On a reçu des données du pc. On les affiche
            HAL_UART_Transmit(&huart2, txBuffer, strlen((char * )txBuffer), HAL_MAX_DELAY);
            //Faire quelquechose de la data 
        }
    }
}

/**
 * @brief Tâche FreeRTOS pour afficher les messages reçus du PC 
 */
void Task_UART_Receive(void *pv_parameters){
    uint8_t rxBuffer_receive[TX_BUFFER_SIZE]; // double buffering pour éviter d'écraser les données si une autre ISR se lance tout de suite

    while(1){
        if (xQueueReceive(UART_RXQueue, rxBuffer_receive, pdMS_TO_TICKS(1000))){
            // On a reçu des données du pc. On les affiche
            HAL_UART_Transmit(&huart2, rxBuffer_receive, strlen((char * )rxBuffer_receive), HAL_MAX_DELAY);
            //Faire quelquechose de la data 
        }
    }
}

/**
 * @brief Tâche FreeRTOS pour Simuler la diminution de la battery
 */

void Task_Battery(void *pvParameters){
    const TickType_t xDelay = pdMS_TO_TICKS(1000); // toutes les 10 secondes

    while (1) {
        if (xSemaphoreTake(xFleetMutex, portMAX_DELAY)) {
            FleetNode* current = fleet.head;

            while (current != NULL) {
                battery_cons(&current->drone);
                current = current->next;
            }
            xSemaphoreGive(xFleetMutex);
        }
        vTaskDelay(xDelay);
    }   
}


/**
 * @brief Fonction de configuration de l'horloge système
 */

void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};


    // Active HSE (8 MHz sur la STM32F4Discovery) et configure le PLL
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = 16;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLM = 16;
    RCC_OscInitStruct.PLL.PLLN = 336;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4; // Donne 84 MHz CPU
    RCC_OscInitStruct.PLL.PLLQ = 4;
    
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    
    // Configuration de l'horloge du CPU, AHB et APB
    RCC_ClkInitStruct.ClockType = (RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2);
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;  // PCLK1 = 42 MHz
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;  // PCLK2 = 84 MHz
    
    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
        Error_Handler();
    }
        /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}



/**
 * @brief Fonction pour faire clignoter la LED toutes les secondes
 */
void Blink_LED_Debug(void) {
    __HAL_RCC_GPIOD_CLK_ENABLE(); // Activer l'horloge pour la LED sur la STM32F4Discovery
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    while(1){
        HAL_GPIO_TogglePin(GPIOD, GPIO_PIN_12);
        HAL_Delay(500);  // Clignotement visible
    }
}


/**
 * @brief Handler d'erreurs
 */
void Error_Handler(){
    Blink_LED_Debug();
}



/*
void Task_Delayed_Start(void *pvParameters) {
    UART2_SendString("Démarrage de FreeRTOS dans 5 secondes...\r\n");
    vTaskDelay(pdMS_TO_TICKS(5000)); // Attendre 5 secondes

    // Créer les autres tâches après 5s
    xTaskCreate(Task_Fleet_Manager, "Fleet_Manager", 256, NULL, 2, NULL);
    xTaskCreate(Task_UART_Transmit, "UART_Send", 128, NULL, 1, NULL);
    xTaskCreate(Task_UART_Receive, "UART_Receive", 128, NULL, 2, NULL);

    // Supprime la tâche après l'exécution
    vTaskDelete(NULL);
}
*/

/*
int main(void) {
    // 1. Initialiser la bibliothèque HAL
    HAL_Init();
    SystemClock_Config(); //Vérifier la PLL

    UART_Init();
    
    //Initialisation de la Pile de message
    UART_RXQueue = xQueueCreate(QUEUE_SIZE,TX_BUFFER_SIZE);

    if (UART_RXQueue == NULL) {
        UART2_SendString("Erreur: File UART non créée!\r\n");
        Error_Handler();
    }

    UART_TXQueue = xQueueCreate(QUEUE_SIZE,TX_BUFFER_SIZE);

    if (UART_TXQueue == NULL) {
        UART2_SendString("Erreur: File UART non créée!\r\n");
        Error_Handler();
    }


    // Démarrage de la réception UART en interruption 
    HAL_UART_Receive_IT(&huart2, rxBuffer, TX_BUFFER_SIZE);

    
    xTaskCreate(Task_UART_Transmit, "UART_Send", 128, NULL, 1, NULL);
    xTaskCreate(Task_UART_Receive, "UART_Reveive", 128, NULL, 2, NULL);

    //Tester la transmission depuis une pile de message TX 
    SendMessageToQueue("Test1");

    HAL_UART_Transmit(&huart2, (uint8_t *)"Hello world \n", 13, HAL_MAX_DELAY);
    UART2_SendString("Démarrage de FreeRTOS...\r\n");


    vTaskStartScheduler();

    while(1){}
}
*/