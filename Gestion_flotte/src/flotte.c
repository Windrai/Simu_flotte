#include "flotte.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * @brief Initialise une flotte vide.
 */
void init_fleet(Fleet* fleet) {
    fleet->head = NULL;
    fleet->size = 0;
}

/**
 * @brief Ajoute un drone à la flotte.
 */
void add_drone_to_fleet(Fleet* fleet, int16_t id, int32_t x, int32_t y, int32_t z) {
    char message[25];

    if (fleet->size >= MAX_FLEET_DRONES) {
        SendMessageToQueue("Flotte pleine: Ajout impossible\n");
        return;
    }

    FleetNode* new_node = (FleetNode*)malloc(sizeof(FleetNode));
    if (new_node == NULL) {
        SendMessageToQueue("Erreur mémoire: Drone non ajouté\n");
        return;
    }

    init_drone(&(new_node->drone), id, x, y, z);
    new_node->next = fleet->head;
    fleet->head = new_node;
    fleet->size++;

    snprintf(message, sizeof(message), "Drone %d ajouté\n", id);
    SendMessageToQueue(message);
}

/**
 * @brief Supprime un drone de la flotte par ID.
 */
void remove_drone_from_fleet(Fleet* fleet, int16_t id) {
    FleetNode *current = fleet->head, *prev = NULL;
    char message[25];

    while (current != NULL) {
        if (current->drone.id == id) {
            if (prev == NULL) {
                fleet->head = current->next;
            } else {
                prev->next = current->next;
            }

            free(current);
            fleet->size--;

            snprintf(message, sizeof(message), "Drone %d supprimé\n", id);
            SendMessageToQueue(message);
            return;
        }
        prev = current;
        current = current->next;
    }

    SendMessageToQueue("Drone non trouvé\n");
}

/**
 * @brief Trouve un drone dans la flotte par son ID.
 */
Drone* find_drone_in_fleet(Fleet* fleet, int16_t id) {
    FleetNode* current = fleet->head;
    while (current != NULL) {
        if (current->drone.id == id) {
            return &(current->drone);
        }
        current = current->next;
    }
    return NULL;
}

/**
 * @brief Met à jour les positions des drones.
 */
void update_fleet_positions(Fleet* fleet) {
    FleetNode* current = fleet->head;
    while (current != NULL) {
        update_drone_position(&(current->drone));
        current = current->next;
    }
}

/**
 * @brief Affiche la flotte via l'UART.
 */
void print_fleet(Fleet* fleet, UART_HandleTypeDef *huart) {
    FleetNode* current = fleet->head;
    char message[50];
        // Vérifier si la flotte contient des drones
    if (fleet->size == 0) {
        UART2_SendString("Aucun drone actif.\r\n");
        return;
    }

    snprintf(message, sizeof(message), "Drones actifs: %d/%d\r\n", fleet->size, MAX_FLEET_DRONES);
    SendMessageToQueue(message);

    while (current != NULL) {
        print_drone(&(current->drone), huart);
        current = current->next;
    }
}

/**
 * @brief Libère la mémoire allouée à la flotte.
 */
void free_fleet(Fleet* fleet) {
    FleetNode* current = fleet->head;
    while (current != NULL) {
        FleetNode* temp = current;
        current = current->next;
        free(temp);
    }
    fleet->head = NULL;
    fleet->size = 0;
}
