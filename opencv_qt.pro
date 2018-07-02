#-------------------------------------------------
#
# Project created by QtCreator 2018-07-02T10:38:40
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = opencv_qt
TEMPLATE = app


# The following lines tells Qmake to use pkg-config for opencv
QT_CONFIG -= no-pkg-config
CONFIG  += link_pkgconfig
PKGCONFIG += opencv
LIBS += -L/usr/local/lib
#LIBS += -L/usr/local/opt/opencv/lib#/usr/local/Cellar/opencv/3.4.1_6/lib
INCLUDEPATH += /usr/local/include
#INCLUDEPATH += /usr/local/Cellar/opencv/3.4.1_6/include

#LIBS += -L/opt/local/lib

LIBS += -lopencv_calib3d \
-lopencv_core \
-lopencv_features2d \
-lopencv_flann \
-lopencv_highgui \
-lopencv_imgproc \
-lopencv_objdetect \
-lopencv_video


LIBS += -stdlib=libc++

QMAKE_CXXFLAGS += -stdlib=libc++
QMAKE_CXXFLAGS += -std=c++11



# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
        mainwindow.cpp

HEADERS += \
        mainwindow.h

FORMS += \
        mainwindow.ui
