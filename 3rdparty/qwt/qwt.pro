# -*- mode: sh -*- ###########################
# Qwt Widget Library
# Copyright (C) 1997   Josef Wilgen
# Copyright (C) 2002   Uwe Rathmann
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the Qwt License, Version 1.0
##############################################

# pro file for building the makefile for qwt
#
# Please read the instructions in INSTALL, if you don´t know
# how to use it.
#
# HEADERS/SOURCES are separated into plot
# relevant files and others. If you are 
# interested in QwtPlot only, you can remove
# the lists at the end of this file.

TARGET            = qwt
TEMPLATE          = lib

# Restrict usage of VERSION to unix.  When VERSION is used on win32, the TARGET
# is named differently by tmake (QWT420.DLL) and qmake (QWT.DLL).
unix:VERSION      = 4.2.0

# Only one of the following flags is allowed !
CONFIG           += qt
#CONFIG           += qtopia

CONFIG           += thread
CONFIG           += warn_on 
CONFIG           += release

# Only one of the following flags is allowed !
#CONFIG           += dll 
CONFIG           += staticlib

#DEFINES          += QT_NO_CAST_ASCII

# Qt 4 compatibility flags
#QT               += compat
#DEFINES          += QT_COMPAT

MOC_DIR           = ../../tmp/qwt
OBJECTS_DIR       = ../../tmp/qwt
DESTDIR           = lib
INCLUDEPATH      += include
DEPENDPATH       += include src

qtopia:CONFIG       -= thread

# Unfortunately tmake doesn´t support win32:dll:DEFINES, but 
# QT_DLL QWT_DLL QWT_MAKEDLL have no effect on other platforms
# than win32

dll:DEFINES    += QT_DLL QWT_DLL QWT_MAKEDLL

# The XOR raster operation does not work with text on the
# X Window system with XFT (X FreeType) enabled.
# Qwt works around this problem.
# If you are building Qwt for Qt-3.x,
# use of XFT is automatically detected.
# If you are building Qwt for Qt-2.3 and if the command 
# ldd $QTDIR/lib/libqt.so.2.3
# lists libXft.so, then uncomment the following line:  

#DEFINES        += QWT_BROKEN_RASTEROP_FONT

# Note, Qt-2.3 uses Xft only if the environment variable
# is set to "true", "yes", or "1".

# Current gcc (2.95) doesn't handle large C++ projects
# very well. Because KDE and Qt libraries result 
# have reached critical sizes, Linux distributors
# reduce them by excluding support for exceptions.
# If you like to use exceptions you will notice a core
# dump whenever one of your exceptions will pass code
# that is not compiled with exception handling.
# If you need exceptions remove the line below and
# be aware that you propably have to recompile your own
# Qt library as well.

linux-g++:TMAKE_CXXFLAGS += -fno-exceptions 

#unix:target.path=/usr/lib
#unix:INSTALLS += target

# "CONFIG -= exceptions" doesn´t seem to work for
# many environments. So we have to do it using
# the compiler flags. The following is for MSVC.

# win32-msvc:TMAKE_CXXFLAGS += /GX-

HEADERS = \
    include/qwt_global.h \
    include/qwt.h \
    include/qwt_array.h \
    include/qwt_autoscl.h \
    include/qwt_curve.h \
    include/qwt_data.h \
    include/qwt_dimap.h \
    include/qwt_double_rect.h \
    include/qwt_event_pattern.h \
    include/qwt_grid.h \
    include/qwt_layout_metrics.h \
    include/qwt_legend.h \
    include/qwt_marker.h \
    include/qwt_math.h \
    include/qwt_painter.h \
    include/qwt_paint_buffer.h \
    include/qwt_picker.h \
    include/qwt_picker_machine.h \
    include/qwt_plot.h \
    include/qwt_plot_layout.h \
    include/qwt_plot_printfilter.h \
    include/qwt_plot_classes.h \
    include/qwt_plot_item.h \
    include/qwt_plot_dict.h \
    include/qwt_plot_canvas.h \
    include/qwt_plot_picker.h \
    include/qwt_plot_zoomer.h \
    include/qwt_push_button.h \
    include/qwt_rect.h \
    include/qwt_scale.h \
    include/qwt_scldiv.h \
    include/qwt_scldraw.h \
    include/qwt_spline.h \
    include/qwt_symbol.h \
    include/qwt_dyngrid_layout.h

SOURCES = \
    src/qwt_autoscl.cpp \
    src/qwt_curve.cpp \
    src/qwt_data.cpp \
    src/qwt_dimap.cpp \
    src/qwt_double_rect.cpp \
    src/qwt_event_pattern.cpp \
    src/qwt_grid.cpp \
    src/qwt_layout_metrics.cpp \
    src/qwt_legend.cpp \
    src/qwt_marker.cpp \
    src/qwt_math.cpp \
    src/qwt_painter.cpp \
    src/qwt_paint_buffer.cpp \
    src/qwt_picker.cpp \
    src/qwt_picker_machine.cpp \
    src/qwt_plot.cpp \
    src/qwt_plot_axis.cpp \
    src/qwt_plot_curve.cpp \
    src/qwt_plot_grid.cpp \
    src/qwt_plot_item.cpp \
    src/qwt_plot_print.cpp \
    src/qwt_plot_marker.cpp \
    src/qwt_plot_layout.cpp \
    src/qwt_plot_printfilter.cpp \
    src/qwt_plot_canvas.cpp \
    src/qwt_plot_picker.cpp \
    src/qwt_plot_zoomer.cpp \
    src/qwt_push_button.cpp \
    src/qwt_rect.cpp \
    src/qwt_scale.cpp \
    src/qwt_scldiv.cpp \
    src/qwt_scldraw.cpp \
    src/qwt_spline.cpp \
    src/qwt_symbol.cpp \
    src/qwt_dyngrid_layout.cpp
 
##############################################
# If you are interested in the plot widget
# only, you can remove the following 
# HEADERS/SOURCES lists.
##############################################

HEADERS += \
    include/qwt_arrbtn.h \
    #include/qwt_analog_clock.h \
    #include/qwt_compass.h \
    #include/qwt_compass_rose.h \
    #include/qwt_dial.h \
    #include/qwt_dial_needle.h \
    include/qwt_counter.h \
    include/qwt_drange.h \
    #include/qwt_knob.h \
    #include/qwt_sclif.h \
    #include/qwt_sldbase.h \
    #include/qwt_slider.h \
    include/qwt_text.h \
    #include/qwt_thermo.h \
    #include/qwt_wheel.h

SOURCES += \
    src/qwt_arrbtn.cpp \
    #src/qwt_analog_clock.cpp \
    #src/qwt_compass.cpp \
    #src/qwt_compass_rose.cpp \
    #src/qwt_dial.cpp \
    #src/qwt_dial_needle.cpp \
    src/qwt_counter.cpp \
    src/qwt_drange.cpp \
    #src/qwt_knob.cpp \
    #src/qwt_sclif.cpp \
    #src/qwt_sldbase.cpp \
    #src/qwt_slider.cpp \
    src/qwt_text.cpp \
    #src/qwt_thermo.cpp \
    #src/qwt_wheel.cpp

#debug:DEFINES += QWT_DEBUG
