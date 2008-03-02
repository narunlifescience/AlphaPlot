include(../config.pri)
TEMPLATE = lib
CONFIG += plugin static
INCLUDEPATH += .. ../core
TARGET = ../$$qtLibraryTarget(scidavis_analysis)
QT += xml

debug {
	CONFIG -= static
	DEFINES += QT_STATICPLUGIN
}

SOURCES += \
	Convolution.cpp \
	Correlation.cpp \
	Differentiation.cpp \
	ExpDecayDialog.cpp \
	ExponentialFit.cpp \
	FFT.cpp \
	FFTDialog.cpp \
	FFTFilter.cpp \
	IntDialog.cpp \
	Integration.cpp \
	Interpolation.cpp \
	InterpolationDialog.cpp \
	MultiPeakFit.cpp \
	MultiPeakFitTool.cpp \
	UserFunctionFit.cpp \
	PluginFit.cpp \
	PolynomFitDialog.cpp \
	PolynomialFit.cpp \
	SigmoidalFit.cpp \
	SmoothCurveDialog.cpp \
	SmoothFilter.cpp \
	StatisticsFilter.cpp \
	TableStatistics.cpp \
	fit_gsl.cpp \
	nrutil.cpp \

HEADERS += \
	Convolution.h \
	Correlation.h \
	Differentiation.h \
	DifferentiationFilter.h \
	DoubleTransposeFilter.h \
	ExpDecayDialog.h \
	ExponentialFit.h \
	FFT.h \
	FFTDialog.h \
	FFTFilter.h \
	IntDialog.h \
	Integration.h \
	Interpolation.h \
	InterpolationDialog.h \
	MultiPeakFit.h \
	MultiPeakFitTool.h \
	UserFunctionFit.h \
	PluginFit.h \
	PolynomFitDialog.h \
	PolynomialFit.h \
	SigmoidalFit.h \
	SmoothCurveDialog.h \
	SmoothFilter.h \
	StatisticsFilter.h \
	TableStatistics.h \
	TruncationFilter.h \
	fit_gsl.h \
	nrutil.h \
