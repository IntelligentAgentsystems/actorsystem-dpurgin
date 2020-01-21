import QtQuick 2.0
import QtQuick.Controls 2.4

import AGS.Components 1.0 as Components
import AGS.Model 1.0 as Model

ApplicationWindow {
    width: 800
    height: 600
    visible: true

    Row {
        anchors.centerIn: parent


        Components.Laptop {
            anchors.verticalCenter: parent.verticalCenter

            model: Model.Laptop { }
        }

        Grid {
            columnSpacing: 10
            rowSpacing: 10

            columns: 2

            Components.Plotter {
                model: Model.Plotter {
                    position: Model.Plotter.NorthEastPos
                }
            }

            Components.Plotter {
                model: Model.Plotter {
                    position: Model.Plotter.NorthWestPos
                }
            }

            Components.Turntable {
                model: Model.Turntable {
                    position: Model.Turntable.WestPos
                }
            }

            Components.Turntable {
                model: Model.Turntable {
                    position: Model.Turntable.EastPos
                }
            }

            Components.Plotter {
                model: Model.Plotter {
                    position: Model.Plotter.SouthWestPos
                }
            }

            Components.Plotter {
                model: Model.Plotter {
                    position: Model.Plotter.SouthEastPos
                }

            }
        }

    }


}
