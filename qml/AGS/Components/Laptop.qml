import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import AGS.Model 1.0 as Model

Item {
    id: root

    implicitHeight: layout.height
    implicitWidth: layout.width

    property Model.Laptop model

    ColumnLayout {
        id: layout

        Image {
            source: "qrc:/images/laptop.png"
        }

        TextField {
            id: textEdit

            Layout.fillWidth: true

            placeholderText: "Sequence of C, M, Y, or K"

            validator: RegExpValidator {
                regExp: /^[CMYKcmyk]+$*/
            }
        }

        Button {
            Layout.alignment: Qt.AlignHCenter
            text: "Print"
            enabled: textEdit.acceptableInput

            onClicked: {
                root.model.print(textEdit.text)
                textEdit.text = ""
            }
        }
    }

}
