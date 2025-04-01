#include "drone.hpp"
#include "flotte.hpp"
#include <iostream>
#include <memory>  // Pour std::unique_ptr
#include <stdexcept>


using std::cout;
using std::endl;
using std::cerr;
using std::this_thread::sleep_for;
using std::chrono::seconds;


int main() {
    //Test drone.cpp
    // Création de drones avec des chemins
    int drone1_id = 1;
    int num_path_points = 5;
    float x = 0.0f, y = 10.0f, z = 20.0f;


    // Récupérer le drone créé avec la nouvelle version de la fonction
    std::unique_ptr<Drone> drone1 = report_et_initialise_drone(drone1_id, num_path_points, x, y, z);
    if (!drone1) {
        cerr << "Échec de la création du drone." << drone1_id << endl;
        return 1;
    }



    drone1->print_drone();
    drone1->print_path();


    // Test de la mise à jour de la position du drone
    drone1->update_drone_position();
    cout << "Drone 1 après mise à jour de la position:" << endl;
    drone1->print_drone();

    // Test d'ajouter un nouveau point de chemin
    drone1->add_point_to_path(3); // Ajouter 3 points
    drone1->print_path();

    // Test de l'activation et de la désactivation du drone
    drone1->deactivate();
    cout << "Drone 1 après désactivation:" << endl;
    drone1->print_drone();

    drone1->activate();
    cout << "Drone 1 après activation:" << endl;
    drone1->print_drone();

    // Communication avec le drone
    drone1->communicate();

    return 0;
}



/*
int main() {
    // Création d'une flotte
    Fleet fleet;

    // Ajout de drones à la flotte
    fleet.add_drone_to_fleet(5, 101, 10.0f, 20.0f, 30.0f);
    fleet.add_drone_to_fleet(5, 102, 15.0f, 25.0f, 35.0f);
    fleet.add_drone_to_fleet(5, 103, 20.0f, 30.0f, 40.0f);

    std::cout << "Affichage de la flotte après ajout de 3 drones:" << std::endl;
    fleet.print_fleet();  // Affichage des drones de la flotte

    // Mise à jour des positions des drones
    fleet.update_fleet_positions();
    std::cout << "\nAffichage de la flotte après mise à jour des positions:" << std::endl;
    fleet.print_fleet();

    // Recherche et affichage d'un drone spécifique
    int search_id = 102;
    std::cout << "\nRecherche du drone avec l'ID " << search_id << ":" << std::endl;
    fleet.print_drone_in_fleet(search_id);  // Affiche le drone d'ID 102

    // Suppression d'un drone de la flotte
    std::cout << "\nSuppression du drone avec l'ID 101:" << std::endl;
    fleet.remove_drone_from_fleet(101);
    fleet.print_fleet();  // Affichage des drones après suppression

    // Essayer de supprimer un drone non existant
    std::cout << "\nSuppression d'un drone avec un ID non existant (999):" << std::endl;
    fleet.remove_drone_from_fleet(999);  // ID 999 n'existe pas

    // Affichage de la flotte après la suppression
    std::cout << "\nAffichage final de la flotte:" << std::endl;
    fleet.print_fleet();

    return 0;
}
*/
