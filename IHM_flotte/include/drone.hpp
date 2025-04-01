#ifndef DRONE_HPP
#define DRONE_HPP

#include <iostream>
#include <list>
#include <memory>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <iomanip>
#include <QObject>
#include <QSharedPointer>
#include <QDebug>
#include <QQmlListProperty>
#include "constants.hpp"







struct Point {
    float x, y, z;
};

// Classe Drone
class Drone : public QObject {
//On expose les propriétées au système QML et déclenche un signak chaque qu'une propritété chang
    Q_OBJECT
    Q_PROPERTY(int pid READ get_id CONSTANT)
    Q_PROPERTY(float pposX READ get_pos_x NOTIFY droneUpdated)
    Q_PROPERTY(float pposY READ get_pos_y NOTIFY droneUpdated)
    Q_PROPERTY(float pposZ READ get_pos_z NOTIFY droneUpdated)
    Q_PROPERTY(float pbattery READ get_battery NOTIFY droneUpdated)
    Q_PROPERTY(bool pactive READ is_active NOTIFY droneUpdated)



public:
    // "Constructeur"
    
    static QSharedPointer<Drone> report_et_initialise_drone(int id, int num_path_points, float x, float y, float z);

    // Méthodes de gestion du chemin
    bool updateFromUARTMessage(const std::string &msg);
    
    // Méthodes d'affichage
    void print_path() const;
    void print_drone() const;

    // Méthodes de gestion de l'état du drone
    void activate();
    void deactivate();
    void communicate() const;

    // Getters
    int get_id() const;
    float get_pos_x() const;
    float get_pos_y() const;
    float get_pos_z() const;
    float get_battery() const;
    bool is_active() const;
    int get_current_step() const;

    // Destructeur
    ~Drone();

    // Normalement Constructeur en privé car initialisation conditionnelle des membres avec create mais impossible avec make_unique.
    explicit Drone(int id, int num_path_points, float x, float y, float z, QObject* parent = nullptr);

    
signals:
    void droneUpdated(); // Signal émis lors de la mise à jour des propriétés


private:
    
    int id;
    float pos_x, pos_y, pos_z;
    bool active;
    int current_step;
    float battery_level;
    std::list<Point> path; // Chemin du drone
    int path_length;

    // Méthode utilitaire pour vérifier si la position est dans la zone définie
    static bool is_within_bounds(float x, float y, float z);

    static QSharedPointer<Drone> create(int id, int num_path_points, float x, float y, float z);

    



};




#endif // DRONE_HPP
