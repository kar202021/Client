QT += core gui network widgets
TARGET = Client
CONFIG += c++11
TEMPLATE = app
SOURCES += main.cpp \
    business/clientlogic.cpp \
    ui/clientwindow.cpp
HEADERS += \
    business/clientlogic.h \
    ui/clientwindow.h
FORMS += \
    ui/clientwindow.ui
