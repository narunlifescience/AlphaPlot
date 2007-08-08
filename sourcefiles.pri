###################### ICONS ################################################
RESOURCES += \
	appicons.qrc \
	icons.qrc \

###################### TRANSLATIONS #########################################

TRANSLATIONS = \
	translations/scidavis_de.ts \
	translations/scidavis_es.ts \
	translations/scidavis_fr.ts \
	translations/scidavis_ru.ts \
	translations/scidavis_ja.ts \
	translations/scidavis_sv.ts \

#system(lupdate -verbose scidavis.pro)
#system(lrelease -verbose scidavis.pro)

###################### DOCUMENTATION ########################################

documentation.files += \
	README \
	CHANGES \
	gpl.txt \
	INSTALL.html \
	scidavis.css \
	scidavis-logo.png \
	manual/html \

##################### Compression #############################
############### (mini gzip implementation) ####################
################# requires zlib >= 1.2.3 ######################
###############################################################

SOURCES += 3rdparty/minigzip/minigzip.c

################# Origin Import (liborigin) ###################
###############################################################

!dynamic_liborigin{
INCLUDEPATH  += 3rdparty/liborigin

HEADERS += 3rdparty/liborigin/OPJFile.h
SOURCES += 3rdparty/liborigin/OPJFile.cpp
}

SOURCES += \
	lib/ColorBox.cpp \
	lib/ColorButton.cpp \
	lib/ExtensibleFileDialog.cpp \
	lib/PatternBox.cpp \
	lib/SymbolDialog.cpp \
	lib/TextDialog.cpp \
	lib/TextFormatButtons.cpp \
	lib/ImageExportDialog.cpp \

HEADERS += \
	lib/ColorBox.h \
	lib/ColorButton.h \
	lib/ExtensibleFileDialog.h \
	lib/Interval.h \
	lib/IntervalAttribute.h \
	lib/PatternBox.h \
	lib/SymbolDialog.h \
	lib/TextDialog.h \
	lib/TextFormatButtons.h \
	lib/ImageExportDialog.h \

SOURCES += \
	core/AbstractFilter.cpp \
	core/ApplicationWindow.cpp \
	core/PreferencesDialog.cpp \
	core/CurveRangeDialog.cpp \
	core/DataSetDialog.cpp \
	core/Filter.cpp \
	core/FilterDialog.cpp \
	core/FindWindowDialog.cpp \
	core/Fit.cpp \
	core/FitDialog.cpp \
	core/Folder.cpp \
	core/MyParser.cpp \
	core/MyWidget.cpp \
	core/OpenProjectDialog.cpp \
	core/ReadOnlyTableModel.cpp \
	core/RenameWindowDialog.cpp \
	core/AbstractScript.cpp \
	core/ScriptEdit.cpp \
	core/AbstractScriptingEngine.cpp \
	core/ScriptingLangDialog.cpp \
	core/main.cpp \

HEADERS += \
	core/AbstractDataSource.h \
	core/AbstractFilter.h \
	core/AbstractSimpleFilter.h \
	core/ApplicationWindow.h \
	core/PreferencesDialog.h \
	core/CopyThroughFilter.h \
	core/CurveRangeDialog.h \
	core/DataSetDialog.h \
	core/Filter.h \
	core/FilterDialog.h \
	core/FindWindowDialog.h \
	core/Fit.h \
	core/FitDialog.h \
	core/Folder.h \
	core/MyParser.h \
	core/MyWidget.h \
	core/OpenProjectDialog.h \
	core/ReadOnlyTableModel.h \
	core/RenameWindowDialog.h \
	core/AbstractScript.h \
	core/ScriptEdit.h \
	core/AbstractScriptingEngine.h \
	core/ScriptingLangDialog.h \
	core/customevents.h \
	core/globals.h \
	core/resource.h \

SOURCES += \
	core/datatypes/String2DateTimeFilter.cpp \

HEADERS += \
	core/datatypes/AbstractDateTimeDataSource.h \
	core/datatypes/AbstractDoubleDataSource.h \
	core/datatypes/AbstractStringDataSource.h \
	core/datatypes/DateTime2DoubleFilter.h \
	core/datatypes/DateTime2StringFilter.h \
	core/datatypes/DayOfWeek2DoubleFilter.h \
	core/datatypes/Double2DateTimeFilter.h \
	core/datatypes/Double2DayOfWeekFilter.h \
	core/datatypes/Double2MonthFilter.h \
	core/datatypes/Double2StringFilter.h \
	core/datatypes/Month2DoubleFilter.h \
	core/datatypes/String2DateTimeFilter.h \
	core/datatypes/String2DayOfWeekFilter.h \
	core/datatypes/String2DoubleFilter.h \
	core/datatypes/String2MonthFilter.h \

SOURCES += \
	table/DateTimeColumnData.cpp \
	table/DoubleColumnData.cpp \
	table/ExportTableDialog.cpp \
	table/ImportTableDialog.cpp \
	table/SetColValuesDialog.cpp \
	table/SortDialog.cpp \
	table/StringColumnData.cpp \
	table/Table.cpp \
	table/TableDialog.cpp \
	table/TableItemDelegate.cpp \
	table/TableModel.cpp \
	table/TableView.cpp \
	table/tablecommands.cpp \

HEADERS += \
	table/AbstractColumnData.h \
	table/DateTimeColumnData.h \
	table/DoubleColumnData.h \
	table/ExportTableDialog.h \
	table/ImportTableDialog.h \
	table/SetColValuesDialog.h \
	table/SortDialog.h \
	table/StringColumnData.h \
	table/Table.h \
	table/TableDialog.h \
	table/TableItemDelegate.h \
	table/TableModel.h \
	table/TableView.h \
	table/tablecommands.h \

SOURCES += \
	note/Note.cpp \

HEADERS += \
	note/Note.h \

SOURCES += \
	analysis/Convolution.cpp \
	analysis/Correlation.cpp \
	analysis/Differentiation.cpp \
	analysis/ExpDecayDialog.cpp \
	analysis/ExponentialFit.cpp \
	analysis/FFT.cpp \
	analysis/FFTDialog.cpp \
	analysis/FFTFilter.cpp \
	analysis/IntDialog.cpp \
	analysis/Integration.cpp \
	analysis/Interpolation.cpp \
	analysis/InterpolationDialog.cpp \
	analysis/MultiPeakFit.cpp \
	analysis/MultiPeakFitTool.cpp \
	analysis/UserFunctionFit.cpp \
	analysis/PluginFit.cpp \
	analysis/PolynomFitDialog.cpp \
	analysis/PolynomialFit.cpp \
	analysis/SigmoidalFit.cpp \
	analysis/SmoothCurveDialog.cpp \
	analysis/SmoothFilter.cpp \
	analysis/StatisticsFilter.cpp \
	analysis/TableStatistics.cpp \
	analysis/fit_gsl.cpp \
	analysis/nrutil.cpp \

HEADERS += \
	analysis/Convolution.h \
	analysis/Correlation.h \
	analysis/Differentiation.h \
	analysis/DifferentiationFilter.h \
	analysis/DoubleTransposeFilter.h \
	analysis/ExpDecayDialog.h \
	analysis/ExponentialFit.h \
	analysis/FFT.h \
	analysis/FFTDialog.h \
	analysis/FFTFilter.h \
	analysis/IntDialog.h \
	analysis/Integration.h \
	analysis/Interpolation.h \
	analysis/InterpolationDialog.h \
	analysis/MultiPeakFit.h \
	analysis/MultiPeakFitTool.h \
	analysis/UserFunctionFit.h \
	analysis/PluginFit.h \
	analysis/PolynomFitDialog.h \
	analysis/PolynomialFit.h \
	analysis/SigmoidalFit.h \
	analysis/SmoothCurveDialog.h \
	analysis/SmoothFilter.h \
	analysis/StatisticsFilter.h \
	analysis/TableStatistics.h \
	analysis/TruncationFilter.h \
	analysis/fit_gsl.h \
	analysis/nrutil.h \

SOURCES += \
	graph/AssociationsDialog.cpp \
	graph/AxesDialog.cpp \
	graph/CanvasPicker.cpp \
	graph/ColorMapEditor.cpp \
	graph/CurvesDialog.cpp \
	graph/ErrDialog.cpp \
	graph/FunctionCurve.cpp \
	graph/FunctionDialog.cpp \
	graph/Graph.cpp \
	graph/LayerDialog.cpp \
	graph/TextEnrichment.cpp \
	graph/MultiLayer.cpp \
	graph/Plot.cpp \
	graph/PlotCurve.cpp \
	graph/PlotDialog.cpp \
	graph/AbstractEnrichment.cpp \
	graph/PlotWizard.cpp \
	graph/ScaleDraw.cpp \
	graph/ScalePicker.cpp \
	graph/SelectionMoveResizer.cpp \
	graph/SymbolBox.cpp \
	graph/TitlePicker.cpp \

HEADERS += \
	graph/AssociationsDialog.h \
	graph/AxesDialog.h \
	graph/CanvasPicker.h \
	graph/ColorMapEditor.h \
	graph/CurvesDialog.h \
	graph/ErrDialog.h \
	graph/FunctionCurve.h \
	graph/FunctionDialog.h \
	graph/Graph.h \
	graph/LayerDialog.h \
	graph/TextEnrichment.h \
	graph/MultiLayer.h \
	graph/Plot.h \
	graph/PlotCurve.h \
	graph/PlotDialog.h \
	graph/AbstractEnrichment.h \
	graph/AbstractGraphTool.h \
	graph/PlotWizard.h \
	graph/QwtDataFilter.h \
	graph/ScaleDraw.h \
	graph/ScalePicker.h \
	graph/SelectionMoveResizer.h \
	graph/SymbolBox.h \
	graph/TitlePicker.h \

SOURCES += \
	graph/types/BoxCurve.cpp \
	graph/types/BarCurve.cpp \
	graph/types/ErrorCurve.cpp \
	graph/types/HistogramCurve.cpp \
	graph/types/PieCurve.cpp \
	graph/types/Spectrogram.cpp \
	graph/types/VectorCurve.cpp \

HEADERS += \
	graph/types/BoxCurve.h \
	graph/types/BarCurve.h \
	graph/types/ErrorCurve.h \
	graph/types/HistogramCurve.h \
	graph/types/PieCurve.h \
	graph/types/Spectrogram.h \
	graph/types/VectorCurve.h \

SOURCES += \
	graph/enrichments/LineEnrichment.cpp \
	graph/enrichments/ImageEnrichmentDialog.cpp \
	graph/enrichments/ImageEnrichment.cpp \
	graph/enrichments/LineEnrichmentDialog.cpp \

HEADERS += \
	graph/enrichments/LineEnrichment.h \
	graph/enrichments/ImageEnrichmentDialog.h \
	graph/enrichments/ImageEnrichment.h \
	graph/enrichments/LineEnrichmentDialog.h \

SOURCES += \
	graph/tools/DataPickerTool.cpp \
	graph/tools/LineProfileTool.cpp \
	graph/tools/RangeSelectorTool.cpp \
	graph/tools/ScreenPickerTool.cpp \
	graph/tools/TranslateCurveTool.cpp \

HEADERS += \
	graph/tools/DataPickerTool.h \
	graph/tools/LineProfileTool.h \
	graph/tools/RangeSelectorTool.h \
	graph/tools/ScreenPickerTool.h \
	graph/tools/TranslateCurveTool.h \

SOURCES += \
	matrix/Matrix.cpp \
	matrix/MatrixDialog.cpp \
	matrix/MatrixSizeDialog.cpp \
	matrix/MatrixValuesDialog.cpp \

HEADERS += \
	matrix/Matrix.h \
	matrix/MatrixDialog.h \
	matrix/MatrixSizeDialog.h \
	matrix/MatrixValuesDialog.h \

SOURCES += \
	graph-3D/Bar3D.cpp \
	graph-3D/Cone3D.cpp \
	graph-3D/Graph3D.cpp \
	graph-3D/PlotDialog3D.cpp \
	graph-3D/FunctionDialog3D.cpp \

HEADERS += \
	graph-3D/Bar3D.h \
	graph-3D/Cone3D.h \
	graph-3D/Graph3D.h \
	graph-3D/PlotDialog3D.h \
	graph-3D/FunctionDialog3D.h \

SOURCES += \
	origin/OpjImporter.cpp \

HEADERS += \
	origin/OpjImporter.h \

