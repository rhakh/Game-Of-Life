import QtQml 2.12
import QtQuick 2.12
import QtCharts 2.15
import QtQuick.Window 2.3
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.3
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
            title: ""
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
                labelFormat: qsTr("%d ms")
                gridVisible: true
                tickCount: 5
                min: 0
                max: 10
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
                text: qsTr("Select algorithms")
                Layout.leftMargin: 10
                Layout.alignment: Qt.AlignLeft
                onClicked: {
                    openMultFilesDialog.open()
                }
            }

            Button {
                text: qsTr("Select map")
                Layout.alignment: Qt.AlignLeft
                onClicked: {
                    openFileDialog.open()
                }
            }

            TextField {
                id: finishGeneration
                Layout.alignment: Qt.AlignLeft
                placeholderText: qsTr("Last Generation")
                validator: RegExpValidator { regExp: /[0-9A-F]+/ }
                inputMethodHints: Qt.ImhDigitsOnly
                selectByMouse: true
                onTextChanged: {
                    var generation = finishGeneration.text;
                    axisX.max = (axisX.max > generation ? axisX.max : generation);
                }
            }

            TextField {
                id: step
                Layout.alignment: Qt.AlignLeft
                placeholderText: qsTr("Step")
                validator: RegExpValidator { regExp: /[0-9A-F]+/ }
                inputMethodHints: Qt.ImhDigitsOnly
                selectByMouse: true
            }

            Button {
                text: isRunning ? "◼" : "▶"
                anchors.rightMargin: 10
                Layout.alignment: Qt.AlignLeft
                property var series: new Map();
                property bool isRunning: false
                property int lastGeneration: 0
                onClicked: {
                    if (!isRunning) {
                        startTests();
                        isRunning = true;
                    }
                    else {
                        stopTests();
                        isRunning = false;
                    }
                }

                function startTests() {
                    // clear chart series
                    chart.removeAllSeries();
                    series.clear();

                    // TODO: validate settings

                    // setup settings
                    algorithmRunner.setup(openMultFilesDialog.choosenFiles, finishGeneration.text, step.text, openFileDialog.choosenMap)
                    lastGeneration = finishGeneration.text

                    // run algorithms and then add results to chart by callback function
                    algorithmRunner.run(function(file, time, generation) {
                        file = basename(file)
                        console.log("File: " + file)
                        console.log("Time: " + time)
                        console.log("Generation: " + generation)

                        if (series.has(file)) {
                            console.log("Series has: " + file);
                            series.get(file).append(generation, time);
                        } else {
                            console.log("Series has not: " + file);
                            var line = chart.createSeries(ChartView.SeriesTypeLine, file, axisX, axisY);
                            line.append(generation, time);
                            line.pointsVisible = true;
                            series.set(file, line)
                        }

                        axisY.max = (axisY.max > time ? axisY.max : time);
                        axisX.max = (axisX.max > generation ? axisX.max : generation);
                        if (generation === lastGeneration)
                            isRunning = false;
                    })
                }

                function stopTests() {
                    algorithmRunner.stop();
                }
            }
        }
    }

    FileDialog {
        id: openMultFilesDialog
        title: qsTr("Please choose an executable files")
        fileMode: FileDialog.OpenFiles
        folder: ""
        property variant choosenFiles: []
        onAccepted: {
            console.log("You chose: " + openMultFilesDialog.files)
            var files = openMultFilesDialog.files;

            // clear previous files
            choosenFiles = [];
            for (var i = 0; i < files.length; i++) {
                // remove prefixed "file://"
                files[i] = files[i].replace(/^(file:\/{2})/, "");
                files[i] = decodeURIComponent(files[i]);
                choosenFiles.push(files[i])
            }

            console.log("QML: Files = " + choosenFiles)
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    FileDialog {
        id: openFileDialog
        title: "Please choose a map"
        fileMode: FileDialog.OpenFile
        folder: ""
        property var choosenMap
        onAccepted: {
            choosenMap = ""
            console.log("You chose: " + openFileDialog.file)
            var path = openFileDialog.file.toString();
            // remove prefixed "file://"
            path = path.replace(/^(file:\/{2})/, "");
            path = decodeURIComponent(path);
            // TODO: check result
            choosenMap = path
        }
        onRejected: {
            console.log("Canceled")
        }
    }

    function basename(str) {
        return (str.slice(str.lastIndexOf("/")+1))
    }
}
