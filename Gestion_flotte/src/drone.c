#include "drone.h"
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>



static int clamp_int(int32_t value, int min, int max){
    return (value < min) ? min : ((value > max) ? max : value);
};


/**
 * @brief Vérifie si les coordonnées sont dans les limites autorisées.
 */
static int is_within_bounds(int32_t x, int32_t y, int32_t z) {
    return (x >= MIN_POS && x <= MAX_POS && 
            y >= MIN_POS && y <= MAX_POS && 
            z >= MIN_POS && z <= MAX_POS);
}


void init_drone(Drone *drone, int16_t id, int32_t x, int32_t y, int32_t z) {
    if (!is_within_bounds(x, y, z)) {
        return ;
    }
    drone->id = id;
    drone->path_index = 0;
    drone->position.x = x;
    drone->position.y = y;
    drone->position.z = z;
    drone->active = 1;
    drone->landing = false;
    
    drone->battery_level = 100;

    // Ajouter la position initiale au chemin
    drone->path[0] = drone->position;
}


void add_point_to_path(Drone *drone, int32_t x, int32_t y, int32_t z) {
    drone->path_index = (drone->path_index + 1) % MAX_PATH_LENGTH;
    drone->path[drone->path_index].x = x;
    drone->path[drone->path_index].y = y;
    drone->path[drone->path_index].z = z;
}


void update_drone_position(Drone *drone) {
    if (!drone->active) return;

    if (drone->landing) {
        // Ne bouge plus en x/y
        if (drone->position.z > 0) {
            drone->position.z -= 10000; // descente hard
            if (drone->position.z < 0)
                drone->position.z = 0;
        } else {
            drone->landing = false;
            drone->active = false;
        }
        return;
    }

    int8_t step_x = rand() % 51;
    int8_t step_y = rand() % 51;
    int8_t step_z = rand() % 51;

    // Affecte aléatoirement un signe à chaque pas.
    if (rand() % 2) { step_x = -step_x; }
    if (rand() % 2) { step_y = -step_y; }
    if (rand() % 2) { step_z = -step_z; }

    // Met à jour les positions.
    drone->position.x += step_x;
    drone->position.y += step_y;
    drone->position.z += step_z;

    drone->position.x = clamp_int(drone->position.x, MIN_POS, MAX_POS);
    drone->position.y = clamp_int(drone->position.y, MIN_POS, MAX_POS);
    drone->position.z = clamp_int(drone->position.z, MIN_POS, MAX_POS);

    // Ajout du point au chemin
    add_point_to_path(drone, drone->position.x, drone->position.y, drone->position.z);
}


void activate_drone(Drone *drone) {
    drone->active = 1;
}

void deactivate_drone(Drone *drone) {
    drone->active = 0;
}


void print_drone(Drone *drone, UART_HandleTypeDef *huart) {
    char message[TX_BUFFER_SIZE];
    
    // Formatage correct des données
    int length = snprintf(message, sizeof(message), 
                          "Drone %" PRId16 " - Pos: (%ld, %ld, %ld) - Bat: %" PRId16 " - Act: %" PRId16 " \r\n", 
                          drone->id, 
                          drone->position.x, 
                          drone->position.y, 
                          drone->position.z,
                          (int16_t)drone->battery_level,
                          (int16_t)drone->active);
    // Vérification de la taille de la chaîne
    if (length > 0 && length < TX_BUFFER_SIZE) {
        // Remplissage éventuel pour être sûr que la taille est MESSAGE_LENGTH.
        memset(message + length, 0, TX_BUFFER_SIZE - length);
        SendMessageToQueue(message);
    } else {
        // En cas d'erreur de formatage ou dépassement, transmettre un message d'erreur
        const char errorMsg[] = "Erreur affichage drone\r\n";
        // On peut aussi copier errorMsg dans un buffer de taille MESSAGE_LENGTH.
        char buffer[TX_BUFFER_SIZE] = {0};
        strncpy(buffer, errorMsg, TX_BUFFER_SIZE - 1);
        SendMessageToQueue(buffer);
    }
}



void battery_cons(Drone *drone){
    if (drone->battery_level > 4){
        drone->battery_level -= 1;
    }
    else if (drone->battery_level > 0){
        drone->landing = true; // Drone commence à descendre en Z
        drone->battery_level -= 1;
    }
    else {
        drone->active = false;  // Drone posé
        drone->landing = false;
    }
}
