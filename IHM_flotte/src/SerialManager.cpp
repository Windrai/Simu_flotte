#include "SerialManager.hpp"
#include <QDebug>

SerialManager::SerialManager(QObject *parent)
    : QObject(parent)
{
    Uart.setPortName("/dev/ttyUSB0");
    Uart.setBaudRate(QSerialPort::Baud115200);
    Uart.setParity(QSerialPort::NoParity);
    Uart.setDataBits(QSerialPort::Data8);
    Uart.setFlowControl(QSerialPort::NoFlowControl);
    Uart.setStopBits(QSerialPort::OneStop);

    if (Uart.open(QIODevice::ReadWrite)) {
        qDebug() << "Port série ouvert!";
        // Connecter le signal readyRead pour traiter les données dès leur arrivée.
        QObject::connect(&Uart, &QSerialPort::readyRead,
                         this, &SerialManager::handleReadyRead);
    } else {
        qDebug() << "Échec ouverture du port série!";
    }
}

void SerialManager::sendDataToSTM32(const QString &command)
{
    if (Uart.isOpen() && Uart.isWritable()){
        QByteArray data = command.toUtf8() + "\r\n";
        Uart.write(data);
        Uart.flush();
        qDebug() << "Commande envoyée";
    }
    else {
        qDebug() << "Erreur : Port UART non ouvert ou non accessible";
    }
}

void SerialManager::handleReadyRead(){
    m_buffer.append(Uart.readAll());

    // Tant qu'on a au moins un '\n' dans le buffer
    while (true) {
        int endIndex = m_buffer.indexOf('\n');
        if (endIndex < 0)
            break;  // Pas encore de message complet

        // Extraire la ligne (sans le '\n')
        QByteArray line = m_buffer.left(endIndex);
        m_buffer.remove(0, endIndex + 1);

        // Nettoyer la ligne (supprimer '\r' et espaces en début/fin)
        QString msg = QString::fromUtf8(line).trimmed();

        // Vérifier que le message n'est pas vide
        if (!msg.isEmpty()) {
            qDebug() << "Message complet reçu:" << msg;
            emit dataReceived(msg);
        }
    }
}

SerialManager::~SerialManager()
{
    if (Uart.isOpen()) {
        Uart.close();
    }
}
