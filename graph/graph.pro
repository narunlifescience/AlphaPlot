include(../config.pri)
TEMPLATE = lib
CONFIG += plugin static
INCLUDEPATH += ../core ../core/datatypes
TARGET = ../$$qtLibraryTarget(scidavis_graph)
QT += xml svg

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

SOURCES += \
	AssociationsDialog.cpp \
	AxesDialog.cpp \
	CanvasPicker.cpp \
	ColorMapEditor.cpp \
	CurvesDialog.cpp \
	ErrDialog.cpp \
	FunctionCurve.cpp \
	FunctionDialog.cpp \
	Layer.cpp \
	LayerDialog.cpp \
	TextEnrichment.cpp \
	Graph.cpp \
	Plot.cpp \
	PlotCurve.cpp \
	PlotDialog.cpp \
	AbstractEnrichment.cpp \
	PlotWizard.cpp \
	ScaleDraw.cpp \
	ScalePicker.cpp \
	SelectionMoveResizer.cpp \
	SymbolBox.cpp \
	TitlePicker.cpp \

HEADERS += \
	AssociationsDialog.h \
	AxesDialog.h \
	CanvasPicker.h \
	ColorMapEditor.h \
	CurvesDialog.h \
	ErrDialog.h \
	FunctionCurve.h \
	FunctionDialog.h \
	Layer.h \
	LayerDialog.h \
	TextEnrichment.h \
	Graph.h \
	Plot.h \
	PlotCurve.h \
	PlotDialog.h \
	AbstractEnrichment.h \
	AbstractGraphTool.h \
	PlotWizard.h \
	QwtDataFilter.h \
	ScaleDraw.h \
	ScalePicker.h \
	SelectionMoveResizer.h \
	SymbolBox.h \
	TitlePicker.h \

SOURCES += \
	types/BoxCurve.cpp \
	types/BarCurve.cpp \
	types/ErrorCurve.cpp \
	types/HistogramCurve.cpp \
	types/PieCurve.cpp \
	types/Spectrogram.cpp \
	types/VectorCurve.cpp \

HEADERS += \
	types/BoxCurve.h \
	types/BarCurve.h \
	types/ErrorCurve.h \
	types/HistogramCurve.h \
	types/PieCurve.h \
	types/Spectrogram.h \
	types/VectorCurve.h \

SOURCES += \
	enrichments/LineEnrichment.cpp \
	enrichments/ImageEnrichmentDialog.cpp \
	enrichments/ImageEnrichment.cpp \
	enrichments/LineEnrichmentDialog.cpp \

HEADERS += \
	enrichments/LineEnrichment.h \
	enrichments/ImageEnrichmentDialog.h \
	enrichments/ImageEnrichment.h \
	enrichments/LineEnrichmentDialog.h \

SOURCES += \
	tools/DataPickerTool.cpp \
	tools/LineProfileTool.cpp \
	tools/RangeSelectorTool.cpp \
	tools/ScreenPickerTool.cpp \
	tools/TranslateCurveTool.cpp \

HEADERS += \
	tools/DataPickerTool.h \
	tools/LineProfileTool.h \
	tools/RangeSelectorTool.h \
	tools/ScreenPickerTool.h \
	tools/TranslateCurveTool.h \

