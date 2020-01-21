import QtQuick 2.12

import AGS.Model 1.0 as Model

Item {
    implicitHeight: 200
    implicitWidth: 200

    property Model.Turntable model

    Image {
        anchors.fill: parent
        source: "qrc:/images/turntable.png"

        rotation: {
            switch (model.direction) {
            case Model.Turntable.NorthDir: return 0
            case Model.Turntable.WestDir: return 270
            case Model.Turntable.SouthDir: return 180
            case Model.Turntable.EastDir: return 90
            }
        }

        Behavior on rotation {
            RotationAnimation {
                duration: 1000
                direction: RotationAnimation.Counterclockwise
            }
        }
    }

}
