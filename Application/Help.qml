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
            text: qsTr("Help help help help")
        }
    }
}
