import QtQuick 2.12

import AGS.Model 1.0 as Model

Item {
    id: root

    implicitWidth: 200
    implicitHeight: 200

    property Model.Plotter model

    Image {
        anchors.fill: parent
        source: "qrc:/images/plotter.png"
    }

    Rectangle {
        width: 25
        height: 25
        anchors.centerIn: parent

        color: {
            switch (root.model.color) {
            case Model.Plotter.Cyan:
                return "cyan";
            case Model.Plotter.Yellow:
                return "yellow";
            case Model.Plotter.Magenta:
                return "magenta";
            case Model.Plotter.Black:
                return "black";
            default:
                return "red";
            }
        }
    }

    Image {
        anchors.centerIn: parent
        source: "qrc:/images/paper.png"
        visible: model.loaded

        scale:  0.5
    }
}
