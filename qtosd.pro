TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += . src
MOC_DIR = .moc
OBJECTS_DIR = .obj
UI_DIR = .ui
RCC_DIR = .rcc
QMAKE_CXXFLAGS = -I/usr/include/alsa -lasound
QMAKE_LFLAGS += -lasound
QT += svg network dbus
CONFIG += debug

# Input
RESOURCES = qtosd.qrc
HEADERS += src/osd.h src/mixer.h src/reader.h src/readerserver.h src/dbus.h
FORMS += ui/osd.ui
SOURCES += src/main.cpp src/osd.cpp src/mixer.cpp src/reader.cpp src/readerserver.cpp src/dbus.cpp
