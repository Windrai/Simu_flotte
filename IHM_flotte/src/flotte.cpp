#include "drone.hpp"
#include "flotte.hpp"
#include <stdexcept>  // Pour std::runtime_error

using std::cerr;
using std::endl;
using std::cout;
using std::runtime_error;




Fleet::~Fleet(){}


void Fleet::add_drone_to_fleet(QSharedPointer<Drone> drone){
    //Meilleur que push_back car évite la copie de drone. Mais laisse drone dans un état indéterminé
    //Donc inutilisable si non réinitialisé.
    //drones.emplace_back(std::move(drone));
    drones.append(drone);
    emit fleetChanged();
}

void Fleet::add_drone_to_fleet(int num_path_points, int id, float x, float y, float z){
    auto new_drone = Drone::report_et_initialise_drone(id,num_path_points, x, y,z);
    //drones.emplace_back(std::move(new_drone)); // utile avec les unique_ptr
    drones.append(new_drone);
    emit fleetChanged();
}

void Fleet::remove_drone_from_fleet(int id){
    //renvoie un itérateur sur la liste dans l'intervalle si la lambda fonction (prédicat ici) renvoie True
    //[id] : Capture par valeur, immuable dans la lambda (copie)
    //(const std::unique_ptr<Drone>&) : Param d'entrée de la lambda
    auto it = std::find_if(drones.begin(), drones.end(), [id](const QSharedPointer<Drone>& drone) {return id==drone->get_id();});
    if (it != drones.end()){
        drones.erase(it);
        emit fleetChanged();
    }
    else {
        cerr << "Le drone d'ID " << id << "est non trouvé dans la flotte." << endl;
    }
}


Drone* Fleet::find_drone_in_fleet(int id) const{
    auto it = std::find_if(drones.begin(), drones.end(), [id](const QSharedPointer<Drone>& drone) {return id==drone->get_id();});
    if (it != drones.end()){
        return it->get();
    }
    else {
        cerr << "Le drone d'ID " << id << " est non trouvé dans la flotte." << endl;
    }
    return nullptr;
}


void Fleet::print_fleet() const {
    if (check_empty_fleet()) return;

    std::cout << "\n========== État de la flotte ==========\n";
    printf("%-10s %-8s %-10s %-30s\n", "Drone ID", "Actif", "Batterie",  "Position");

    for (const auto& d : drones) {
        printf("%-10d %-8s %-10.1f (%8.1f, %8.1f, %8.1f)\n",
               d->get_id(),
               d->is_active() ? "Oui" : "Non",
               d->get_battery(),
               d->get_pos_x(),
               d->get_pos_y(),
               d->get_pos_z());
    }

    std::cout << "=======================================\n\n";
}


void Fleet::print_drone_in_fleet(int id) const{
    Drone* drone = find_drone_in_fleet(id);
    
    if (drone != nullptr) drone->print_drone();
}

bool Fleet::check_empty_fleet() const{
    if (drones.empty()) {
        cout << "La flotte est vide." << endl;
        return true;
    }
    return false;
}



void Fleet::updateDroneFromUARTMessage(const std::string &msg) {
    // On ne traite que les messages qui commencent par "Drone"
    if (msg.find("Drone") != 0) {
        //qDebug() << "Message ignoré (ne commence pas par 'Drone'):" << QString::fromStdString(msg);
        return;
    }
    
    int receivedId;
    int xi, yi, zi;
    int battery, act;
    // On s'attend à recevoir un message au format : 
    // "Drone %d - Pos: (%d, %d, %d)"
    int nb = sscanf(msg.c_str(), "Drone %d - Pos: (%d, %d, %d) - Bat: %d - Act: %d ", &receivedId, &xi, &yi, &zi, &battery, &act);
    if (nb == 6) {
        // Recherche d'un drone existant
        Drone* existingDrone = find_drone_in_fleet(receivedId);
        if (existingDrone) {
            // Mise à jour de la position
            if (existingDrone->updateFromUARTMessage(msg))
                emit fleet_Updated();
                qDebug() << "Drone" << receivedId << "mis à jour avec :" << QString::fromStdString(msg);
        } else {
            // Si le drone n'existe pas, on le crée et on l'ajoute à la flotte.
            auto newDrone = Drone::report_et_initialise_drone(receivedId, 5,
                                        static_cast<float>(xi),
                                        static_cast<float>(yi),
                                        static_cast<float>(zi));
            if (newDrone) {
                add_drone_to_fleet(newDrone);
                emit fleetChanged();
                qDebug() << "Nouveau drone ajouté: ID" << receivedId << "avec :" << QString::fromStdString(msg);
            } else {
                qDebug() << "Erreur lors de la création du drone ID" << receivedId;
            }
        }
        
    } else {
        //qDebug() << "Format de message incorrect :" << QString::fromStdString(msg);
    }
}







qsizetype Fleet::dronesCount(QQmlListProperty<Drone>* list) {
    auto* fleet = qobject_cast<Fleet*>(list->object);
    return fleet ? fleet->drones.size() : 0;
}

Drone* Fleet::droneAt(QQmlListProperty<Drone>* list, qsizetype index) {
    auto* fleet = qobject_cast<Fleet*>(list->object);
    return fleet && index >= 0 && index < fleet->drones.size() ? fleet->drones[index].data() : nullptr;
}

/* Fonction statique pour ajouter un drone
list->object est un pointeur générique (QObject*) vers l'objet propriétaire de la liste (fleet).
*/
void Fleet::appendDrone(QQmlListProperty<Drone>* list, Drone* drone) {
    auto* fleet = qobject_cast<Fleet*>(list->object);
    //Si la conversion réussit (ce qui signifie que list->object est effectivement une instance de Fleet)
    if (fleet) {
        fleet->drones.append(QSharedPointer<Drone>(drone)); // Ajoute le drone
    }
}


QQmlListProperty<Drone> Fleet::getDrones() {
    return QQmlListProperty<Drone>(this, this, &Fleet::appendDrone, &Fleet::dronesCount, &Fleet::droneAt, nullptr);
}


