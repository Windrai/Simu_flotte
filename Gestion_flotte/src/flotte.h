#ifndef FLOTTE_H
#define FLOTTE_H

#include "drone.h"
#include "stm32f4xx_hal.h"

#define MAX_FLEET_DRONES 10  // Nombre maximum de drones dans la flotte


/**
 * @brief 
 * Structure permettant la création d'une liste chaînée de drone
 *      drone : Drone contenu dans le noeud
 *      next : Pointeur vers la structure du prochain drone dans la flotte
*/
typedef struct FleetNode {
    Drone drone;
    struct FleetNode* next;
} FleetNode;


/**
 * @brief 
 * Structure contenant la flotte
 *      head : Pointeur vers le premier drone de la flotte
 *      size : Taille actuelle de la flotte
*/
typedef struct {
    FleetNode* head; 
    int size;         
} Fleet;


/**
 * @brief 
 * init_fleet() : Initialisation de la flotte
 * add_drone_to_fleet() : Ajout d'un drone à la flotte (avec limite MAX_FLEET_DRONES)
 * remove_drone_from_fleet() : Suppression d'un drone de la flotte par ID
 * find_drone_in_fleet(): Trouver un drone par son ID
 * update_fleet_positions() : Met à jour toutes les positions des drones dans la flotte
 * print_fleet() : Affichage des informations de la flotte via UART
 * free_fleet() : Libère la mémoire allouée à la flotte
*/


void init_fleet(Fleet* fleet);
void add_drone_to_fleet(Fleet* fleet, int16_t id, int32_t x, int32_t y, int32_t z);
void remove_drone_from_fleet(Fleet* fleet, int16_t id);
Drone* find_drone_in_fleet(Fleet* fleet, int16_t id);
void update_fleet_positions(Fleet* fleet);
void print_fleet(Fleet* fleet, UART_HandleTypeDef *huart);
void free_fleet(Fleet* fleet);


extern void SendMessageToQueue(const char *msg);
extern void UART2_SendString(const char *str);
#endif
