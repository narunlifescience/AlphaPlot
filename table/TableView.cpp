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
	connect(d_view, SIGNAL(advanceCell()), this, SLOT(advanceCell()));

	d_main_layout = new QVBoxLayout(this);
	d_main_layout->setSpacing(0);
	d_main_layout->setContentsMargins(0, 0, 0, 0);
	d_main_layout->addWidget(d_view);

	d_options_bar = new QWidget();
	d_main_layout->addWidget(d_options_bar);
	d_sub_layout =  new QVBoxLayout(d_options_bar);
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
	d_sub_layout->addWidget(d_tool_box);

	d_delegate = new TableItemDelegate(d_view);
	d_view->setItemDelegate(d_delegate);
	
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
	QHeaderView * h_header = d_view->horizontalHeader();
	// Remark: ResizeToContents works in Qt 4.2.3 but is broken in 4.3.0
	// Should be fixed in 4.3.1 though, see:
	// http://trolltech.com/developer/task-tracker/index_html?method=entry&id=165567
	v_header->setResizeMode(QHeaderView::ResizeToContents);
	v_header->setMovable(false);
	h_header->setDefaultAlignment(Qt::AlignTop);
	h_header->setResizeMode(QHeaderView::Interactive);
	h_header->setMovable(true);
	h_header->setDefaultAlignment(Qt::AlignLeft | Qt::AlignTop);
	connect(h_header, SIGNAL(sectionMoved(int,int,int)), this, SLOT(horizontalSectionMovedHandler(int,int,int)));
	h_header->viewport()->installEventFilter(this);
	v_header->viewport()->installEventFilter(this);

	connect(d_model, SIGNAL(headerDataChanged(Qt::Orientation,int,int)), d_view, SLOT(updateHeaderGeometry(Qt::Orientation,int,int)) ); 

    setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(emitContextMenuRequest(const QPoint&)));
	
	// keyboard shortcuts
	QShortcut * sel_all = new QShortcut(QKeySequence(tr("Ctrl+A", "Table: select all")), d_view);
	connect(sel_all, SIGNAL(activated()), d_view, SLOT(selectAll()));

	retranslateStrings();
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
	QHeaderView * h_header = d_view->horizontalHeader();

	if (e->type() == QEvent::MouseButtonPress) 
	{
		const QMouseEvent *me = static_cast<const QMouseEvent *>(e);
		if (me->button() == Qt::RightButton) 
		{
			if(object == static_cast<QObject *>(h_header->viewport())) 
			{
				QPoint pos = h_header->viewport()->mapToGlobal(me->pos());
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

