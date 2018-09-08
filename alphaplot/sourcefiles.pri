#############################################################################
###################### PROJECT FILES SECTION ################################
#############################################################################

###################### HEADERS ##############################################

HEADERS  += src/ApplicationWindow.h \
            src/core/IconLoader.h \
            src/core/Utilities.h \
            src/globals.h\
            src/scripting/ScriptingFunctions.h\
            src/Graph.h \
            src/3Dplot/Graph3D.h \
            src/Table.h \
            src/CurvesDialog.h \
            src/PlotDialog.h \
            src/3Dplot/Plot3DDialog.h \
            src/PlotWizard.h \
            src/ExportDialog.h \
            src/AxesDialog.h \
            src/PolynomFitDialog.h \
            src/ExpDecayDialog.h \
            src/FunctionDialog.h \
            src/FitDialog.h \
            src/3Dplot/SurfaceDialog.h \
            src/TextDialog.h \
            src/LineDialog.h \
            src/ScalePicker.h \
            src/TitlePicker.h \
            src/CanvasPicker.h \
            src/PlotCurve.h \
            src/QwtErrorPlotCurve.h \
            src/QwtPieCurve.h \
            src/ErrDialog.h \
            src/Legend.h \
            src/ArrowMarker.h \
            src/ImageMarker.h \
            src/ImageDialog.h \
            src/fit_gsl.h \
            src/MultiLayer.h\
            src/LayerDialog.h \
            src/IntDialog.h \
            src/3Dplot/Bar.h \
            src/3Dplot/Cone3D.h \
            src/ConfigDialog.h \
            src/QwtBarCurve.h \
            src/BoxCurve.h \
            src/QwtHistogram.h \
            src/VectorCurve.h \
            src/ScaleDraw.h \
            src/Matrix.h \
            src/DataSetDialog.h \
            src/scripting/MyParser.h \
            src/ColorBox.h \
            src/SymbolBox.h \
            src/PatternBox.h \
            src/SymbolDialog.h \
            src/Plot.h \
            src/ColorButton.h \
            src/AssociationsDialog.h \
            src/RenameWindowDialog.h \
            src/MyWidget.h \
            src/InterpolationDialog.h\
            src/ImportASCIIDialog.h \
            src/ImageExportDialog.h\
            src/SmoothCurveDialog.h\
            src/FilterDialog.h\
            src/FFTDialog.h\
            src/Note.h\
            src/Folder.h\
            src/FindDialog.h\
            src/scripting/ScriptingEnv.h\
            src/scripting/Script.h\
            src/scripting/ScriptEdit.h\
            src/FunctionCurve.h\
            src/Fit.h\
            src/MultiPeakFit.h\
            src/ExponentialFit.h\
            src/PolynomialFit.h\
            src/NonLinearFit.h\
            src/PluginFit.h\
            src/SigmoidalFit.h\
            src/scripting/customevents.h\
            src/scripting/ScriptingLangDialog.h\
            src/TextFormatButtons.h\
            src/TableStatistics.h\
            src/Spectrogram.h\
            src/ColorMapEditor.h\
            src/SelectionMoveResizer.h\
            src/Filter.h\
            src/Differentiation.h\
            src/Integration.h\
            src/Interpolation.h\
            src/SmoothFilter.h\
            src/FFTFilter.h\
            src/FFT.h\
            src/Convolution.h\
            src/Correlation.h\
            src/PlotToolInterface.h\
            src/ScreenPickerTool.h\
            src/DataPickerTool.h\
            src/RangeSelectorTool.h\
            src/TranslateCurveTool.h\
            src/MultiPeakFitTool.h\
            src/CurveRangeDialog.h\
            src/LineProfileTool.h\
            src/PlotEnrichement.h\
            src/ExtensibleFileDialog.h\
            src/OpenProjectDialog.h\
            src/Grid.h\
            src/DataPointPicker.h \
            src/TeXTableSettings.h \
            src/TeXTableSettingsWidget.h \
            src/TeXTableExportDialog.h \
            src/scripting/widgets/Console.h \
            src/scripting/widgets/ConsoleWidget.h \
            src/scripting/SyntaxHighlighter.h \
            src/widgets/aSettingsListView.h \
            src/ui/GeneralApplicationSettings.h \
            src/ui/GeneralAppreanceSettings.h \
            src/ui/SettingsPage.h \
            src/ui/SettingsDialog.h \
            src/ui/PropertiesDialog.h \
            src/ui/RandomDistributionDialog.h \
            src/About.h \
            src/2Dplot/Spline2D.h \
            src/2Dplot/Vector2D.h \
            src/2Dplot/DataManager2D.h \
            src/2Dplot/Curve2D.h \
            src/2Dplot/PlotPoint.h \
            src/2Dplot/Pie2D.h \
            src/2Dplot/widgets/MyTreeWidget.h \
            src/2Dplot/widgets/AddAxisWidget.h \
    $$PWD/src/2Dplot/LineScatterCommon.h

###################### SOURCES ##############################################

SOURCES  += src/ApplicationWindow.cpp \
            src/core/IconLoader.cpp \
            src/core/Utilities.cpp \
            src/Graph.cpp \
            src/3Dplot/Graph3D.cpp \
            src/Table.cpp \
            src/CurvesDialog.cpp \
            src/PlotDialog.cpp \
            src/3Dplot/Plot3DDialog.cpp \
            src/PlotWizard.cpp \
            src/ExportDialog.cpp \
            src/AxesDialog.cpp \
            src/PolynomFitDialog.cpp \
            src/TextDialog.cpp \
            src/ScalePicker.cpp\
            src/TitlePicker.cpp \
            src/CanvasPicker.cpp \
            src/ExpDecayDialog.cpp \
            src/FunctionDialog.cpp \
            src/FitDialog.cpp \
            src/3Dplot/SurfaceDialog.cpp \
            src/LineDialog.cpp \
            src/PlotCurve.cpp \
            src/QwtErrorPlotCurve.cpp \
            src/QwtPieCurve.cpp \
            src/ErrDialog.cpp \
            src/Legend.cpp \
            src/ArrowMarker.cpp \
            src/ImageMarker.cpp \
            src/ImageDialog.cpp \
            src/MultiLayer.cpp\
            src/LayerDialog.cpp \
            src/IntDialog.cpp \
            src/3Dplot/Bar.cpp \
            src/3Dplot/Cone3D.cpp \
            src/DataSetDialog.cpp \
            src/ConfigDialog.cpp \
            src/QwtBarCurve.cpp \
            src/BoxCurve.cpp \
            src/QwtHistogram.cpp \
            src/VectorCurve.cpp \
            src/Matrix.cpp \
            src/scripting/MyParser.cpp\
            src/ColorBox.cpp \
            src/SymbolBox.cpp \
            src/PatternBox.cpp \
            src/SymbolDialog.cpp \
            src/Plot.cpp \
            src/ColorButton.cpp \
            src/AssociationsDialog.cpp \
            src/RenameWindowDialog.cpp \
            src/MyWidget.cpp\
            src/InterpolationDialog.cpp\
            src/fit_gsl.cpp\
            src/SmoothCurveDialog.cpp\
            src/FilterDialog.cpp\
            src/FFTDialog.cpp\
            src/Note.cpp\
            src/Folder.cpp\
            src/FindDialog.cpp\
            src/TextFormatButtons.cpp\
            src/scripting/ScriptEdit.cpp\
            src/ImportASCIIDialog.cpp\
            src/ImageExportDialog.cpp\
            src/ScaleDraw.cpp\
            src/FunctionCurve.cpp\
            src/Fit.cpp\
            src/MultiPeakFit.cpp\
            src/ExponentialFit.cpp\
            src/PolynomialFit.cpp\
            src/PluginFit.cpp\
            src/NonLinearFit.cpp\
            src/SigmoidalFit.cpp\
            src/scripting/ScriptingFunctions.cpp\
            src/scripting/ScriptingEnv.cpp\
            src/scripting/Script.cpp\
            src/scripting/ScriptingLangDialog.cpp\
            src/TableStatistics.cpp\
            src/Spectrogram.cpp\
            src/ColorMapEditor.cpp\
            src/SelectionMoveResizer.cpp\
            src/Filter.cpp\
            src/Differentiation.cpp\
            src/Integration.cpp\
            src/Interpolation.cpp\
            src/SmoothFilter.cpp\
            src/FFTFilter.cpp\
            src/FFT.cpp\
            src/Convolution.cpp\
            src/Correlation.cpp\
            src/ScreenPickerTool.cpp\
            src/DataPickerTool.cpp\
            src/RangeSelectorTool.cpp\
            src/TranslateCurveTool.cpp\
            src/MultiPeakFitTool.cpp\
            src/CurveRangeDialog.cpp\
            src/LineProfileTool.cpp\
            src/PlotEnrichement.cpp\
            src/ExtensibleFileDialog.cpp\
            src/OpenProjectDialog.cpp\
            src/Grid.cpp\
            src/globals.cpp \
            src/DataPointPicker.cpp \
            src/TeXTableSettings.cpp \
            src/TeXTableSettingsWidget.cpp \
            src/TeXTableExportDialog.cpp \
            src/scripting/widgets/Console.cpp \
            src/scripting/widgets/ConsoleWidget.cpp \
            src/scripting/SyntaxHighlighter.cpp \
            src/widgets/aSettingsListView.cpp \
            src/ui/GeneralApplicationSettings.cpp \
            src/ui/GeneralAppreanceSettings.cpp \
            src/ui/SettingsPage.cpp \
            src/ui/SettingsDialog.cpp \
            src/ui/PropertiesDialog.cpp \
            src/ui/RandomDistributionDialog.cpp \
            src/About.cpp \
            src/main.cpp \
            src/2Dplot/Spline2D.cpp \
            src/2Dplot/Vector2D.cpp \
            src/2Dplot/DataManager2D.cpp \
            src/2Dplot/Curve2D.cpp \
            src/2Dplot/PlotPoint.cpp \
            src/2Dplot/Pie2D.cpp \
            src/2Dplot/widgets/MyTreeWidget.cpp \
            src/2Dplot/widgets/AddAxisWidget.cpp

###################### FORMS ##############################################
FORMS        += src/ApplicationWindow.ui \
                src/scripting/widgets/ConsoleWidget.ui \
                src/ui/GeneralApplicationSettings.ui \
                src/ui/GeneralAppreanceSettings.ui \
                src/ui/SettingsDialog.ui \
                src/ui/PropertiesDialog.ui \
                src/ui/RandomDistributionDialog.ui \
                src/2Dplot/widgets/AddAxisWidget.ui \
                src/About.ui \

########### Future code backported from the aspect framework ##################
INCLUDEPATH  += src
INCLUDEPATH  += src/future

FORMS        += src/future/matrix/matrixcontroltabs.ui \
                src/future/core/ProjectConfigPage.ui \
                src/future/table/controltabs.ui \
                src/future/table/DimensionsDialog.ui

HEADERS     += src/future/core/AbstractAspect.h \
               src/future/core/AbstractPart.h \
               src/future/core/AspectPrivate.h \
               src/future/core/aspectcommands.h \
               src/future/core/future_Folder.h \
               src/future/core/Project.h \
               src/future/core/ProjectConfigPage.h \
               src/future/core/PartMdiView.h \
               src/future/core/AbstractColumn.h \
               src/future/core/column/Column.h \
               src/future/core/column/ColumnPrivate.h \
               src/future/core/column/columncommands.h \
               src/future/core/AbstractFilter.h \
               src/future/core/AbstractSimpleFilter.h \
               src/future/core/datatypes/SimpleCopyThroughFilter.h \
               src/future/core/datatypes/DateTime2DoubleFilter.h \
               src/future/core/datatypes/DateTime2StringFilter.h \
               src/future/core/datatypes/DayOfWeek2DoubleFilter.h \
               src/future/core/datatypes/Double2DateTimeFilter.h \
               src/future/core/datatypes/Double2DayOfWeekFilter.h \
               src/future/core/datatypes/Double2MonthFilter.h \
               src/future/core/datatypes/Double2StringFilter.h \
               src/future/core/datatypes/Month2DoubleFilter.h \
               src/future/core/datatypes/String2DateTimeFilter.h \
               src/future/core/datatypes/String2DayOfWeekFilter.h \
               src/future/core/datatypes/String2DoubleFilter.h \
               src/future/core/datatypes/String2MonthFilter.h \
               src/future/lib/macros.h \
               src/future/lib/XmlStreamReader.h \
               src/future/lib/ActionManager.h \
               src/future/lib/ConfigPageWidget.h \
               src/future/lib/Interval.h \
               src/future/lib/IntervalAttribute.h \
               src/future/matrix/future_Matrix.h \
               src/future/matrix/MatrixModel.h \
               src/future/matrix/MatrixView.h \
               src/future/matrix/matrixcommands.h \
               src/future/table/future_Table.h \
               src/future/table/TableModel.h \
               src/future/table/TableView.h \
               src/future/table/tablecommands.h \
               src/future/table/TableDoubleHeaderView.h \
               src/future/table/TableItemDelegate.h \
               src/future/table/TableCommentsHeaderModel.h \
               src/future/table/future_SortDialog.h \
               src/future/table/AsciiTableImportFilter.h \
               src/future/core/AbstractImportFilter.h \
               src/future/core/interfaces.h \

SOURCES     += src/future/core/AbstractAspect.cpp \
               src/future/core/AbstractPart.cpp \
               src/future/core/AspectPrivate.cpp \
               src/future/core/future_Folder.cpp \
               src/future/core/PartMdiView.cpp \
               src/future/core/Project.cpp \
               src/future/core/column/Column.cpp \
               src/future/core/column/ColumnPrivate.cpp \
               src/future/core/column/columncommands.cpp \
               src/future/core/datatypes/DateTime2StringFilter.cpp \
               src/future/core/datatypes/String2DateTimeFilter.cpp \
               src/future/core/datatypes/Double2StringFilter.cpp \
               src/future/core/datatypes/Double2DateTimeFilter.cpp \
               src/future/core/AbstractSimpleFilter.cpp \
               src/future/core/AbstractFilter.cpp \
               src/future/core/ProjectConfigPage.cpp \
               src/future/lib/XmlStreamReader.cpp \
               src/future/lib/ActionManager.cpp \
               src/future/lib/ConfigPageWidget.cpp \
               src/future/matrix/future_Matrix.cpp \
               src/future/matrix/MatrixModel.cpp \
               src/future/matrix/MatrixView.cpp \
               src/future/matrix/matrixcommands.cpp \
               src/future/table/future_Table.cpp \
               src/future/table/TableModel.cpp \
               src/future/table/TableView.cpp \
               src/future/table/tablecommands.cpp \
               src/future/table/TableDoubleHeaderView.cpp \
               src/future/table/TableItemDelegate.cpp \
               src/future/table/TableCommentsHeaderModel.cpp \
               src/future/table/future_SortDialog.cpp \
               src/future/table/AsciiTableImportFilter.cpp \

###############################################################
##################### Compression #############################
############### (mini gzip implementation) ####################
################# requires zlib >= 1.2.3 ######################
###############################################################

SOURCES     += ../3rdparty/minigzip/minigzip.c

##############################################################
####################### QCustomPlot ##########################
##############################################################

HEADERS     += ../3rdparty/qcustomplot/qcustomplot.h

SOURCES     += ../3rdparty/qcustomplot/qcustomplot.cpp

##############################################################
####################### PropertyBrowser ######################
##############################################################

    SOURCES += ../3rdparty/propertybrowser/qtpropertybrowser.cpp \
               ../3rdparty/propertybrowser/qtpropertymanager.cpp \
               ../3rdparty/propertybrowser/qteditorfactory.cpp \
               ../3rdparty/propertybrowser/qttreepropertybrowser.cpp \
               ../3rdparty/propertybrowser/qtpropertybrowserutils.cpp

    HEADERS += ../3rdparty/propertybrowser/qtpropertybrowser.h \
               ../3rdparty/propertybrowser/qtpropertymanager.h \
               ../3rdparty/propertybrowser/qteditorfactory.h \
               ../3rdparty/propertybrowser/qttreepropertybrowser.h \
               ../3rdparty/propertybrowser/qtpropertybrowserutils_p.h

    RESOURCES += ../3rdparty/propertybrowser/qtpropertybrowser.qrc

##############################################################
####################### 2DPlot ###############################
##############################################################

HEADERS     += src/2Dplot/widgets/LayoutButton2D.h \
               src/2Dplot/widgets/Axis2DPropertiesDialog.h \
               src/2Dplot/widgets/propertyeditor.h \
               src/2Dplot/widgets/Function2DDialog.h \
               src/2Dplot/Axis2D.h \
               src/2Dplot/AxisRect2D.h \
               src/2Dplot/Bar2D.h \
               src/2Dplot/Grid2D.h \
               src/2Dplot/Histogram2D.h \
               src/2Dplot/Layout2D.h \
               src/2Dplot/LayoutGrid2D.h \
               src/2Dplot/Legend2D.h \
               src/2Dplot/LineScatter2D.h \
               src/2Dplot/Plot2D.h \
               src/2Dplot/StatBox2D.h \
               src/2Dplot/Properties2D.h \

SOURCES     += src/2Dplot/widgets/LayoutButton2D.cpp \
               src/2Dplot/widgets/Axis2DPropertiesDialog.cpp \
               src/2Dplot/widgets/propertyeditor.cpp \
               src/2Dplot/widgets/Function2DDialog.cpp \
               src/2Dplot/Axis2D.cpp \
               src/2Dplot/AxisRect2D.cpp \
               src/2Dplot/Bar2D.cpp \
               src/2Dplot/Grid2D.cpp \
               src/2Dplot/Histogram2D.cpp \
               src/2Dplot/Layout2D.cpp \
               src/2Dplot/LayoutGrid2D.cpp \
               src/2Dplot/Legend2D.cpp \
               src/2Dplot/LineScatter2D.cpp \
               src/2Dplot/Plot2D.cpp \
               src/2Dplot/StatBox2D.cpp \
               src/2Dplot/Properties2D.cpp \

FORMS       += src/2Dplot/widgets/Axis2DPropertiesDialog.ui \
               src/2Dplot/widgets/propertyeditor.ui \
               src/2Dplot/widgets/Function2DDialog.ui \
