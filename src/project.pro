#-------------------------------------------------
#
# Project created by QtCreator 2013-09-08T10:44:46
#
#-------------------------------------------------

QT       += core
QT       -= gui

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += opencv
INCLUDEPATH +=  /usr/include/eigen3/

TARGET = lf_rec
TEMPLATE = app

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

HEADERS		=     *.h
SOURCES		=     *.cpp
        
QMAKE_CXXFLAGS += -std=c++0x     
QMAKE_CXXFLAGS += -Wall -fopenmp
LIBS += -fopenmp
LIBS +=  -lhdf5 -lhdf5_hl -lz #-lopencv_core -lopencv_gpu -lopencv_imgproc -lopencv_highgui
OBJECTS_DIR = ./obj
