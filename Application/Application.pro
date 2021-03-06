QT += charts qml quick
QT += quickcontrols2

CONFIG += c++14

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
        AlgorithmRunner.cpp \
        GameOfLifeModel.cpp \
        main.cpp

LIBS += -lboost_system \
        -lboost_filesystem  \
        -lboost_program_options \
        -pthread

macx {
   LIBS += -L /usr/local/Cellar/boost/1.75.0_2/lib \
            -lboost_thread-mt
   INCLUDEPATH += /usr/local/Cellar/boost/1.75.0_2/include
}

unix:!macx {
    LIBS += -lboost_thread
    INCLUDEPATH += /usr/include/boost
}


CCFLAG += -O2
RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

QML_IMPORT_NAME = GameOfLifeModel

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    AlgorithmRunner.h \
    GameOfLifeModel.hpp

DISTFILES += \
    qmldir
