/***************************************************************************
    File                 : MatrixView.cpp
    Project              : SciDAVis
    Description          : View class for Matrix
    --------------------------------------------------------------------
    Copyright            : (C) 2008-2009 Tilman Benkert (thzs*gmx.net)
                           (replace * with @ in the email addresses) 

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *  This program is free software; you can redistribute it and/or modify   *
 *  it under the terms of the GNU General Public License as published by   *
 *  the Free Software Foundation; either version 2 of the License, or      *
 *  (at your option) any later version.                                    *
 *                                                                         *
 *  This program is distributed in the hope that it will be useful,        *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *  GNU General Public License for more details.                           *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the Free Software           *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor,                    *
 *   Boston, MA  02110-1301  USA                                           *
 *                                                                         *
 ***************************************************************************/

#include <limits>

#include "matrix/future_Matrix.h"
#include "matrix/MatrixView.h"
#include "matrix/MatrixModel.h"
#include "matrix/matrixcommands.h"

#include "core/AbstractFilter.h"

#include <QKeyEvent>
#include <QtDebug>
#include <QHeaderView>
#include <QRect>
#include <QPoint>
#include <QSize>
#include <QFontMetrics>
#include <QFont>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QShortcut>
#include <QModelIndex>
#include <QGridLayout>
#include <QScrollArea>
#include <QMenu>
#include <QtDebug>

#ifndef LEGACY_CODE_0_2_x
MatrixView::MatrixView(future::Matrix *matrix)
 : d_matrix(matrix) 
#else
MatrixView::MatrixView(const QString& label, QWidget* parent, const QString name, Qt::WFlags f)
 : MyWidget(label, parent, name, f)
#endif
{
#ifndef LEGACY_CODE_0_2_x
	d_model = new MatrixModel(matrix);
	init();
#else
	d_model = NULL;
#endif
}

MatrixView::~MatrixView() 
{
	delete d_model;
}

void MatrixView::setMatrix(future::Matrix * matrix)
{
	d_matrix = matrix;
	d_model = new MatrixModel(matrix);
	init();
}

void MatrixView::init()
{
	d_main_layout = new QHBoxLayout(this);
	d_main_layout->setSpacing(0);
	d_main_layout->setContentsMargins(0, 0, 0, 0);
	
	d_view_widget = new MatrixViewWidget(this);
	d_view_widget->setModel(d_model);
	connect(d_view_widget, SIGNAL(advanceCell()), this, SLOT(advanceCell()));
	d_main_layout->addWidget(d_view_widget);
	
	d_hide_button = new QToolButton();
	d_hide_button->setArrowType(Qt::RightArrow);
	d_hide_button->setSizePolicy(QSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding));
	d_hide_button->setCheckable(false);
	d_main_layout->addWidget(d_hide_button);
	connect(d_hide_button, SIGNAL(pressed()), this, SLOT(toggleControlTabBar()));
	d_control_tabs = new QWidget();
    ui.setupUi(d_control_tabs);
#if 0 // this seems not to work
	ui.first_row_spinbox->setMaximum(std::numeric_limits<double>::max());
	ui.first_row_spinbox->setMinimum(std::numeric_limits<double>::min());
	ui.first_col_spinbox->setMaximum(std::numeric_limits<double>::max());
	ui.first_col_spinbox->setMinimum(std::numeric_limits<double>::min());
	ui.last_row_spinbox->setMaximum(std::numeric_limits<double>::max());
	ui.last_row_spinbox->setMinimum(std::numeric_limits<double>::min());
	ui.last_col_spinbox->setMaximum(std::numeric_limits<double>::max());
	ui.last_col_spinbox->setMinimum(std::numeric_limits<double>::min());
#endif

	updateCoordinatesTab();
	updateFormulaTab();
	updateFormatTab();
	d_main_layout->addWidget(d_control_tabs);

	d_view_widget->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	d_main_layout->setStretchFactor(d_view_widget, 1);

	setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));

	d_view_widget->setFocusPolicy(Qt::StrongFocus);
	setFocusPolicy(Qt::StrongFocus);
	setFocus();
#if QT_VERSION >= 0x040300
	d_view_widget->setCornerButtonEnabled(true);
#endif

	d_view_widget->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QHeaderView * h_header = d_view_widget->horizontalHeader();
	QHeaderView * v_header = d_view_widget->verticalHeader();
	v_header->setResizeMode(QHeaderView::Interactive);
	h_header->setResizeMode(QHeaderView::Interactive);
	v_header->setMovable(false);
	h_header->setMovable(false);
	v_header->setDefaultSectionSize(future::Matrix::defaultRowHeight());
	h_header->setDefaultSectionSize(future::Matrix::defaultColumnWidth());

	v_header->installEventFilter(this);
	h_header->installEventFilter(this);
	d_view_widget->installEventFilter(this);

	rereadSectionSizes();

#ifndef LEGACY_CODE_0_2_x
	// keyboard shortcuts
	QShortcut * sel_all = new QShortcut(QKeySequence(tr("Ctrl+A", "Matrix: select all")), d_view_widget);
	connect(sel_all, SIGNAL(activated()), d_view_widget, SLOT(selectAll()));
#endif

	connect(ui.button_set_coordinates, SIGNAL(pressed()), 
		this, SLOT(applyCoordinates()));
#ifndef LEGACY_CODE_0_2_x
	connect(ui.button_set_formula, SIGNAL(pressed()), 
		this, SLOT(applyFormula()));
#endif
	connect(ui.button_set_format, SIGNAL(pressed()), 
		this, SLOT(applyFormat()));
	connect(ui.format_box, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTypeInfo()));
	connect(ui.digits_box, SIGNAL(valueChanged(int)), this, SLOT(updateTypeInfo()));

	connect(d_matrix, SIGNAL(coordinatesChanged()), this, SLOT(updateCoordinatesTab()));
	connect(d_matrix, SIGNAL(formulaChanged()), this, SLOT(updateFormulaTab()));
	connect(d_matrix, SIGNAL(formatChanged()), this, SLOT(updateFormatTab()));

	retranslateStrings();
}

void MatrixView::rereadSectionSizes()
{
	QHeaderView * h_header = d_view_widget->horizontalHeader();
	QHeaderView * v_header = d_view_widget->verticalHeader();

	disconnect(v_header, SIGNAL(sectionResized(int, int, int)), this, SLOT(handleVerticalSectionResized(int, int, int)));
	disconnect(h_header, SIGNAL(sectionResized(int, int, int)), this, SLOT(handleHorizontalSectionResized(int, int, int)));

	int cols = d_matrix->columnCount();
	for (int i=0; i<cols; i++)
		h_header->resizeSection(i, d_matrix->columnWidth(i));
	int rows = d_matrix->rowCount();
	for (int i=0; i<rows; i++)
		v_header->resizeSection(i, d_matrix->rowHeight(i));
		
	connect(v_header, SIGNAL(sectionResized(int, int, int)), this, SLOT(handleVerticalSectionResized(int, int, int)));
	connect(h_header, SIGNAL(sectionResized(int, int, int)), this, SLOT(handleHorizontalSectionResized(int, int, int)));
}

void MatrixView::changeEvent(QEvent * event)
{
	if (event->type() == QEvent::LanguageChange) 
		retranslateStrings();
	MyWidget::changeEvent(event);	
}

void MatrixView::retranslateStrings()
{
	d_hide_button->setToolTip(tr("Show/hide control tabs"));
    ui.retranslateUi(d_control_tabs);
	ui.format_box->clear();
	ui.format_box->addItem(tr("Decimal"), QVariant('f'));
	ui.format_box->addItem(tr("Scientific (e)"), QVariant('e'));
	ui.format_box->addItem(tr("Scientific (E)"), QVariant('E'));
	ui.format_box->addItem(tr("Automatic (e)"), QVariant('g'));
	ui.format_box->addItem(tr("Automatic (E)"), QVariant('G'));
}
	
void MatrixView::advanceCell()
{
	QModelIndex idx = d_view_widget->currentIndex();
    if(idx.row()+1 < d_matrix->rowCount())
		d_view_widget->setCurrentIndex(idx.sibling(idx.row()+1, idx.column()));
}

void MatrixView::goToCell(int row, int col)
{
	QModelIndex index = d_model->index(row, col);
	d_view_widget->scrollTo(index);
	d_view_widget->setCurrentIndex(index);
}

void MatrixView::selectAll()
{
	d_view_widget->selectAll();
}

void MatrixView::toggleControlTabBar() 
{ 
	d_control_tabs->setVisible(!d_control_tabs->isVisible());
	if(d_control_tabs->isVisible())
		d_hide_button->setArrowType(Qt::RightArrow);
	else
		d_hide_button->setArrowType(Qt::LeftArrow);
	emit controlTabBarStatusChanged(d_control_tabs->isVisible());
}

int MatrixView::selectedColumnCount(bool full)
{
	int count = 0;
	int cols = d_matrix->columnCount();
	for (int i=0; i<cols; i++)
		if(isColumnSelected(i, full)) count++;
	return count;
}

bool MatrixView::isColumnSelected(int col, bool full)
{
	if(full)
		return d_view_widget->selectionModel()->isColumnSelected(col, QModelIndex());
	else
		return d_view_widget->selectionModel()->columnIntersectsSelection(col, QModelIndex());
}

int MatrixView::selectedRowCount(bool full)
{
	int count = 0;
	int rows = d_matrix->rowCount();
	for (int i=0; i<rows; i++)
		if(isRowSelected(i, full)) count++;
	return count;
}

bool MatrixView::isRowSelected(int row, bool full)
{
	if(full)
		return d_view_widget->selectionModel()->isRowSelected(row, QModelIndex());
	else
		return d_view_widget->selectionModel()->rowIntersectsSelection(row, QModelIndex());
}

int MatrixView::firstSelectedColumn(bool full)
{
	int cols = d_matrix->columnCount();
	for (int i=0; i<cols; i++)
	{
		if(isColumnSelected(i, full))
			return i;
	}
	return -1;
}

int MatrixView::lastSelectedColumn(bool full)
{
	int cols = d_matrix->columnCount();
	for(int i=cols-1; i>=0; i--)
		if(isColumnSelected(i, full)) return i;

	return -2;
}

int MatrixView::firstSelectedRow(bool full)
{
	int rows = d_matrix->rowCount();
	for (int i=0; i<rows; i++)
	{
		if(isRowSelected(i, full))
			return i;
	}
	return -1;
}

int MatrixView::lastSelectedRow(bool full)
{
	int rows = d_matrix->rowCount();
	for(int i=rows-1; i>=0; i--)
		if(isRowSelected(i, full)) return i;

	return -2;
}

bool MatrixView::isCellSelected(int row, int col)
{
	if(row < 0 || col < 0 || row >= d_matrix->rowCount() || col >= d_matrix->columnCount()) return false;

	return d_view_widget->selectionModel()->isSelected(d_model->index(row, col));
}

void MatrixView::setCellSelected(int row, int col)
{
	 d_view_widget->selectionModel()->select(d_model->index(row, col), QItemSelectionModel::Select);
}

void MatrixView::setCellsSelected(int first_row, int first_col, int last_row, int last_col)
{
	QModelIndex top_left = d_model->index(first_row, first_col);
	QModelIndex bottom_right = d_model->index(last_row, last_col);
	d_view_widget->selectionModel()->select(QItemSelection(top_left, bottom_right), QItemSelectionModel::SelectCurrent);
}

void MatrixView::getCurrentCell(int * row, int * col)
{
	QModelIndex index = d_view_widget->selectionModel()->currentIndex();
	if(index.isValid()) 
	{
		*row = index.row();
		*col = index.column();
	}
	else
	{
		*row = -1;
		*col = -1;
	}
}

bool MatrixView::eventFilter(QObject * watched, QEvent * event)
{
	QHeaderView * v_header = d_view_widget->verticalHeader();
	QHeaderView * h_header = d_view_widget->horizontalHeader();

	if (event->type() == QEvent::ContextMenu) 
	{
		QContextMenuEvent *cm_event = static_cast<QContextMenuEvent *>(event);
		QPoint global_pos = cm_event->globalPos();
		if(watched == v_header)	
			d_matrix->showMatrixViewRowContextMenu(global_pos);
		else if(watched == h_header)
			d_matrix->showMatrixViewColumnContextMenu(global_pos);
		else if(watched == d_view_widget)
			d_matrix->showMatrixViewContextMenu(global_pos);
		else
			return MyWidget::eventFilter(watched, event);
			
		event->accept();
		return true;
	} 
	else 
			return MyWidget::eventFilter(watched, event);
}
	
void MatrixView::showControlCoordinatesTab()
{
	d_control_tabs->setVisible(true);
	d_hide_button->setArrowType(Qt::RightArrow);
	ui.tab_widget->setCurrentIndex(0);
	ui.tab_widget->setFocus();
	emit controlTabBarStatusChanged(d_control_tabs->isVisible());
}

void MatrixView::showControlFormatTab()
{
	d_control_tabs->setVisible(true);
	d_hide_button->setArrowType(Qt::RightArrow);
	ui.tab_widget->setCurrentIndex(1);
	ui.tab_widget->setFocus();
	emit controlTabBarStatusChanged(d_control_tabs->isVisible());
}

void MatrixView::showControlFormulaTab()
{
	d_control_tabs->setVisible(true);
	d_hide_button->setArrowType(Qt::RightArrow);
	ui.tab_widget->setCurrentIndex(2);
	ui.tab_widget->setFocus();
	emit controlTabBarStatusChanged(d_control_tabs->isVisible());
}

void MatrixView::applyCoordinates()
{
	d_matrix->setCoordinates(ui.first_col_spinbox->value(), ui.last_col_spinbox->value(), 
			ui.first_row_spinbox->value(), ui.last_row_spinbox->value());
}

void MatrixView::updateCoordinatesTab()
{
	ui.first_col_spinbox->setValue(d_matrix->xStart());
	ui.last_col_spinbox->setValue(d_matrix->xEnd());
	ui.first_row_spinbox->setValue(d_matrix->yStart());
	ui.last_row_spinbox->setValue(d_matrix->yEnd());
}

void MatrixView::updateFormulaTab()
{
	ui.formula_box->setPlainText(d_matrix->formula());
}

#ifndef LEGACY_CODE_0_2_x
void MatrixView::applyFormula()
{
	d_matrix->setFormula(ui.formula_box->toPlainText());
}
#endif

void MatrixView::updateFormatTab()
{
	ui.digits_box->setValue(d_matrix->displayedDigits());
	int format_index = ui.format_box->findData(d_matrix->numericFormat());
	ui.format_box->setCurrentIndex(format_index);
}

void MatrixView::applyFormat()
{
	int digits = ui.digits_box->value();
	int format_index = ui.format_box->currentIndex();
	if (format_index >= 0)
	{
		char format = ui.format_box->itemData(format_index).toChar().toLatin1();
		d_matrix->setNumericFormat(format);
	}
	d_matrix->setDisplayedDigits(digits);
}

void MatrixView::handleHorizontalSectionResized(int logicalIndex, int oldSize, int newSize)
{
	static bool inside = false;
	d_matrix->setColumnWidth(logicalIndex, newSize);
	if (inside) return;
	inside = true;

	QHeaderView * h_header = d_view_widget->horizontalHeader();
	int cols = d_matrix->columnCount();
	for (int i=0; i<cols; i++)
		if(isColumnSelected(i, true)) 
			h_header->resizeSection(i, newSize);

	inside = false;
}

void MatrixView::handleVerticalSectionResized(int logicalIndex, int oldSize, int newSize)
{
	static bool inside = false;
	d_matrix->setRowHeight(logicalIndex, newSize);
	if (inside) return;
	inside = true;

	QHeaderView * v_header = d_view_widget->verticalHeader();
	int rows = d_matrix->rowCount();
	for (int i=0; i<rows; i++)
		if(isRowSelected(i, true)) 
			v_header->resizeSection(i, newSize);

	inside = false;
}

void MatrixView::setRowHeight(int row, int height) 
{ 
	QHeaderView * v_header = d_view_widget->verticalHeader();
	v_header->resizeSection(row, height);
}

void MatrixView::setColumnWidth(int col, int width) 
{ 
	QHeaderView * h_header = d_view_widget->horizontalHeader();
	h_header->resizeSection(col, width);
}

int MatrixView::rowHeight(int row) const 
{ 
	QHeaderView * v_header = d_view_widget->verticalHeader();
	return v_header->sectionSize(row);
}

int MatrixView::columnWidth(int col) const 
{ 
	QHeaderView * h_header = d_view_widget->horizontalHeader();
	return h_header->sectionSize(col);
}

void MatrixView::updateTypeInfo()
{
	int digits = ui.digits_box->value();
	int format_index = ui.format_box->currentIndex();
	if (format_index >= 0)
	{
		char format = ui.format_box->itemData(format_index).toChar().toLatin1();
		ui.type_info->setText(tr("Example: %1").arg(QString::number(100.1234567890123456, format, digits)));
	}
}

/* ================== MatrixViewWidget ================ */

void MatrixViewWidget::selectAll()
{
	// the original QTableView::selectAll() toggles all cells which is strange behavior IMHO - thzs
	QItemSelectionModel * sel_model = selectionModel();
	QItemSelection sel(model()->index(0, 0, QModelIndex()), model()->index(model()->rowCount()-1, 
		model()->columnCount()-1, QModelIndex()));
	sel_model->select(sel, QItemSelectionModel::Select);
}

void MatrixViewWidget::keyPressEvent(QKeyEvent * event)
{
	bool advance = false;
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
		advance = true;
	QTableView::keyPressEvent(event);
	if (advance)
		emit advanceCell();
}

