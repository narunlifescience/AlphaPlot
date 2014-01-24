#############################################################################
###################### PROJECT FILES SECTION ################################
#############################################################################

###################### ICONS ################################################
RESOURCES    +=        appicons.qrc
RESOURCES    +=        icons.qrc
RC_FILE      =         scidavis.rc

win32 {
  win_icon.files = icons/scidavis.ico
  win_icon.path = "$$INSTALLBASE"
}

###################### TRANSLATIONS #########################################

TRANSLATIONS    = translations/scidavis_de.ts \
                  translations/scidavis_es.ts \
                  translations/scidavis_fr.ts \
                  translations/scidavis_ru.ts \
                  translations/scidavis_ja.ts \
                  translations/scidavis_sv.ts \
                  translations/scidavis_pt-br.ts \
                  translations/scidavis_cs.ts \
                  translations/scidavis_cs-alt.ts \

contains( INSTALLS, translationfiles ){
    exists(translations/scidavis_de.ts){
        translationfiles.files =     translations/scidavis_de.qm \
                             translations/scidavis_es.qm \
                             translations/scidavis_fr.qm \
                             translations/scidavis_ru.qm \
                             translations/scidavis_ja.qm \
                             translations/scidavis_sv.qm \
                             translations/scidavis_pt-br.qm \
                             translations/scidavis_cs.qm \
                             translations/scidavis_cs-alt.qm \

        unix: tstarget.target = translations/scidavis_de.qm
        win32: tstarget.target = translations\scidavis_de.qm
# note the translation files are not writable during AEGIS
# integration, so we don't want to perform an update then
        tstarget.commands = (! test -w translations/scidavis_de.ts || $$LUPDATE_BIN scidavis.pro) && $$LRELEASE_BIN scidavis.pro

        QMAKE_EXTRA_TARGETS += tstarget
        PRE_TARGETDEPS += translations/scidavis_de.qm
    }
    !exists(translations/scidavis_de.ts){
        message("=======================================")   
        message("Could not find translation (.ts) files.")   
        message("=======================================")   
    }
}

###################### DOCUMENTATION ########################################

documentation.files += ../manual/html \
                       ../INSTALL.html \
                       ../scidavis.css \
                       ../scidavis-logo.png \
                       ../README \
                       ../CHANGES \
                       ../gpl.txt 

###################### DESKTOP INTEGRATION ##################################

unix {
	desktop_entry.files = scidavis.desktop
	desktop_entry.path = "$$INSTALLBASE/share/applications"

	mime_package.files = scidavis.xml
	mime_package.path = "$$INSTALLBASE/share/mime/packages"

	#deprecated
	mime_link.files = x-sciprj.desktop
	mime_link.path = "$$INSTALLBASE/share/mimelnk/application"
	
	contains(INSTALLS, icons) {
		# scalable icon
		icons.files = icons/scidavis.svg
		icons.path = "$$INSTALLBASE/share/icons/hicolor/scalable/apps"

		# hicolor icons for different resolutions
		resolutions = 16 22 32 48 64 128
		for(res, resolutions) {
			eval(icon_hicolor_$${res}.files = icons/hicolor-$${res}/scidavis.png)
			eval(icon_hicolor_$${res}.path = "$$INSTALLBASE/share/icons/hicolor/$${res}x$${res}/apps")
			INSTALLS += icon_hicolor_$${res}
		}

		# locolor icons for different resolutions
		resolutions = 16 22 32
		for(res, resolutions) {
			eval(icon_locolor_$${res}.files = icons/locolor-$${res}/scidavis.png)
			eval(icon_locolor_$${res}.path = "$$INSTALLBASE/share/icons/locolor/$${res}x$${res}/apps")

			INSTALLS += icon_locolor_$${res}
		}
	}
}

###################### HEADERS ##############################################

HEADERS  += src/ApplicationWindow.h \
            src/globals.h\
            src/Graph.h \
            src/Graph3D.h \
            src/Table.h \
            src/CurvesDialog.h \
            src/PlotDialog.h \
            src/Plot3DDialog.h \
            src/PlotWizard.h \
            src/ExportDialog.h \
            src/AxesDialog.h \
            src/PolynomFitDialog.h \
            src/ExpDecayDialog.h \
            src/FunctionDialog.h \
            src/FitDialog.h \
            src/SurfaceDialog.h \
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
            src/Bar.h \
            src/Cone3D.h \
            src/ConfigDialog.h \
            src/QwtBarCurve.h \
            src/BoxCurve.h \
            src/QwtHistogram.h \
            src/VectorCurve.h \
            src/ScaleDraw.h \
            src/Matrix.h \
            src/DataSetDialog.h \
            src/MyParser.h \
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
            src/ScriptingEnv.h\
            src/Script.h\
            src/ScriptEdit.h\
            src/FunctionCurve.h\
            src/Fit.h\
            src/MultiPeakFit.h\
            src/ExponentialFit.h\
            src/PolynomialFit.h\
            src/NonLinearFit.h\
            src/PluginFit.h\
            src/SigmoidalFit.h\
            src/customevents.h\
            src/ScriptingLangDialog.h\
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


###################### SOURCES ##############################################

SOURCES  += src/ApplicationWindow.cpp \
            src/Graph.cpp \
            src/Graph3D.cpp \
            src/Table.cpp \
            src/CurvesDialog.cpp \
            src/PlotDialog.cpp \
            src/Plot3DDialog.cpp \
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
            src/SurfaceDialog.cpp \
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
            src/Bar.cpp \
            src/Cone3D.cpp \
            src/DataSetDialog.cpp \
            src/ConfigDialog.cpp \
            src/QwtBarCurve.cpp \
            src/BoxCurve.cpp \
            src/QwtHistogram.cpp \
            src/VectorCurve.cpp \
            src/Matrix.cpp \
            src/MyParser.cpp\
            src/ColorBox.cpp \
            src/SymbolBox.cpp \
            src/PatternBox.cpp \
            src/main.cpp \
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
            src/ScriptEdit.cpp\
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
            src/ScriptingEnv.cpp\
            src/Script.cpp\
            src/ScriptingLangDialog.cpp\
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
            src/version.cpp \

###################### FORMS ##############################################
FORMS +=   src/SciDAVisAbout.ui

########### Future code backported from the aspect framework ##################
DEFINES += LEGACY_CODE_0_2_x
INCLUDEPATH  += src
INCLUDEPATH  += src/future

FORMS += src/future/matrix/matrixcontroltabs.ui \
         src/future/core/ProjectConfigPage.ui \
		 src/future/table/controltabs.ui \
		 src/future/table/DimensionsDialog.ui

HEADERS += src/future/core/AbstractAspect.h \
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

SOURCES += src/future/core/AbstractAspect.cpp \
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

SOURCES += ../3rdparty/minigzip/minigzip.c

###############################################################
#### Origin OPJ import via liborigin2
###############################################################
contains(DEFINES, ORIGIN_IMPORT) {
	HEADERS += src/importOPJ.h
	SOURCES += src/importOPJ.cpp
}
