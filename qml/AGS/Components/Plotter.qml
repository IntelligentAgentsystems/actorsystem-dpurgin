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
            console.log("model.color in rectangle:" + root.model.color)
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

    Component.onCompleted: {
        console.log("model.color: " + model.color)
    }
}
