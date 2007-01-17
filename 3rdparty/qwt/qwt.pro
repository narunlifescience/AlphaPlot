QMAKE_PROJECT_DEPTH = 0
+linux-g++-64: libsuff=64
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

VERSION      = 5.0.0

# Only one of the following flags is allowed !
CONFIG           += qt
#CONFIG           += qtopia

CONFIG           += warn_on 
CONFIG           += release
CONFIG           += thread

# Only one of the following flags is allowed !
# CONFIG           += dll 
CONFIG           += staticlib

# DEFINES          += QT_NO_CAST_ASCII
# DEFINES          += QT_NO_CAST_TO_ASCII

MOC_DIR           = src/moc
OBJECTS_DIR       = src/obj
DESTDIR           = lib$${libsuff}
INCLUDEPATH      += include
DEPENDPATH       += include src

win32:dll:DEFINES    += QT_DLL QWT_DLL QWT_MAKEDLL

# debug:QMAKE_CXXFLAGS += -O0

HEADERS = \
    include/qwt.h \
    include/qwt_abstract_scale_draw.h \
    include/qwt_array.h \
    include/qwt_color_map.h \
    include/qwt_curve_fitter.h \
    include/qwt_data.h \
    include/qwt_double_interval.h \
    include/qwt_double_rect.h \
    include/qwt_dyngrid_layout.h \
    include/qwt_event_pattern.h \
    include/qwt_global.h \
    include/qwt_interval_data.h \
    include/qwt_layout_metrics.h \
    include/qwt_legend.h \
    include/qwt_legend_item.h \
    include/qwt_math.h \
    include/qwt_painter.h \
    include/qwt_paint_buffer.h \
    include/qwt_panner.h \
    include/qwt_picker.h \
    include/qwt_picker_machine.h \
    include/qwt_plot.h \
    include/qwt_plot_curve.h \
    include/qwt_plot_dict.h \
    include/qwt_plot_grid.h \
    include/qwt_plot_item.h \
    include/qwt_plot_layout.h \
    include/qwt_plot_marker.h \
    include/qwt_plot_printfilter.h \
    include/qwt_plot_rasteritem.h \
    include/qwt_plot_spectrogram.h \
    include/qwt_plot_canvas.h \
    include/qwt_plot_panner.h \
    include/qwt_plot_picker.h \
    include/qwt_plot_zoomer.h \
    include/qwt_plot_magnifier.h \
    include/qwt_polygon.h \
    include/qwt_raster_data.h \
    include/qwt_rect.h \
    include/qwt_round_scale_draw.h \
    include/qwt_scale_widget.h \
    include/qwt_scale_div.h \
    include/qwt_scale_draw.h \
    include/qwt_scale_engine.h \
    include/qwt_scale_map.h \
    include/qwt_spline.h \
    include/qwt_symbol.h \
    include/qwt_text_plugin.h \
    include/qwt_text_engine.h \
    include/qwt_text_label.h \
    include/qwt_text.h \
    include/qwt_valuelist.h

SOURCES = \
    src/qwt_abstract_scale_draw.cpp \
    src/qwt_data.cpp \
    src/qwt_color_map.cpp \
    src/qwt_curve_fitter.cpp \
    src/qwt_double_interval.cpp \
    src/qwt_double_rect.cpp \
    src/qwt_dyngrid_layout.cpp \
    src/qwt_event_pattern.cpp \
    src/qwt_interval_data.cpp \
    src/qwt_layout_metrics.cpp \
    src/qwt_legend.cpp \
    src/qwt_legend_item.cpp \
    src/qwt_math.cpp \
    src/qwt_painter.cpp \
    src/qwt_paint_buffer.cpp \
    src/qwt_panner.cpp \
    src/qwt_picker.cpp \
    src/qwt_picker_machine.cpp \
    src/qwt_plot.cpp \
    src/qwt_plot_print.cpp \
    src/qwt_plot_xml.cpp \
    src/qwt_plot_axis.cpp \
    src/qwt_plot_curve.cpp \
    src/qwt_plot_dict.cpp \
    src/qwt_plot_grid.cpp \
    src/qwt_plot_item.cpp \
    src/qwt_plot_spectrogram.cpp \
    src/qwt_plot_marker.cpp \
    src/qwt_plot_layout.cpp \
    src/qwt_plot_printfilter.cpp \
    src/qwt_plot_canvas.cpp \
    src/qwt_plot_panner.cpp \
    src/qwt_plot_rasteritem.cpp \
    src/qwt_plot_picker.cpp \
    src/qwt_plot_zoomer.cpp \
    src/qwt_plot_magnifier.cpp \
    src/qwt_raster_data.cpp \
    src/qwt_rect.cpp \
    src/qwt_round_scale_draw.cpp \
    src/qwt_scale_widget.cpp \
    src/qwt_scale_div.cpp \
    src/qwt_scale_draw.cpp \
    src/qwt_scale_engine.cpp \
    src/qwt_scale_map.cpp \
    src/qwt_spline.cpp \
    src/qwt_symbol.cpp \
    src/qwt_text_engine.cpp \
    src/qwt_text_label.cpp \
    src/qwt_text.cpp 
 
##############################################
# If you don't need to display svg images
# on the plot canvas, remove the following 
# lines
##############################################

# QT += svg
# HEADERS += include/qwt_plot_svgitem.h
# SOURCES += src/qwt_plot_svgitem.cpp 

##############################################
# If you are interested in the plot widget
# only, you can remove the following 
# HEADERS/SOURCES lists.
##############################################

HEADERS += \
    include/qwt_abstract_slider.h \
    include/qwt_abstract_scale.h \
    include/qwt_arrow_button.h \
    include/qwt_analog_clock.h \
    include/qwt_compass.h \
    include/qwt_compass_rose.h \
    include/qwt_counter.h \
    include/qwt_dial.h \
    include/qwt_dial_needle.h \
    include/qwt_double_range.h \
    include/qwt_knob.h \
    include/qwt_slider.h \
    include/qwt_thermo.h \
    include/qwt_wheel.h

SOURCES += \
    src/qwt_abstract_slider.cpp \
    src/qwt_abstract_scale.cpp \
    src/qwt_arrow_button.cpp \
    src/qwt_analog_clock.cpp \
    src/qwt_compass.cpp \
    src/qwt_compass_rose.cpp \
    src/qwt_counter.cpp \
    src/qwt_dial.cpp \
    src/qwt_dial_needle.cpp \
    src/qwt_double_range.cpp \
    src/qwt_knob.cpp \
    src/qwt_slider.cpp \
    src/qwt_thermo.cpp \
    src/qwt_wheel.cpp

unix {
    INSTALLBASE    = /usr
    target.path    = $$INSTALLBASE/lib$${libsuff}
    headers.path   = $$INSTALLBASE/include
    headers.files  = $$HEADERS
    doc.path       = $$INSTALLBASE/doc
    doc.files      = doc/html doc/man
    INSTALLS       = target headers doc
}
