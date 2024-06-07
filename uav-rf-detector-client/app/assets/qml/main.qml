import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts
import Qt.labs.qmlmodels

ApplicationWindow {
    visible: true
    id: window
    width: 1600
    height: 900
    minimumWidth: 1280
    minimumHeight: 720

    Material.theme: Material.Dark
    Material.accent: Material.Green

    ColumnLayout {
        anchors.fill: parent

        RowLayout {
            width: window.width
            Layout.fillHeight: true

            RowLayout {
                Layout.fillWidth: true

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumWidth: window.width / 2
                    Layout.maximumWidth: window.width / 2
                    Layout.maximumHeight: 75
                    radius: 10
                    color: 'black'

                    Label {
                        text: "Текущий статус"
                        anchors.centerIn: parent
                        color: 'white'
                    }
                }

                Button {
                    Layout.alignment: Qt.AlignLeft
                    Layout.preferredWidth: 200
                    Layout.leftMargin: 25
                    text: qsTr("Включить поиск")
                }

                Item {
                    // spacer item
                    Layout.fillWidth: true
                }

                Rectangle {
                    Layout.alignment: Qt.AlignRight
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.maximumWidth: 25
                    Layout.maximumHeight: 25
                    Layout.rightMargin: 25
                    color: 'black'
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            ColumnLayout {

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumWidth: window.width / 2
                    Layout.maximumWidth: window.width / 2
                    radius: 10
                    color: 'black'

                    Label {
                        text: "Спектр\n      и\nВодопад"
                        anchors.centerIn: parent
                        color: 'white'
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.minimumWidth: window.width / 2
                Layout.maximumWidth: window.width / 2
                radius: 10
                color: 'black'

                ResizableColumnsTableView {
                    anchors.fill: parent
                    columnWidths: [75, 125, 200, 175]
                    model: TableModel {
                            TableModelColumn { display: "number" }
                            TableModelColumn { display: "name" }
                            TableModelColumn { display: "frequency" }
                            TableModelColumn { display: "detected-date-time" }

                            rows: [
                                {
                                    "number": 1,
                                    "name": "DJI",
                                    "frequency": 2400.00,
                                    "detected-date-time": "11:22 07.06.24"
                                },
                            ]
                        }
                    headerModel: ["Номер", "Название", "Несущая Частота, МГц", "Дата обнаружения"]
                }
            }
        }
    }
}
