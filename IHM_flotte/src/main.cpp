#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext> // Pour setContextProperty pex
#include <QTimer>  // Pour le QTimer
#include <QDebug>
#include <QSerialPortInfo>
#include <unistd.h>
#include "drone.hpp"
#include "flotte.hpp"
#include "SerialManager.hpp"




int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // Instanciation du gestionnaire de port série (assurez-vous qu'il est correctement configuré)
    SerialManager serialManager(nullptr);

    // Création d'une flotte vide (les drones seront mis à jour par les messages UART)
    Fleet fleet;

    // Supposons que le SerialManager émette un signal dataReceived(const QString&)
    // Vous pouvez connecter ce signal à une lambda qui met à jour la flotte :
    QObject::connect(&serialManager, &SerialManager::dataReceived,
                     [&fleet](const QString &msg) {
        // Conversion du message en std::string
        std::string message = msg.toStdString();
        fleet.updateDroneFromUARTMessage(message);
        fleet.print_fleet();
    });


/*
    // Pour tester sans matériel, on simule la réception d'un message UART toutes les 2 secondes.
    QTimer *uartTimer = new QTimer(&app);
    QObject::connect(uartTimer, &QTimer::timeout, [&fleet]() {
        // Exemple de message reçu (assurez-vous que le format correspond à celui attendu par sscanf)
        QString simulatedMessage = "Drone 3 - Pos: (300, 350, 400)";
        fleet.updateDroneFromUARTMessage(simulatedMessage.toStdString());
        fleet.print_fleet();
    });
    uartTimer->start(1000);  // Toutes les 2 secondes
*/
    // Exposer la flotte à QML
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("fleet", &fleet);

    // Exposons nos constantes sous forme de QVariantMap
    QVariantMap constants;
    constants["MIN_POS"] = Constants::MIN_POS;  // Ou utilise une méthode statique/constexpr
    constants["MAX_POS"] = Constants::MAX_POS;
    constants["RADIUS"] = Constants::RADIUS;
    constants["HEIGHT"] = Constants::HEIGHT;
    constants["WIDTH"] = Constants::WIDTH;

    engine.rootContext()->setContextProperty("Constants", constants);



    // On charge le fichier QML qui contient l'interface utilisateur dans l'application
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    

    // On vérifie que le fichier QML est correctement chargé. L'application se ferme sinon.
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                 &app, [url](QObject* obj, const QUrl& objUrl) {
                     if (!obj && url == objUrl)
                         QCoreApplication::exit(-1);
                 }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}


