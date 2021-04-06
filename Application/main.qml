import QtQml 2.12
import QtQuick 2.12
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.2
import GOL 1.0
import Qt.labs.platform 1.1

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

            color: {
                if (value & 0x01) {
                    switch ((value & 0xfe) >> 1) {
                    case 0:
                        return "#EC407A" // pink
                    case 1:
                        return "#FC1E0F" // red
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
                onClicked: {
                    parent.model.value = !parent.model.value
                }
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
                    saveFileDialog.open()
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

    FileDialog {
        id: openFileDialog
        title: "Please choose a file"
        fileMode: FileDialog.OpenFile
        folder: ""
        onAccepted: {
            console.log("You chose: " + openFileDialog.file)
            var path = openFileDialog.file.toString();
            // remove prefixed "file://"
            path = path.replace(/^(file:\/{2})/, "");
            path = decodeURIComponent(path);
            gameOfLifeModel.loadFile(path); // TODO: check result
            generation.text = qsTr("Generation: 0")
            tableView.forceLayout()
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    FileDialog {
        id: saveFileDialog
        title: "Save a file"
        fileMode: FileDialog.SaveFile
        folder: ""
        onAccepted: {
            console.log("You chose: " + saveFileDialog.file)
            var path = saveFileDialog.file.toString();
            // remove prefixed "file://"
            path = path.replace(/^(file:\/{2})/, "");
            path = decodeURIComponent(path);
            gameOfLifeModel.saveToFile(path); // TODO: check result
        }
        onRejected: {
            console.log("Canceled")
        }
    }
}
