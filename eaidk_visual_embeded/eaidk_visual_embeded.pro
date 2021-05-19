#-------------------------------------------------
#
# Project created by QtCreator 2016-09-05T20:04:12
#
#-------------------------------------------------

QT       += core gui
QT += multimedia

QT += multimediawidgets
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = eaidk_visual_embeded
TEMPLATE = app
QMAKE_CFLAGS = -DUSE_SDK_FACEDB -g
QMAKE_CXXFLAGS = -DUSE_SDK_FACEDB -g

SOURCES += main.cpp\
        mainwindow.cpp \
    source/camera.cpp \
    source/face_attr/CvxText.cpp \
    source/AlgThread.cpp \
    source/thread.cpp \
    source/config.cpp \
    source/util.cpp \
    source/lock.cpp

INCLUDEPATH += /usr/include/freetype2

LIBS += /usr/lib64/libopencv_*.so \
        /usr/local/AID/Tengine/lib/libtengine.so \
        /usr/local/AID/Tengine/lib/libhclcpu.so \
        ../libs/libface.so \
        /usr/lib64/libpthread.so \
        /usr/lib64/libfreetype.so

HEADERS  += mainwindow.h \
    header/camera.h \
    header/include/tengine_c_api.h \
    header/face_attr/faceapp.hpp \
    header/face_attr/CvxText.hpp \
    source/AlgThread.h \
    source/thread.h \
    source/config.h \
    source/util.h \
    source/lock.h \
    source/AlgThread.h

FORMS    += mainwindow.ui
