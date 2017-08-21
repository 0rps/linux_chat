TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    client.cpp \
    server.cpp \
    message.cpp \
    messageparser.cpp

QMAKE_CXXFLAGS += -std=c++11

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    client.h \
    server.h \
    utils.h \
    message.h \
    messageparser.h

