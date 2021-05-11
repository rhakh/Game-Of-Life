import QtQuick 2.0

Item {
    anchors.fill: parent
    anchors.centerIn: parent

    Rectangle {
        anchors.fill: parent
        anchors.centerIn: parent
        color: "white"

        Text {
            id: helpText
            anchors.fill: parent
            anchors.centerIn: parent
            wrapMode: Text.WordWrap
            text: qsTr("This application is used as playground for GOF and testing efficiency of GOF algorithms.\n" +
                       "On the 'Home' page you can set random map or select map from file, customize speed and save to file resulted map.\n" +
                       "The 'Angorith analythis' tab contains from chart where you will be stored results of tests.\n" +
                       "To start test, you need to select executables, map, step and last generation. The chart updates each time after particular test run.")
        }
    }
}
