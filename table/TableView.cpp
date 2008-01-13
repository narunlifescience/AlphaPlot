/***************************************************************************
    File                 : TableView.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : View class for table data

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

#include "TableView.h"
#include "TableModel.h"
#include "TableItemDelegate.h"
#include "tablecommands.h"
#include "TableDoubleHeaderView.h"

#include "Column.h"
#include "core/AbstractFilter.h"
#include "core/datatypes/SimpleCopyThroughFilter.h"
#include "core/datatypes/Double2StringFilter.h"
#include "core/datatypes/String2DoubleFilter.h"
#include "core/datatypes/DateTime2StringFilter.h"
#include "core/datatypes/String2DateTimeFilter.h"

#include <QKeyEvent>
#include <QtDebug>
#include <QHeaderView>
#include <QRect>
#include <QSize>
#include <QFontMetrics>
#include <QFont>
#include <QItemSelectionModel>
#include <QItemSelection>
#include <QShortcut>
#include <QModelIndex>
#include <QGridLayout>
#include <QScrollArea>

TableView::TableView(QWidget * parent, TableModel * model )
 : QWidget( parent ), d_model(model)
{
	init(model);
}

void TableView::init(TableModel * model)
{
	d_view = new TableViewWidget();
	d_view->setModel(model);
	d_view->setSelectionModel(model->selectionModel());
	connect(d_view, SIGNAL(advanceCell()), this, SLOT(advanceCell()));

	d_main_layout = new QVBoxLayout(this);
	d_main_layout->setSpacing(0);
	d_main_layout->setContentsMargins(0, 0, 0, 0);
	
	d_horizontal_header = new TableDoubleHeaderView();
    d_horizontal_header->setClickable(true);
    d_horizontal_header->setHighlightSections(true);
	d_view->setHorizontalHeader(d_horizontal_header);

	d_main_layout->addWidget(d_view);

	d_options_bar = new QWidget();
	d_sub_layout = new QVBoxLayout(d_options_bar);
	d_sub_layout->setSpacing(0);
	d_sub_layout->setContentsMargins(0, 0, 0, 0);
	d_hide_button = new QToolButton();
	d_hide_button->setArrowType(Qt::DownArrow);
	d_hide_button->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	d_hide_button->setCheckable(false);
	d_sub_layout->addWidget(d_hide_button);
	connect(d_hide_button, SIGNAL(pressed()), this, SLOT(toggleOptionTabBar()));
	d_options_tabs = new QWidget();
    ui.setupUi(d_options_tabs);
	d_tool_box = new QScrollArea();
	d_tool_box->setWidget(d_options_tabs);
	d_options_tabs->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed));
	d_tool_box->setWidgetResizable(true);
	d_sub_layout->addWidget(d_tool_box);

	d_delegate = new TableItemDelegate(d_view);
	d_view->setItemDelegate(d_delegate);
	
	d_main_layout->addWidget(d_options_bar);
	d_view->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
	d_main_layout->setStretchFactor(d_view, 1);

	d_view->setFocusPolicy(Qt::StrongFocus);
	setFocusPolicy(Qt::StrongFocus);
	setFocus();
#if QT_VERSION >= 0x040300
	d_view->setCornerButtonEnabled(true);
#endif

	d_view->setSelectionMode(QAbstractItemView::ExtendedSelection);

	QHeaderView * v_header = d_view->verticalHeader();
	// Remark: ResizeToContents works in Qt 4.2.3 but is broken in 4.3.0
	// Should be fixed in 4.3.1 though, see:
	// http://trolltech.com/developer/task-tracker/index_html?method=entry&id=165567
	v_header->setResizeMode(QHeaderView::ResizeToContents);
	v_header->setMovable(false);
	d_horizontal_header->setResizeMode(QHeaderView::Interactive);
	d_horizontal_header->setMovable(true);
	connect(d_horizontal_header, SIGNAL(sectionMoved(int,int,int)), this, SLOT(horizontalSectionMovedHandler(int,int,int)));
	d_horizontal_header->viewport()->installEventFilter(this);
	v_header->viewport()->installEventFilter(this);

	connect(d_model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)), d_view, SLOT(updateHeaderGeometry(Qt::Orientation,int,int)) ); 

    setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(emitContextMenuRequest(const QPoint&)));
	
	// keyboard shortcuts
	QShortcut * sel_all = new QShortcut(QKeySequence(tr("Ctrl+A", "Table: select all")), d_view);
	connect(sel_all, SIGNAL(activated()), d_view, SLOT(selectAll()));

	connect(ui.type_box, SIGNAL(currentIndexChanged(int)), this, SLOT(updateFormatBox()));
	connect(ui.format_box, SIGNAL(currentIndexChanged(int)), this, SLOT(updateTypeInfo()));
	connect(ui.digits_box, SIGNAL(valueChanged(int)), this, SLOT(updateTypeInfo()));
	retranslateStrings();

	QItemSelectionModel * sel_model = model->selectionModel();

	connect(sel_model, SIGNAL(currentColumnChanged(const QModelIndex&, const QModelIndex&)), 
		this, SLOT(currentColumnChanged(const QModelIndex&, const QModelIndex&)));
	connect(sel_model, SIGNAL(selectionChanged(const QItemSelection&,const QItemSelection&)),
		this, SLOT(selectionChanged(const QItemSelection&,const QItemSelection&)));
	connect(ui.button_set_description, SIGNAL(pressed()), 
		this, SLOT(applyDescription()));
	connect(ui.button_set_type, SIGNAL(pressed()),
		this, SLOT(applyType()));
}

void TableView::changeEvent(QEvent * event)
{
	if (event->type() == QEvent::LanguageChange) 
		retranslateStrings();
	else
         QWidget::changeEvent(event);	
}

void TableView::retranslateStrings()
{
	d_hide_button->setToolTip(tr("Show/hide options"));
    ui.retranslateUi(d_tool_box);

	ui.type_box->clear();
	ui.type_box->addItem(tr("Numeric"), QVariant(int(SciDAVis::Numeric)));
	ui.type_box->addItem(tr("Text"), QVariant(int(SciDAVis::Text)));
	ui.type_box->addItem(tr("Month names"), QVariant(int(SciDAVis::Month)));
	ui.type_box->addItem(tr("Day names"), QVariant(int(SciDAVis::Day)));
	ui.type_box->addItem(tr("Date and time"), QVariant(int(SciDAVis::DateTime)));

	ui.type_box->setCurrentIndex(0);

	// TODO: implement formula stuff
	ui.formula_info->document()->setPlainText("not implemented yet");
}
	
TableView::~TableView() 
{
}

void TableView::advanceCell()
{
	QModelIndex idx = d_view->currentIndex();
    if(idx.row()+1 >= d_model->rowCount())
	{
		int new_size = d_model->rowCount()+1;
		emit requestResize(new_size);
		if(d_model->rowCount() != new_size) // request was ignored
			d_model->setRowCount(new_size);

	}
	d_view->setCurrentIndex(idx.sibling(idx.row()+1, idx.column()));
}

void TableView::emitContextMenuRequest(const QPoint& pos)
{
	emit requestContextMenu(this, mapToGlobal(pos));
}

void TableView::scrollToIndex(const QModelIndex & index)
{
	d_view->scrollTo(index);
	d_view->setCurrentIndex(index);
}

bool TableView::eventFilter(QObject *object, QEvent *e)
{
	QHeaderView * v_header = d_view->verticalHeader();

	if (e->type() == QEvent::MouseButtonPress) 
	{
		const QMouseEvent *me = static_cast<const QMouseEvent *>(e);
		if (me->button() == Qt::RightButton) 
		{
			if(object == static_cast<QObject *>(d_horizontal_header->viewport())) 
			{
				QPoint pos = d_horizontal_header->viewport()->mapToGlobal(me->pos());
				emit requestColumnContextMenu(this, pos);
				return true;
			}
			else if (object == static_cast<QObject *>(v_header->viewport())) 
			{
				QPoint pos = v_header->viewport()->mapToGlobal(me->pos());
				emit requestRowContextMenu(this, pos);
				return true;
			}
		}
	}

	return QWidget::eventFilter(object, e);
}

void TableView::selectAll()
{
	d_view->selectAll();
}

void TableView::toggleOptionTabBar() 
{ 
	d_tool_box->setVisible(!d_tool_box->isVisible());
	if(d_tool_box->isVisible())
		d_hide_button->setArrowType(Qt::DownArrow);
	else
		d_hide_button->setArrowType(Qt::UpArrow);
}


void TableViewWidget::selectAll()
{
	TableModel * table_model = static_cast<TableModel *>(model());
	QItemSelectionModel * sel_model = table_model->selectionModel();
	QItemSelection sel(table_model->index(0, 0, QModelIndex()), table_model->index(table_model->rowCount()-1, table_model->columnCount()-1, QModelIndex()));
	sel_model->select(sel, QItemSelectionModel::Select);
}

void TableViewWidget::updateHeaderGeometry(Qt::Orientation o, int first, int last)
{
	Q_UNUSED(first)
	Q_UNUSED(last)
	if(o != Qt::Horizontal) return;
	horizontalHeader()->setStretchLastSection(true);  // ugly hack (flaw in Qt? Does anyone know a better way?)
	horizontalHeader()->updateGeometry();
	horizontalHeader()->setStretchLastSection(false); // ugly hack part 2
}

void TableViewWidget::keyPressEvent(QKeyEvent * event)
{
    if(event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
		emit advanceCell();
	QTableView::keyPressEvent(event);
}

void TableView::horizontalSectionMovedHandler(int index, int from, int to)
{
	static bool inside = false;
	if(inside) return;

	Q_ASSERT(index == from);

	inside = true;
	d_view->horizontalHeader()->moveSection(to, from);
	inside = false;
	emit columnMoved(from, to);
}

bool TableView::areCommentsShown() const
{
	return d_horizontal_header->areCommentsShown();
}

void TableView::toggleComments()
{
	showComments(!areCommentsShown());
}

void TableView::showComments(bool on)
{
	d_horizontal_header->showComments(on);
}

void TableView::currentColumnChanged(const QModelIndex & current, const QModelIndex & previous)
{
	Q_UNUSED(previous);
	int col = current.column();	
	if(col < 0 || col >= d_model->columnCount()) return;
	shared_ptr<Column> col_ptr = d_model->column(col);

	QString str = QString(tr("Current column:\nName: %1\nPosition: %2"))\
		.arg(col_ptr->columnLabel()).arg(col+1);
		
	ui.column_info->document()->setPlainText(str);
	ui.name_edit->setText(col_ptr->columnLabel());
	ui.comment_box->document()->setPlainText(col_ptr->columnComment());
}

void TableView::selectionChanged(const QItemSelection & selected, const QItemSelection & deselected)
{
	

}

void TableView::updateFormatBox()
{
	int type_index = ui.type_box->currentIndex();
	if(type_index < 0) return; // should never happen
	ui.format_box->clear();
	ui.digits_box->setEnabled(false);
	switch(ui.type_box->itemData(type_index).toInt())
	{
		case SciDAVis::Numeric:
			ui.digits_box->setEnabled(true);
			ui.format_box->addItem(tr("Decimal"), QVariant('f'));
			ui.format_box->addItem(tr("Scientific"), QVariant('E'));
			break;
		case SciDAVis::Text:
			ui.format_box->addItem(tr("Text"), QVariant());
			break;
		case SciDAVis::Month:
			ui.format_box->addItem(tr("Number without leading zero"), QVariant("M"));
			ui.format_box->addItem(tr("Number with leading zero"), QVariant("MM"));
			ui.format_box->addItem(tr("Abbreviated month name"), QVariant("MMM"));
			ui.format_box->addItem(tr("Full month name"), QVariant("MMMM"));
			break;
		case SciDAVis::Day:
			ui.format_box->addItem(tr("Number without leading zero"), QVariant("d"));
			ui.format_box->addItem(tr("Number with leading zero"), QVariant("dd"));
			ui.format_box->addItem(tr("Abbreviated day name"), QVariant("ddd"));
			ui.format_box->addItem(tr("Full day name"), QVariant("dddd"));
			break;
		case SciDAVis::DateTime:
			{
				// TODO: allow adding of the combo box entries here
				const char * date_strings[] = {
					"yyyy-MM-dd", 	
					"yyyy/MM/dd", 
					"dd/MM/yyyy", 
					"dd/MM/yy", 
					"dd.MM.yyyy", 	
					"dd.MM.yy",
					"MM/yyyy",
					"dd.MM.", 
					"yyyyMMdd",
					0
				};

				const char * time_strings[] = {
					"hh",
					"hh ap",
					"hh:mm",
					"hh:mm ap",
					"hh:mm:ss",
					"hh:mm:ss.zzz",
					"hh:mm:ss:zzz",
					"mm:ss.zzz",
					"hhmmss",
					0
				};
				int j,i;
				for(i=0; date_strings[i] != 0; i++)
					ui.format_box->addItem(QString(date_strings[i]), QVariant(date_strings[i]));
				for(j=0; time_strings[j] != 0; j++)
					ui.format_box->addItem(QString(time_strings[j]), QVariant(time_strings[j]));
				for(i=0; date_strings[i] != 0; i++)
					for(j=0; time_strings[j] != 0; j++)
						ui.format_box->addItem(QString("%1 %2").arg(date_strings[i]).arg(time_strings[j]), 
							QVariant(QString(date_strings[i]) + " " + QString(time_strings[j])));
				break;
			}
		default:
			ui.format_box->addItem(QString()); // just for savety to have at least one item in any case
	}
	ui.format_box->setCurrentIndex(0);
}

void TableView::updateTypeInfo()
{
	int format_index = ui.format_box->currentIndex();
	int type_index = ui.type_box->currentIndex();

	QString str = tr("Selected column type:\n");
	if(format_index >= 0 && type_index >= 0)
	{
		int type = ui.type_box->itemData(type_index).toInt();
		switch(type)
		{
			case SciDAVis::Numeric:
				str += tr("Double precision\nfloating point values\n");
				ui.digits_box->setEnabled(true);
				break;
			case SciDAVis::Text:
				str += tr("Text\n");
				break;
			case SciDAVis::Month:
				str += tr("Month names\n");
				break;
			case SciDAVis::Day:
				str += tr("Days of the week\n");
				break;
			case SciDAVis::DateTime:
				str += tr("Dates and/or times\n");
				break;
		}
		str += tr("Example: ");
		switch(type)
		{
			case SciDAVis::Numeric:
				str += QString::number(123.1234567890123456, ui.format_box->itemData(format_index).toChar().toLatin1(), ui.digits_box->value());
				break;
			case SciDAVis::Text:
				str += tr("Hello world!\n");
				break;
			case SciDAVis::Month:
				str += QLocale().toString(QDate(1900,1,1), ui.format_box->itemData(format_index).toString());
				break;
			case SciDAVis::Day:
				str += QLocale().toString(QDate(1900,1,1), ui.format_box->itemData(format_index).toString());
				break;
			case SciDAVis::DateTime:
				str += QDateTime(QDate(1900,1,1), QTime(23,59,59,999)).toString(ui.format_box->itemData(format_index).toString());
				break;
		}
	}
	ui.type_info->setText(str);
}

void TableView::showOptionsDescriptionTab()
{
	d_tool_box->setVisible(true);
	d_hide_button->setArrowType(Qt::DownArrow);
	ui.tab_widget->setCurrentIndex(0);
}

void TableView::showOptionsTypeTab()
{
	d_tool_box->setVisible(true);
	d_hide_button->setArrowType(Qt::DownArrow);
	ui.tab_widget->setCurrentIndex(1);
}

void TableView::showOptionsFormulaTab()
{
	d_tool_box->setVisible(true);
	d_hide_button->setArrowType(Qt::DownArrow);
	ui.tab_widget->setCurrentIndex(2);
}

void TableView::applyDescription()
{
	QItemSelectionModel * sel_model = d_model->selectionModel();
	int index = sel_model->currentIndex().column();
	if(index >= 0)
	{
		d_model->column(index)->setColumnLabel(ui.name_edit->text());
		d_model->column(index)->setColumnComment(ui.comment_box->document()->toPlainText());
	}
}

void TableView::applyType()
{
	int format_index = ui.format_box->currentIndex();
	int type_index = ui.type_box->currentIndex();
	if(format_index < 0 && type_index < 0) return;

	SciDAVis::ColumnMode mode = (SciDAVis::ColumnMode)ui.type_box->itemData(type_index).toInt();
	QList< shared_ptr<Column> > list = d_model->selectedColumns();
	switch(mode)
	{
		case SciDAVis::Numeric:
			foreach(shared_ptr<Column> col, list)
			{
				col->setColumnMode(mode);
				shared_ptr<Double2StringFilter> filter = static_pointer_cast<Double2StringFilter>(col->outputFilter());
				filter->setNumericFormat(ui.format_box->itemData(format_index).toChar().toLatin1());
				filter->setNumDigits(ui.digits_box->value());
				d_model->emitColumnChanged(col); // filter changes will not be visible without this
			}
			break;
		case SciDAVis::Text:
			foreach(shared_ptr<Column> col, list)
				col->setColumnMode(mode);
			break;
		case SciDAVis::Month:
		case SciDAVis::Day:
		case SciDAVis::DateTime:
			foreach(shared_ptr<Column> col, list)
			{
				col->setColumnMode(mode);
				shared_ptr<DateTime2StringFilter> filter = static_pointer_cast<DateTime2StringFilter>(col->outputFilter());
				filter->setFormat(ui.format_box->itemData(format_index).toString());
				d_model->emitColumnChanged(col); // filter changes will not be visible without this
			}
			break;
	}
}
