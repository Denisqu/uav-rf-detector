import QtQuick 2.7
import QtQuick.Controls 2.3
import QtQuick.Controls.Material 2.12
import QtQuick.Layouts

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
        id: l1

        // TODO: нужно как-то растянуть r1 во весь экран
        RowLayout {
            id: r1
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {
                Layout.fillWidth: true

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumWidth: window.width / 2
                    Layout.maximumWidth: window.width / 2
                    Layout.maximumHeight: 75
                    color: 'black'

                    Label {
                        text: "Текущий статус"
                        anchors.centerIn: parent
                        color: 'white'
                    }
                }

                Button {
                    text: qsTr("Включить поиск")
                    //Material.background: Material.
                }

                Rectangle {
                    Layout.alignment: Qt.AlignRight
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.maximumWidth: 25
                    Layout.maximumHeight: 25
                    color: 'black'
                }
            }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            RowLayout {

                Button {
                    text: qsTr("Включить поиск")
                    //Material.background: Material.
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.minimumWidth: 50
                    Layout.preferredWidth: 100
                    Layout.maximumWidth: 300
                    Layout.minimumHeight: 75
                    color: black
                }
            }
        }
    }
}
