import QtQml 2.12
import QtQuick 2.12
import QtCharts 2.15
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
import QtQuick.Dialogs 1.3
import Qt.labs.platform 1.1

Item {
    anchors.fill: parent
    anchors.centerIn: parent

    Rectangle {
        id: background
        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: footer.top
        color: "white"

        ChartView {
            id: chart
            title: "Line"
            anchors.fill: parent
            antialiasing: true
            theme: ChartView.ChartThemeBlueCerulean

            ValueAxis {
                id: axisX
                gridVisible: true
                tickCount: 5
                min: 0
                max: 10
            }

            ValueAxis {
                id: axisY
                gridVisible: true
                tickCount: 5
                min: 0
                max: 10
            }

            LineSeries {
                name: "Serial"
                axisX: axisX
                axisY: axisY
                XYPoint { x: 0; y: 0 }
                XYPoint { x: 1.1; y: 2.1 }
                XYPoint { x: 1.9; y: 3.3 }
                XYPoint { x: 2.1; y: 2.1 }
                XYPoint { x: 2.9; y: 4.9 }
                XYPoint { x: 3.4; y: 3.0 }
                XYPoint { x: 4.1; y: 3.3 }
            }

            LineSeries {
                name: "MPI"
                axisX: axisX
                axisY: axisY
                XYPoint { x: 2; y: 3 }
                XYPoint { x: 2.1; y: 4.1 }
                XYPoint { x: 3.9; y: 5.3 }
                XYPoint { x: 4.1; y: 4.1 }
                XYPoint { x: 5.9; y: 6.9 }
                XYPoint { x: 6.4; y: 5.0 }
                XYPoint { x: 7.1; y: 5.3 }
            }
        }
    }

    Rectangle {
        id: footer
        height: 50
        color: "white"
        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        RowLayout {
            id: row
            spacing: 10

            Button {
                text: qsTr("Select")
                Layout.leftMargin: 10
                Layout.alignment: Qt.AlignLeft
                onClicked: {
                    openMultFilesDialog.open()
                }
            }

            Button {
                text: qsTr("Add")
                Layout.alignment: Qt.AlignLeft
                onClicked: {
                    var line = chart.createSeries(ChartView.SeriesTypeLine, "Concurrent", axisX, axisY);

                    line.append(0,0)
                    line.append(1.1,2.1)
                    line.append(4.5,4.5)

                    axisX.min = 0;
                    axisX.max = 10;

                    axisY.min = 0;
                    axisY.max = 10;

                    testRunner.run(function(file, time, generation) {
                        console.log("File: " + file)
                        console.log("Time: " + time)
                        console.log("Generation: " + generation)
                    })


                }
            }
        }
    }

    // drop-down list of all possible maps
    // drop-down list of possible generation

    FileDialog {
        id: openMultFilesDialog
        title: "Please choose a file"
        fileMode: FileDialog.OpenFiles
        folder: ""
        property variant editFiles: []
        onAccepted: {
            console.log("You chose: " + openMultFilesDialog.files)
            var files = openMultFilesDialog.files;

            // clear previous files
            editFiles = [];
            for (var i = 0; i < files.length; i++) {
                // remove prefixed "file://"
                files[i] = files[i].replace(/^(file:\/{2})/, "");
                files[i] = decodeURIComponent(files[i]);
                editFiles.push(files[i])
            }

            console.log("QML: Files = " + editFiles)
            testRunner.setup(editFiles, [10, 100, 500], "/home/rhakh/Game-Of-Life/Patterns/puffer-train.txt")
        }
        onRejected: {
            console.log("Canceled")
        }
    }
}
