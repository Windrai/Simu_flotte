#include "drone.hpp"
#include "flotte.hpp"
#include <stdexcept>  // Pour std::runtime_error
//#include <algorithm>
//#include <unordered_map>

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


// Méthodes statiques pour QQmlListProperty
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





// Méthode pour exposer les drones à QML
/*
QList<QObject*> Fleet::getDrones() const {
    QList<QObject*> droneList;
    for (const auto& drone : drones) {
        droneList.append(drone.data()); // Convertit QSharedPointer<Drone> en QObject*
    }
    return droneList;
}
*/




/*
// Fonction utilitaire pour associer une position à une cellule
static GridCell Fleet::get_grid_cell(float x, float y, float z, float cell_size) {
    return {
        static_cast<int>(std::floor(x / cell_size)),
        static_cast<int>(std::floor(y / cell_size)),
        static_cast<int>(std::floor(z / cell_size))
    };
}

QList<QPair<int, int>> Fleet::detect_collisions(float collision_threshold, float cell_size) const {
    std::unordered_map<GridCell, QList<QSharedPointer<Drone>>> grid;

    // Étape 1 : Assigner les drones aux cellules
    for (const auto& drone : drones) {
        GridCell cell = get_grid_cell(drone->get_pos_x(), drone->get_pos_y(), drone->get_pos_z(), cell_size);
        grid[cell].append(drone);
    }

    QList<QPair<int, int>> collisions;

    // Étape 2 : Vérifier les drones par cellule et les cellules voisines
    for (const auto& entry : grid) {
        const auto& cell = entry.first;
        const auto& cell_drones = entry.second;

        // Itérer sur les drones de cette cellule
        for (int i = 0; i < cell_drones.size(); ++i) {
            for (int j = i + 1; j < cell_drones.size(); ++j) {
                if (calculate_distance(
                        {cell_drones[i]->get_pos_x(), cell_drones[i]->get_pos_y(), cell_drones[i]->get_pos_z()},
                        {cell_drones[j]->get_pos_x(), cell_drones[j]->get_pos_y(), cell_drones[j]->get_pos_z()}) <
                    collision_threshold) {
                    collisions.append(qMakePair(cell_drones[i]->get_id(), cell_drones[j]->get_id()));
                }
            }
        }

        // Vérifier les cellules voisines
        for (int dx = -1; dx <= 1; ++dx) {
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dz = -1; dz <= 1; ++dz) {
                    if (dx == 0 && dy == 0 && dz == 0) continue; // Ne pas comparer à la même cellule
                    GridCell neighbor_cell = {cell.x + dx, cell.y + dy, cell.z + dz};
                    if (grid.find(neighbor_cell) == grid.end()) continue;

                    const auto& neighbor_drones = grid.at(neighbor_cell);
                    for (const auto& drone1 : cell_drones) {
                        for (const auto& drone2 : neighbor_drones) {
                            if (calculate_distance(
                                    {drone1->get_pos_x(), drone1->get_pos_y(), drone1->get_pos_z()},
                                    {drone2->get_pos_x(), drone2->get_pos_y(), drone2->get_pos_z()}) <
                                collision_threshold) {
                                collisions.append(qMakePair(drone1->get_id(), drone2->get_id()));
                            }
                        }
                    }
                }
            }
        }
    }

    return collisions;
}
*/