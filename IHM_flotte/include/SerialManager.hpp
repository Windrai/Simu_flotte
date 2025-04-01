#ifndef UARTMANAGER_H
#define UARTMANAGER_H

#include <iostream>
#include <cstdlib>
#include <QObject>
#include <QSerialPort>
#include <QDebug>



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