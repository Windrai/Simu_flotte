# 🖥️ IHM – Interface de Simulation de Flotte de Drones

Cette application représente l’**Interface Homme-Machine (IHM)** du projet de simulation de flotte de drones. Elle est développée avec **Qt Quick (QML + C++)** et permet de :

- Visualiser les drones en temps réel
- Recevoir et interpréter les messages UART envoyés par la carte STM32
- Gérer dynamiquement la flotte (création et mise à jour automatique) //Création pas encore réalisée
- Afficher une interface graphique claire avec des informations utiles

---

## 📐 Architecture

- **C++ (Qt)** : logique de la flotte, gestion UART, backend
- **QML (Qt Quick)** : interface graphique moderne et réactive
- **UART** : communication série avec la carte STM32 (port `/dev/ttyUSB0` par défaut)

---

## 🚀 Fonctionnalités principales

- Réception automatique de messages UART (ex : `Drone 2 - Pos: (120, 45, 80) - Bat: 85 - Act: 1`)
- Création/mise à jour de drones en temps réel à partir des données reçues
- Carte simulée avec affichage des positions
- Liste latérale affichant l’état détaillé des drones (position, batterie, activité)
- Données exposées en QML via `QQmlListProperty` et `setContextProperty`

---

## 📁 Arborescence des fichiers

IHM_flotte/
├── src/
    ├── main.cpp               // Entrée de l'application Qt
    ├── drone.hpp
    ├── drone.cpp              // Classe représentant un drone
    ├── flotte.hpp
    ├── flotte.cpp             // Gestion de la flotte
    ├── SerialManager.hpp
    ├── SerialManager.cpp      // Gestion de la communication UART
├── qml/
    ├── main.qml              // Vue principale
    ├── Header.qml            // En-tête (titre)
    ├── panneau_lateral.qml   // Affichage des drones
    ├── qml.qrc                // Ressources QML embarquées
    └── map.qml               // Affichage de la "zone de vol"
├── include/                  //Fichiers d'entêtes
└── build/                   // Répertoire de build 
---

## 🧱 Technologies & dépendances

- Qt 5 ou 6 (modules : **Core**, **Quick**, **SerialPort**, **QML**)
- C++17
- CMake ≥ 3.10

---

## 🏗️ Compilation

### ✅ Prérequis

- Qt installé (via Qt Installer ou paquet système)
- CMake et un compilateur C++17 (GCC, Clang, MSVC)

### 🔧 Étapes

```bash
cd IHM
cd build
cmake ..
make
./ihm_drones  # ou le nom généré de l'exécutable

## 🧑‍💻 Auteur

**[Grégoire Domer]**  
Projet étudiant / perso en systèmes embarqués, Qt et simulation temps réel

---

## 📄 Licence

Projet sous licence MIT. Libre utilisation, modification et distribution.
