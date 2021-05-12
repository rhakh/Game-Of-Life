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
    width: minimumWidth
    minimumWidth: 640
    minimumHeight: 480 + tabBar.height + 3

    color: "#09102B"
    title: qsTr("Game of Life")

    header: TabBar {
        id: tabBar
        TabButton {
            text: qsTr("Home")
        }
        TabButton {
            text: qsTr("Algorithm analysis")
        }
        TabButton {
            text: qsTr("Help")
        }

        onCurrentIndexChanged: {
            switch (currentIndex)
            {
            case 0:
                loader.source = "qrc:/Home.qml"
                break;
            case 1:
                loader.source = "qrc:/Algorithm_analysis.qml"
                break;
            case 2:
                loader.source = "qrc:/Help.qml"
                break;
            }

        }
    }

    Loader {
        id: loader
        anchors.fill: parent
        source: "qrc:/Home.qml"
    }
}
