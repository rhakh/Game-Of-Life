import QtQml 2.12
import QtQuick 2.12
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
// import GameOfLifeModel 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 640
    height: 480
    minimumWidth: 640
    minimumHeight: 480

    color: "#09102B"
    title: qsTr("Game of Life")

    TableView {
        id: tableView
        anchors.fill: parent

        rowSpacing: 1
        columnSpacing: 1

        ScrollBar.horizontal: ScrollBar {}
        ScrollBar.vertical: ScrollBar {}

        delegate: Rectangle {
            id: cell
            implicitWidth: 15
            implicitHeight: 15

            required property var model
            required property bool value

            color: value ? "white" : "black"

            MouseArea {
                anchors.fill: parent
                onClicked: parent.model.value = !parent.value
            }
        }

//        model: GameOfLifeModel {
//            id: gameOfLifeModel
//        }

        // scroll
        contentX: (contentWidth - width) / 2;
        contentY: (contentHeight - height) / 2;
    }

    footer: Rectangle {
        signal nextStep

        id: footer
        height: 50
        color: "#F3F3F4"

        RowLayout {
            id: row
            spacing: 10

            Button {
                text: qsTr("Load")
                Layout.leftMargin: 10
                Layout.alignment: Qt.AlignLeft
            }

            Button {
                text: qsTr("Save")
                Layout.alignment: Qt.AlignLeft
            }

            Slider {
                id: slider
                from: 0
                to: 1
                value: 0.25
                implicitWidth: 100
                Layout.alignment: Qt.AlignLeft
            }

            Button {
                text: timer.running ? "❙❙" : "▶️"
                Layout.alignment: Qt.AlignLeft
                onClicked: timer.running = !timer.running
            }

            Label {
                text: qsTr("Generation:");
                Layout.alignment: Qt.AlignLeft
            }

            Item {
                Layout.fillWidth: true
            }
        }

        Timer {
            id: timer
            interval: 1000 - (980 * slider.value)
            running: false
            repeat: true

            onTriggered: {

            }
        }
    }
}
