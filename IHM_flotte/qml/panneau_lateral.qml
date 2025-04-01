import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: rec_panneau
    width: Constants.WIDTH / 5
    height: parent.height
    color: "grey"
    anchors.right: parent.right

    Flickable {
        id: flickable
        width: parent.width
        height: parent.height
        contentWidth: parent.width
        contentHeight: column_content.height
        clip: true

        Column {
            id: column_content
            width: flickable.width
            spacing: 10
            padding: 10

            Repeater {
                model: fleet.pdrones

                Rectangle {
                    width: column_content.width - 20
                    height: Constants.HEIGHT / 6
                    radius: 10
                    color: "#ffffff"
                    border.color: "#cccccc"

                    Column {
                        anchors.fill: parent
                        anchors.margins: 10

                        Text {
                            text: "Drone ID: " + model.pid
                            font.bold: true
                        }

                        Text {
                            text: "x: " + model.pposX.toFixed(1) + ", y: " + model.pposY.toFixed(1)
                        }

                        Text {
                            text: "Actif: " + model.pactive
                        }

                        Row {
                            spacing: 5
                            anchors.left: parent.left

                            Text {
                                text: "Batterie:"
                            }

                            Rectangle {
                                width: parent.width * 0.4
                                height: 12
                                border.color: "black"
                                border.width: 2
                                radius: 6

                                Rectangle {
                                    width: parent.width * (model.pbattery / 100)
                                    height: parent.height
                                    radius: 6
                                    z: 1
                                    color: model.pbattery <= 20 ? "#ff4c4c" : (model.pbattery <= 50 ? "#ffa500" : "#32cd32")
                                    // rouge si <= 20%, orange si <= 50%, vert sinon
                                }
                            }

                            Text {
                                text: model.pbattery + "%"
                                font.pixelSize: 10
                                verticalAlignment: Text.AlignVCenter
                            }
                        }


                    }
                }
            }
        }

        ScrollBar.vertical: ScrollBar {
            policy: ScrollBar.AlwaysOn
        }  
    }
}
