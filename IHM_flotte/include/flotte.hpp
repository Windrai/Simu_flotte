#ifndef FLEET_H
#define FLEET_H

#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <iostream>
#include "drone.hpp"
#include <QSharedPointer>  // Inclure QSharedPointer
#include <QObject>

//ON remplave std::list avec QList pour utilisation avec Qt.
//Utiliser des QsharedPointer avec Qlist plutot que des unique_ptr à cause des copies effectuées



/**
 * @class Fleet
 * @brief Représente une flotte de drones, exposée à QML pour affichage et interaction.
 *
 * La classe `Fleet` agit comme un conteneur dynamique de drones, chacun représenté par un objet `Drone`.
 * Elle fournit toutes les méthodes nécessaires pour ajouter, supprimer, rechercher et mettre à jour les drones.
 *
 * Les données sont synchronisées avec l'interface QML via des propriétés et signaux :
 * - `QQmlListProperty<Drone>` expose la liste de drones à l’interface
 * - Le signal `fleet_Updated()` est émis à chaque mise à jour d’un drone
 * - Le signal `fleetChanged()` est émis quand la structure de la flotte change (ajout/suppression). A implémenter 
 *
 * ## Fonctionnalités principales :
 * - Ajout / suppression / recherche de drones
 * - Synchronisation avec l'IHM via `QQmlListProperty`
 * - Affichage de l'état de la flotte en console
 * - Mise à jour automatique des drones depuis des messages UART (réception STM32)
 *
 * ## Intégration QML :
 * - La propriété `pdrones` expose une liste réactive utilisable dans des `Repeater`, `ListView`, etc.
 * - La taille de la flotte est accessible via `pfleetSize`
 *
 * @see Drone pour les objets individuels représentés dans la flotte
 */

class Fleet : public QObject {
    Q_OBJECT

    Q_PROPERTY(QQmlListProperty<Drone> pdrones READ getDrones NOTIFY fleet_Updated)
    Q_PROPERTY(int pfleetSize READ size NOTIFY fleetChanged)

public:
    Fleet(QObject *parent = nullptr) : QObject(parent) {}
    ~Fleet();

    // Méthodes principales
    void add_drone_to_fleet(QSharedPointer<Drone> drone); 
    void add_drone_to_fleet(int num_path_points, int id, float x, float y, float z);
    void remove_drone_from_fleet(int id);              
    Drone* find_drone_in_fleet(int id) const;       
    void print_fleet() const;                    // Affichage des informations de la flotte
    void print_drone_in_fleet(int id) const;    // Affichage d'un drone contenue dans la flotte 
    bool check_empty_fleet() const;     
    void updateDroneFromUARTMessage(const std::string &msg);
    int size() const { return drones.size(); }

    //Expose la liste à QML
    QQmlListProperty<Drone> getDrones();

signals:
    void fleet_Updated();
    void fleetChanged();

private:

    QList<QSharedPointer<Drone>> drones;

    // Méthode statique pour accéder au nombre d'éléments.
    static qsizetype dronesCount(QQmlListProperty<Drone>* list);

    //Méthode statique pour accéder à un drone de la liste
    static Drone* droneAt(QQmlListProperty<Drone>* list, qsizetype index);

     //Fonction statique pour ajouter un drone
    static void appendDrone(QQmlListProperty<Drone>* list, Drone* drone);
};


#endif // FLEET_H