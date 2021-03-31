import QtQml 2.12
import QtQuick 2.12
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import GOL 1.0

ApplicationWindow {
    id: root
    visible: true
    width: 640
    height: 480
    minimumWidth: 640
    minimumHeight: 480

    color: "#09102B"
    title: qsTr("Game of Life")

    FileDialog {
        id: openFileDialog
        title: "Please choose a file"
        folder: ""
        selectMultiple: false
        width: root.width
        height: root.height
        onAccepted: {
            console.log("You chose: " + openFileDialog.fileUrls)
            gameOfLifeModel.loadFile(openFileDialog.fileUrl.toString()) // TODO: check result
            generation.text = qsTr("Generation: 0")
            tableView.forceLayout()
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    TableView {
        id: tableView
        anchors.fill: parent
        anchors.centerIn: parent

        rowSpacing: 1
        columnSpacing: 1

        ScrollBar.horizontal: ScrollBar {}
        ScrollBar.vertical: ScrollBar {}

        delegate: Rectangle {
            id: cell
            implicitWidth: 8
            implicitHeight: 8

            required property var model
            required property int value

            // two color
            // color: value & 0x01 ? "#6200EE" : "#EFE5FD"

            // multi color
            color: {
                // console.log("value = " + value.toString() + ", 0x01 = " + (value & 0xfe).toString() + ", >> 1 = " + ((value & 0x01) >> 1).toString())
                if (value & 0x01) {
                    switch ((value & 0xfe) >> 1) {
                    case 1:
                        return "#fc1e0f" // red
                    case 2:
                        return "#FFFF00" // yellow
                    case 3:
                        return "#76FF03" // green
                    case 4:
                        return "#00E5FF" // blue
                    case 5:
                        return "#E040FB" // purp
                    case 6:
                    case 7:
                    case 8:
                        return "#1DE9B6" // ocean
                    }
                } else {
                    return "#E3F2FD"
                }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: parent.model.value = !parent.value
            }
        }

        model: GameOfLifeModel {
            id: gameOfLifeModel
        }

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
                text: qsTr("New")
                Layout.leftMargin: 10
                Layout.alignment: Qt.AlignLeft
                onClicked: {
                    gameOfLifeModel.randomFillMap()
                }
            }

            Button {
                text: qsTr("Load")
                Layout.alignment: Qt.AlignLeft
                onClicked: {
                    openFileDialog.open()
                }
            }

            Button {
                text: qsTr("Save")
                Layout.alignment: Qt.AlignLeft
                onClicked: {

                }
            }

            Slider {
                id: slider
                from: 0
                to: 1
                value: 0.05
                implicitWidth: 100
                Layout.alignment: Qt.AlignLeft
            }

            Button {
                text: qsTr("Next Step")
                Layout.alignment: Qt.AlignLeft
                enabled: !playButton.isRunning
                onClicked: {
                    generation.text = qsTr("Generation: ") + gameOfLifeModel.getIteration()
                    gameOfLifeModel.nextStep()
                }
            }

            Button {
                property bool isRunning: false
                id: playButton
                text: timer.running ? "❙❙" : "▶️"
                Layout.alignment: Qt.AlignLeft
                focus: true
                onClicked:  {
                    isRunning = !isRunning
                    timer.running = !timer.running
                }
            }

            Label {
                id: generation
                text: qsTr("Generation: 0");
                Layout.alignment: Qt.AlignLeft
            }

            Item {
                Layout.fillWidth: true
            }
        }

        Timer {
            id: timer
            interval: 1000 * slider.value + 1
            running: false
            repeat: true

            onTriggered: {
                generation.text = qsTr("Generation: ") + gameOfLifeModel.getIteration()
                gameOfLifeModel.nextStep()
            }
        }
    }
}
