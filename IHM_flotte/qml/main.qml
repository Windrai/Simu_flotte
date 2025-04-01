import QtQuick 2.15
import QtQuick.Controls 2.15




ApplicationWindow {
    width: Constants.WIDTH
    height: Constants.HEIGHT
    visible: true
    title: "Fleet Visualization"

    Rectangle {
        anchors.fill: parent
        color: "white"


        // Map des drones
        Loader {
            source: "qrc:/map.qml"
            anchors.centerIn: parent
            z : 1
        }

        // Panneau latéral
  // Panneau latéral
        Loader {
            source: "qrc:/panneau_lateral.qml"
            anchors.right: parent.right
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            z : 2 
        }

    }
}
