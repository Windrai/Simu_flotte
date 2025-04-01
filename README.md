# 🛰️ Simulation de Flotte de Drones – IHM + STM32 + UART

Ce projet propose une simulation complète d'une **flotte de drones**, répartie entre deux environnements :

- 🖥️ Une **IHM (interface PC)** développée en **Qt/QML + C++**, pour visualiser et piloter la flotte.
- 📟 Un **firmware embarqué STM32F407** codé en **C + FreeRTOS**, simulant des drones en tâche temps réel.
- 🔄 Une **communication série UART** entre les deux, pour échanger des informations en temps réel.

---

## 🌐 Objectifs

- Simuler une flotte autonome, évolutive, réactive
- Gérer chaque drone : position, état actif, batterie, chemin
- Afficher une interface temps réel conviviale pour l’utilisateur
- Structurer l’architecture logicielle autour de la séparation des responsabilités : IHM ↔ Firmware

---

## 📁 Arborescence du projet

```text
.
├── IHM_flotte/                 <- Interface Qt/QML (PC)
├── Gestion_flotte              <- Partie embarquée STM32
```

---

## 🖥️ Partie IHM (PC - Qt/QML)

### Fonctionnalités :
- Visualisation des drones avec leur position, batterie, statut
- Réception automatique des messages UART envoyés par la STM32
- Affichage via une interface QML moderne
- Gestion dynamique de la flotte (création automatique à la réception d’un message inconnu)

### Compilation :
```bash
cd IHM
mkdir build && cd build
cmake ..
make
./ihm_drones  # ou nom de l'exécutable
```

> Dépendances : Qt 5 ou 6, modules `Quick`, `SerialPort`, `QML`, C++17

---

## 📟 Partie Embarquée STM32 (Firmware FreeRTOS)

### Fonctionnalités :
- Simulation de drones (position aléatoire, batterie, atterrissage)
- UART avec file RX/TX et transmission périodique vers le PC
- Gestion de la flotte via une liste chaînée
- FreeRTOS : tâches indépendantes pour déplacement, batterie, communication

### Compilation et Flash (via PlatformIO) :
```bash
cd Gestion_flotte
pio run                 # Compilation
pio run --target upload # Flash carte STM32
```

> Carte : STM32F407Discovery  
> Port UART : USART2 (PA2/PA3), 115200 bauds

---

## 🔌 Communication UART

La communication se fait par des messages texte :

### Exemples :

**Message STM32 → PC** (toutes les secondes) :
```
Drone 2 - Pos: (120, 400, 500) - Bat: 85 - Act: 1
```

**Message PC → STM32** (à implémenter) :
```
CMD;MOVE;ID=3;X=500;Y=300
```

---

## 📌 À venir / améliorations possibles

- Ajout de commandes en émission (depuis l'IHM vers STM32)
- Simulateur 3D pour affichage spatial
- Détection de collisions
- Tests unitaires pour chaque module

---

## 🧑‍💻 Auteur

**[Grégoire Domer]**  
Projet perso en systèmes embarqués, Qt et simulation temps réel

---
