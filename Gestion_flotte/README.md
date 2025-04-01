# 📟 Gestion embarquée – Simulation de Flotte de Drones

Ce module embarqué est développé pour une **STM32F407 Discovery**. Il gère une flotte de drones simulés en tâche temps réel avec **FreeRTOS**, communique avec un PC via **UART**, et expose l'état des drones (position, batterie, activité).

---

## 🎯 Objectifs

- Simuler une flotte de drones embarqués
- Gérer leurs déplacements aléatoires, batterie, et atterrissage
- Communiquer avec un PC via UART pour affichage/commande
- Organiser la logique autour de tâches **FreeRTOS**
- Permettre une architecture évolutive, modulaire et multi-threadée

---

## 📁 Arborescence du dossier `Gestion_flotte/`

```text
Gestion_flotte/
├── FreeRTOS-Kernel/        (Kernel FreeRTOS)
├── include/                (Fichiers d'en-tête projet)
├── lib/                    (Librairies additionnelles : UART, HAL, etc.)
├── src/                    (Code source principal)
│   ├── main.c              (Init STM32 + tâches FreeRTOS)
│   ├── drone.c             (Logique individuelle d’un drone)
│   ├── flotte.c            (Gestion de la flotte – liste chaînée)
│   ├── uart_func.c         (Fonctions UART, files FreeRTOS)
│   └── stm32f4xx_it.c      (Interruptions Cortex-M4 et périphériques)
├── test/                   (Code de test unitaire/FreeRTOS)
└── platformio.ini          (Fichier de configuration PlatformIO)
```
---

## ⚙️ Fonctionnalités

### Gestion des drones

- Ajout/suppression dynamique de drones à la flotte
- Positions mises à jour aléatoirement à chaque tick
- Consommation de batterie + atterrissage automatique

### Tâches FreeRTOS

| Tâche                 | Description                               | Périodicité  |
|----------------------|-------------------------------------------|--------------|
| `Fleet_Manager`      | Met à jour la position des drones         | 1 Hz         |
| `Battery_Cons`       | Simule la décharge batterie               | 1 Hz         |
| `UART_Transmit`      | Envoie les messages UART                  | event-driven |
| `UART_Receive`       | Reçoit et affiche les commandes du PC     | event-driven |

---

## 🔌 Communication UART

- Port : **USART2**
- Baudrate : **115200**
- TX/RX : GPIOA PIN 2/3

### Exemple de message envoyé vers le PC :
- Drone 2 - Pos: (10240, 40960, 0) - Bat: 76 - Act: 1


### Traitement :

- Les messages sont formatés dans un buffer, puis envoyés dans une file (`UART_TXQueue`)
- L’interruption RX ajoute les données entrantes à `UART_RXQueue`

---

## 📚 Structure logicielle

### `drone.c`

- Implémente toutes les fonctions de bases pour gérer un drone (ajout, suppression, gestion des logs de postions, activation, etc...)
- Met à jour la position avec des pas pseudo-aléatoires
- Simule la décharge de batterie avec atterrissage forcé

### `flotte.c`

- Utilise une **liste chaînée** pour représenter la flotte (`FleetNode`). Idéalement un tableau statique serait mieux pour de l'embarqué.
- Permet d’ajouter, supprimer, chercher un drone, etc...
- Itère sur tous les drones pour mise à jour + affichage UART

### `main.c`

- Initialise HAL, UART, FreeRTOS, flotte
- Crée les tâches système
- Affiche périodiquement l’état de la flotte

---

## 🔐 Synchronisation & Sécurité

- Utilisation d’un `SemaphoreHandle_t` (`xFleetMutex`) pour protéger l'accès à la flotte entre tâches concurrentes

---

## 🧪 Tests & Debug

- UART peut afficher des logs dans la console série
- La LED verte (PD12) peut être utilisée pour du debug visuel (`Blink_LED_Debug`)
- Les tâches peuvent être monitorées via une interface FreeRTOS ou un outil STLink

---

## ✅ Compilation (via PlatformIO)

> Assure-toi que PlatformIO est installé dans VSCode et que la carte STM32F407VG est connectée via ST-Link.

```bash
pio run
pio run --target upload


> Le fichier platformio.ini doit contenir la configuration suivante (exemple) :
[env:disco_f407vg]
platform = ststm32
board = disco_f407vg
framework = stm32cube
monitor_speed = 115200
``
---
## 🧑‍💻 Auteur

**[Grégoire Domer]**  
Projet étudiant / perso en systèmes embarqués, Qt et simulation temps réel

---

## 📄 Licence

Projet sous licence MIT. Libre utilisation, modification et distribution.
