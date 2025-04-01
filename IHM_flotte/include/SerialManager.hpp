#ifndef UARTMANAGER_H
#define UARTMANAGER_H

#include <iostream>
#include <cstdlib>
#include <QObject>
#include <QSerialPort>
#include <QDebug>


/**
 * @class SerialManager
 * @brief Gère la communication série (UART) entre le PC et la carte STM32.
 *
 * La classe `SerialManager` encapsule l'utilisation de `QSerialPort` pour établir une liaison série
 * avec la carte STM32, configurer le port, lire les données entrantes, et envoyer des commandes.
 *
 * Elle fournit un mécanisme de réception asynchrone basé sur un signal Qt (`readyRead`) qui déclenche
 * la lecture du buffer et l'émission d'un signal `dataReceived(const QString&)`, connecté côté application
 * à la logique métier (ex : mise à jour de la flotte).
 *
 * ## Fonctionnalités :
 * - Ouverture et configuration du port série (baudrate, parité, etc.)
 * - Lecture asynchrone et découpage des messages par ligne (`\n`)
 * - Envoi de commandes vers la STM32 via UART
 * - Signal de réception (`dataReceived`) déclenché à chaque message complet

 * ## Utilisation typique :
 * - Connecter `dataReceived` à un slot/lambda pour parser les messages UART
 * - Utiliser `sendDataToSTM32()` pour envoyer une commande formatée depuis l’IHM
 *
 * @see QSerialPort (QtSerialPort)
 */


class SerialManager : public QObject {
    Q_OBJECT

    public : 
    explicit SerialManager(QObject *parent);
    ~SerialManager();
    void sendDataToSTM32(const QString &command);

    public slots :
    void handleReadyRead();

    signals:
    void dataReceived(const QString &msg);

    private : 
    QSerialPort Uart;
    QByteArray m_buffer; // Buffer pour accumuler les données UART
};


#endif // UARTMANAGER_H