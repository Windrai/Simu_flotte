#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP


/**
 * @namespace Constants
 * @brief Contient les constantes globales utilisées pour la simulation de drones.
 *
 * Ce namespace centralise les paramètres physiques de la simulation, notamment
 * les bornes de positionnement, les dimensions de l’espace de vol, et le rayon d’un drone.
 *
 * ## Constantes définies :
 * - `MIN_POS` : Position minimale sur un axe 
 * - `MAX_POS` : Position maximale sur un axe 
 * - `RADIUS`  : Rayon (ou taille) d’un drone 
 * - `WIDTH`   : Largeur de la fenêtre d'affichage de la map
 * - `HEIGHT`  : Hauteur de la fenêtre d'affichage de la map
 *
 * Ces valeurs peuvent être exposées à QML sous forme de `QVariantMap` dans le main pour accès global dans l’IHM.
 */



namespace Constants {
    constexpr int MIN_POS = 0;
    constexpr int MAX_POS = 50000;
    constexpr int RADIUS = 10;
    constexpr int WIDTH = 1000;
    constexpr int HEIGHT = 1000;
}

using Constants::MIN_POS;
using Constants::MAX_POS;

#endif // CONSTANTS_HPP