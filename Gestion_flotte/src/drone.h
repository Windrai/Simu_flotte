#ifndef DRONE_H
#define DRONE_H

#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdbool.h>



/**
 * @brief 
 * MAX_DRONES : Nombre de Drones que l'on peut instancier au maximum
 * MAX_PATH_LENGTH : Nombre de point
 * MIN_POS : Position minimale pouvant être atteinte par un drone
 * MAX_POS : Position maximale pouvant être atteinte par un drone
 * TX_BUFFER_SIZE : Taille du buffer utilisé pour envoyer des informations sur l'UART (Même taille pour les buffers RX)
 * QUEUE_SIZE : Taille des piles de messages
*/

#define MAX_DRONES 10
#define MAX_PATH_LENGTH 20
#define MIN_POS 0
#define MAX_POS 50000
#define TX_BUFFER_SIZE 64
#define QUEUE_SIZE 30

extern void SendMessageToQueue(const char *message);


/**
 * @brief 
 * Représente un point dans un repère orthonormé : 
 *      x : Position en x
 *      y : Position en y
 *      z : Position en z
*/
typedef struct {
    int32_t x;
    int32_t y;
    int32_t z;
} Point;

/**
 * @brief 
 * Représente un point dans un repère orthonormé : 
 *      id : Identifiant du Drone
 *      position : Position actuelle du drone
 *      path : Buffer circulaire contenant les MAX_PATH_LENGTH dernières postions du drone
 *      path_index : Indique à quel index nous nous trouvons actuellement dans path
 *      active : Indique si le drone est actif ou non
 *      battery_level : Indique le pourcentage de batterie restante
*/
typedef struct {
    int16_t id;
    Point position;
    Point path[MAX_PATH_LENGTH];
    int8_t path_index;
    int8_t active;
    int8_t battery_level;
    bool landing;
} Drone;



/**
 * @brief 
 * init_drone() : Initialisation d'un drone
 * add_point_to_path() : Ajoute un nouveau point au chemin du drone (FIFO circulaire).
 * update_drone_position() : Met à jour la position du drone en ajoutant un déplacement aléatoire.
 * activate_drone() : Active le drone
 * deactivate_drone(): Désactive le drone
 * print_drone(): Affiche les informations du drone sur l'UART.
 * battery_cons() : Simule la diminution de la batterie
*/

void init_drone(Drone *drone, int16_t id, int32_t x, int32_t y, int32_t z);
void add_point_to_path(Drone *drone, int32_t x, int32_t y, int32_t z);
void update_drone_position(Drone *drone);
void activate_drone(Drone *drone);
void deactivate_drone(Drone *drone);
void print_drone(Drone *drone, UART_HandleTypeDef *huart);
void battery_cons(Drone *drone);

#endif
