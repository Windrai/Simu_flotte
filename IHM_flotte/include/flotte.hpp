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
class Fleet : public QObject {
    Q_OBJECT

    //Q_PROPERTY(QList<QObject*> pdrones READ getDrones NOTIFY fleet_Updated)
    Q_PROPERTY(QQmlListProperty<Drone> pdrones READ getDrones NOTIFY fleet_Updated)
    Q_PROPERTY(int pfleetSize READ size NOTIFY fleetChanged)

public:
    Fleet(QObject *parent = nullptr) : QObject(parent) {}
    ~Fleet();

    // Méthodes principales
    void add_drone_to_fleet(QSharedPointer<Drone> drone); // Ajout d'un drone à la flotte
    void add_drone_to_fleet(int num_path_points, int id, float x, float y, float z);
    void remove_drone_from_fleet(int id);                   // Suppression d'un drone par ID
    Drone* find_drone_in_fleet(int id) const;                   // Recherche d'un drone par ID
    void print_fleet() const;                    // Affichage des informations de la flotte
    void print_drone_in_fleet(int id) const;    // Affichage d'un drone contenue dans la flotte 
    bool check_empty_fleet() const;      // Vérification : La flotte est vide ? 
    void updateDroneFromUARTMessage(const std::string &msg);
    
    // Mise à jour des positions des drones
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




/* Explication pour l'utilisation de QQmlListProperty : 

QQmlListProperty est un wrapper qui permet à QML d'interagir avec une liste d'objets C++.
Ce n'est pas une liste en soi, mais plutôt une abstraction qui définit comment accéder, 
parcourir, et modifier les éléments d'une liste. Native QML

J'avais d'abord utilisé une QList et je n'arrivais pas à accéder au propriété même en 
effectuant une conversion en QList<QObject*>
*/

/*
struct GridCell {
    int x, y, z;

    // Opérateur requis pour les clés dans std::unordered_map
    bool operator==(const GridCell& other) const {
        return x == other.x && y == other.y && z == other.z;
    }
};

// Spécialisation pour utiliser GridCell comme clé dans unordered_map
namespace std {
    template <>
    struct hash<GridCell> {
        size_t operator()(const GridCell& cell) const {
            return hash<int>()(cell.x) ^ hash<int>()(cell.y) ^ hash<int>()(cell.z);
        }
    };
}
*/
#endif // FLEET_H