#ifndef UART_FUNC_H
#define UART_FUNC_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"


#include <string.h>
#include <stdio.h> 
#include <stdlib.h>

#include "drone.h"


/**
 * @brief 
 * Fonctions et variables définies dans main.c
*/
extern UART_HandleTypeDef huart2;
extern uint8_t rxBuffer[TX_BUFFER_SIZE];
extern QueueHandle_t UART_RXQueue;
extern QueueHandle_t UART_TXQueue;
extern void Error_Handler();


/**
 * @brief 
 * UART2_SendString() : Envoie d'une chaine de caractères via UART.
 * UART2_SendNumber() : Envoie d'un nombre sur l'UART
 * SendMessageToQueue() : Envoie d'un message la file d'attente TX
 * UART_Init() : Initialisation de l'UART
*/
void UART2_SendString(const char *str);
void UART2_SendNumber(uint32_t number);
void SendMessageToQueue(const char *msg);
void UART_Init();
void HAL_UART_MspInit();

#endif