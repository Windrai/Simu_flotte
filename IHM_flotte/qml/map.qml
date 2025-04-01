import QtQuick 2.15
import QtQuick.Controls 2.15

Flickable {
    id: fleetMap
    width: 400 + Constants.RADIUS
    height: 500 + Constants.RADIUS
    contentWidth: mapContainer.width * fleetMap.zoomLevel
    contentHeight: mapContainer.height * fleetMap.zoomLevel
    clip: true

    // Propriété pour gérer le facteur de zoom
    property real zoomLevel: 1.0

    // Repère orthonormé fixe
    Canvas {
        id: fixedGridCanvas
        width: fleetMap.width
        height: fleetMap.height
        anchors.fill: parent
        z: 1
        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);

            ctx.strokeStyle = "gray";
            ctx.lineWidth = 1;
            ctx.fillStyle = "black";
            ctx.font = "10px sans-serif";

            // Valeurs du monde logique
            let minVal = 0;
            let maxVal = 50000;
            let logicalStep = 10000;

            // Conversion logique → pixels
            function logicalToPixel(val, axisSize) {
                return (val - minVal) / (maxVal - minVal) * axisSize;
            }

            // Tracer lignes verticales
            for (let val = minVal; val <= maxVal; val += logicalStep) {
                let x = logicalToPixel(val, width);
                ctx.beginPath();
                ctx.moveTo(x, 0);
                ctx.lineTo(x, height);
                ctx.stroke();
                ctx.fillText(val.toString(), x + 2, 10);
            }

            // Tracer lignes horizontales
            for (let val = minVal; val <= maxVal; val += logicalStep) {
                let y = logicalToPixel(val, height);
                ctx.beginPath();
                ctx.moveTo(0, y);
                ctx.lineTo(width, y);
                ctx.stroke();
                ctx.fillText(val.toString(), 2, y - 2);
            }
        }

    }

    Rectangle {
        id: mapContainer
        width: fleetMap.width 
        height: fleetMap.height 
        color: "lightblue"
        border.color: "black"
        border.width: 2
        z: 0

        // Zone contenant les éléments zoomables
        Item {
            id: zoomableArea
            width: mapContainer.width * fleetMap.zoomLevel
            height: mapContainer.height * fleetMap.zoomLevel

            // Gestion du zoom avec la molette
            MouseArea {
                anchors.fill: parent
                onWheel: {
                    if (wheel.angleDelta.y > 0) {
                        fleetMap.zoomLevel = Math.min(fleetMap.zoomLevel + 0.1, 5.0) // Limite supérieure
                    } else {
                        fleetMap.zoomLevel = Math.max(fleetMap.zoomLevel - 0.1, 0.5) // Limite inférieure
                    }
                    fleetMap.contentWidth = mapContainer.width * fleetMap.zoomLevel
                    fleetMap.contentHeight = mapContainer.height * fleetMap.zoomLevel
                    fixedGridCanvas.requestPaint(); // Redessiner le repère lors du zoom
                }
            }

            // Affichage des drones avec le zoom appliqué individuellement
            Repeater {
                model: fleet.pdrones
                delegate: Rectangle {
                    width: 20 * fleetMap.zoomLevel
                    height: 20 * fleetMap.zoomLevel
                    radius: Constants.RADIUS * fleetMap.zoomLevel
                    color: "red"
                    x: (pposX - Constants.MIN_POS) / (Constants.MAX_POS - Constants.MIN_POS) * zoomableArea.width
                    y: (pposY - Constants.MIN_POS) / (Constants.MAX_POS - Constants.MIN_POS) * zoomableArea.height
                    Text {
                        text: "ID: " + pid
                        anchors.centerIn: parent
                        font.pixelSize: 10 * fleetMap.zoomLevel
                        font.bold: true
                        color: "black"
                    }
                    Behavior on x {
                        NumberAnimation { duration: 1000 }
                    }
                    Behavior on y {
                        NumberAnimation { duration: 1000 }
                    }
                }
            }
        }
    }
}
