import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

import AGS.Model 1.0 as Model

Item {
    id: root

    implicitHeight: layout.height
    implicitWidth: layout.width

    property Model.Laptop model
    property real colorButtonSize: 25

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

        Row {
            Rectangle {
                width: colorButtonSize
                height: colorButtonSize
                color: "cyan"
                MouseArea {
                    anchors.fill: parent
                    onClicked: textEdit.text += "C"
                    cursorShape: Qt.PointingHandCursor
                }
            }
            Rectangle {
                width: colorButtonSize
                height: colorButtonSize
                color: "magenta"
                MouseArea {
                    anchors.fill: parent
                    onClicked: textEdit.text += "M"
                    cursorShape: Qt.PointingHandCursor
                }
            }
            Rectangle {
                width: colorButtonSize
                height: colorButtonSize
                color: "yellow"
                MouseArea {
                    anchors.fill: parent
                    onClicked: textEdit.text += "Y"
                    cursorShape: Qt.PointingHandCursor
                }
            }
            Rectangle {
                width: colorButtonSize
                height: colorButtonSize
                color: "black"
                MouseArea {
                    anchors.fill: parent
                    onClicked: textEdit.text += "K"
                    cursorShape: Qt.PointingHandCursor
                }
            }
        }

        Button {
            Layout.topMargin: 5
            Layout.alignment: Qt.AlignHCenter
            text: "Print"
            enabled: textEdit.acceptableInput

            onClicked: {
                root.model.print(textEdit.text)
                textEdit.text = ""
            }
        }

        Label {
            Layout.topMargin: 5
            text: "Printed: " + model.printed
        }

        Label {
            text: "Left: " + model.leftToPrint
        }
    }

}
