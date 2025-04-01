#include "uart_func.h"


extern void UART2_SendString(const char *str) {
    HAL_UART_Transmit(&huart2, (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
}


extern void UART2_SendNumber(uint32_t number) {
    char buffer[20];  // Buffer pour stocker le nombre converti
    int len = snprintf(buffer, sizeof(buffer), "%lu", number); // Convertit en chaîne
    HAL_UART_Transmit(&huart2, (uint8_t *)buffer, len, HAL_MAX_DELAY);
}



void SendMessageToQueue(const char *msg) {
    if (xQueueSend(UART_TXQueue, msg, pdMS_TO_TICKS(1000)) != pdTRUE) {
        // Gestion d'erreur si la file est pleine
        Error_Handler();
    }
}



/*Callback déclenché lorsqu'un message est reçu sur l’UART*/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (huart->Instance == USART2)
    {
        xQueueSendFromISR(UART_RXQueue, rxBuffer, NULL);

        HAL_UART_Receive_IT(&huart2, rxBuffer, TX_BUFFER_SIZE);

        // Si une tâche de plus haute priorité a été réveillée, déclencher un changement de contexte
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}


void UART_Init(){
    HAL_UART_MspInit(&huart2);

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 115200;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.WordLength = UART_WORDLENGTH_8B; //8bits 
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE; // pas de contrôle de flux hardware
    huart2.Init.StopBits = UART_STOPBITS_1; // 1 bit de stop
    huart2.Init.OverSampling = UART_OVERSAMPLING_16; // Suréchantillonnage x16
    // Initialiser l'UART en mode asynchrone
    if (HAL_UART_Init(&huart2) != HAL_OK) {
        // Gestion d'erreur
        Error_Handler();
    }
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart) {
    GPIO_InitTypeDef GPIO_UART = {0};

    if (huart->Instance == USART2){
            // Suivre les recommendations pour GPIO 
        __HAL_RCC_USART2_CLK_ENABLE();
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_UART.Pin = GPIO_PIN_2 | GPIO_PIN_3;
        GPIO_UART.Mode = GPIO_MODE_AF_PP;        // Mode alternate function push-pull
        GPIO_UART.Pull = GPIO_PULLUP;
        GPIO_UART.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        GPIO_UART.Alternate = GPIO_AF7_USART2;   // Alternate function pour USART2
        HAL_GPIO_Init(GPIOA, &GPIO_UART);

        
    }


}