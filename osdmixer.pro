######################################################################
# Automatically generated by qmake (2.01a) So Jun 15 14:21:52 2008
######################################################################

TEMPLATE = app
TARGET = 
DEPENDPATH += .
INCLUDEPATH += .
QMAKE_CXXFLAGS = -I/usr/include/alsa -lasound
QMAKE_LFLAGS += -lasound
QT += svg

# Input
HEADERS += osd.h mixer.h reader.h
FORMS += osd.ui
SOURCES += main.cpp osd.cpp mixer.cpp reader.cpp
