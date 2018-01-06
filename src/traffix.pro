QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

# TODO: MIGHT NEED TO CHANGE THIS TARGET PATH
TARGET = framework
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# WHENEVER A HEADER/SOURCE FILE IS ADDED THAT IS INVOLVED IN THE GUI, IT MUST BE INCLUDED HERE:
SOURCES += \
        main.cpp \
        CityGenerator.cpp \
        ConsoleDriver.cpp \
        GUIDriver.cpp \
        Simulation.cpp \
        controller/Controller.cpp \
        controller/PretimedController.cpp \
        controller/BasicController.cpp \
        gui/gui.cpp \
        framework/Car.cpp \
        framework/DijkstraDirectedSP.cpp \
        framework/Intersection.cpp \
        framework/Point2D.cpp \
        framework/RoadSegment.cpp \
        framework/TrafficLight.cpp \
        framework/WeightedDigraph.cpp

HEADERS += \
        CityGenerator.h \
        ConsoleDriver.h \
        GUIDriver.h \
        Simulation.h \
        gui/gui.h \
        controller/Controller.h \
        controller/PretimedController.h \
        controller/BasicController.h \
        misc/pair_hash.h \
        framework/Framework.h

FORMS += \
        gui/gui.ui

RESOURCES += \
        resources.qrc
