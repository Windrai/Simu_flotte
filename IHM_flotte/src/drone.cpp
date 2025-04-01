#include "drone.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>

using namespace std;


Drone::Drone(int id, int num_path_points, float x, float y, float z, QObject* parent)
    : QObject(parent), id(id), pos_x(x), pos_y(y), pos_z(z), active(true), current_step(0), battery_level(100) {
    if (!is_within_bounds(x, y, z)) {
        cerr << "L'une des coordonnées n'est pas dans la zone définie" << endl;
        return;
    }

    if (num_path_points < 1) {
        cerr << "Erreur : Il faut à minima 1 point dans le chemin" << endl;
        return;
    }
}


QSharedPointer<Drone> Drone::create(int id, int num_path_points, float x, float y, float z) {
    if (!is_within_bounds(x, y, z)) {
        cerr << "Coordonnées hors des limites autorisées" << endl;
        return nullptr;
    }

    if (num_path_points < 1) {
        cerr << "Le chemin doit contenir au moins un point" << endl;
        return nullptr;
    }
    return QSharedPointer<Drone>::create(id, num_path_points, x, y, z);
}


// Vérification si la position du drone est dans les limites autorisées
bool Drone::is_within_bounds(float x, float y, float z) {
    return (x >= MIN_POS && x <= Constants::MAX_POS && y >= MIN_POS && y <= Constants::MAX_POS && z >= MIN_POS && z <= MAX_POS);
}







void Drone::print_path() const {
    int index = 0;
    for (const auto& p : path) {
        cout << std::fixed << std::setprecision(2) << "Point " << index++ << " : {" << p.x << ", " << p.y << ", " << p.z << "}" << endl;
    }
}


void Drone::print_drone() const {
    cout << std::fixed << std::setprecision(2) << "Drone ID : " << id << ", Active : " << active << ", Position : (" << pos_x << ", " << pos_y << ", " << pos_z << ")" << endl;
}


void Drone::activate() {
    active = true;
}


void Drone::deactivate() {
    active = false;
}

// Getters
int Drone::get_id() const { return id; }
float Drone::get_pos_x() const { return pos_x; }
float Drone::get_pos_y() const { return pos_y; }
float Drone::get_pos_z() const { return pos_z; }
float Drone::get_battery() const { return battery_level; }
bool Drone::is_active() const { return active; }
int Drone::get_current_step() const { return current_step; }


// Destructeur
Drone::~Drone() = default;


// Fonction pour initialiser un drone et afficher un rapport
QSharedPointer<Drone> Drone::report_et_initialise_drone(int id, int num_path_points, float x, float y, float z) {
    auto drone = Drone::create(id, num_path_points, x, y, z);
    if (!drone) {
        cerr << "Erreur lors de la création du drone." << endl;
        return nullptr;
    }
    cout << "Drone créé avec succès : " << drone->get_id() << endl;
    return drone;
}



bool Drone::updateFromUARTMessage(const std::string &msg) {
    // On attend un message du type :
    // "Drone %d - Pos: (%d, %d, %d) - Bat: %d - Act: %d "
    int receivedId;
    int x, y, z;
    int battery, act;
    // Tente d'extraire l'ID et les coordonnées
    if (sscanf(msg.c_str(), "Drone %d - Pos: (%d, %d, %d) - Bat: %d - Act: %d ", &receivedId, &x, &y, &z, &battery, &act) == 6) {
        // Si l'ID correspond, on met à jour la position
        if (this->id == receivedId) {
            this->pos_x = static_cast<float>(x);
            this->pos_y = static_cast<float>(y);
            this->pos_z = static_cast<float>(z);
            this->battery_level= static_cast<float>(battery);
            this->active = act;
            return true;
        }
    }
    else printf("Problème de chaîne dans updateFromUARTMessage");
    return false;
}
