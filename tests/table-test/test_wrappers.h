#include "Column.h"
#include "TableModel.h"
#include <QTableView>
#include <QInputDialog>
#include <QMessageBox>
#include <QContextMenuEvent>
#include <QMenu>

class ColumnTestWrapper : public Column
{
	
	public:
		virtual QUndoStack *undoStack() const 
		{ 
			static QUndoStack * undo_stack = 0;
			if(!undo_stack) undo_stack = new QUndoStack();
			return undo_stack; 
		}


		ColumnTestWrapper(const QString& label, SciDAVis::ColumnMode mode) : Column(label, mode) {};
		ColumnTestWrapper(const QString& label, QVector<double> data, IntervalAttribute<bool> validity = IntervalAttribute<bool>())
			: Column(label, data, validity) {};
		ColumnTestWrapper(const QString& label, QStringList data, IntervalAttribute<bool> validity = IntervalAttribute<bool>()) 
			: Column(label, data, validity) {};
		ColumnTestWrapper(const QString& label, QList<QDateTime> data, IntervalAttribute<bool> validity = IntervalAttribute<bool>())
			: Column(label, data, validity) {};
};


class TableViewTestWrapper : public QTableView
{
	Q_OBJECT

	public:
		TableViewTestWrapper(shared_ptr<ColumnTestWrapper> * cols, QWidget * parent = 0) 
			: QTableView(parent), column(cols) {};
		~TableViewTestWrapper() {};

	public slots:
		void insertRows()
		{
			int col = QInputDialog::getInteger(this, "insertRows", "column :");	
			int before = QInputDialog::getInteger(this, "insertRows", "before :");	
			int count = QInputDialog::getInteger(this, "insertRows", "count :");	
			column[col]->insertRows(before, count);
		}
		void insertRowsAll()
		{
			int before = QInputDialog::getInteger(this, "insertRowsAll", "before :");	
			int count = QInputDialog::getInteger(this, "insertRowsAll", "count :");	
			static_cast<TableModel *>(model())->insertRows(before, count);
		}
		void removeRows()
		{
			int col = QInputDialog::getInteger(this, "removeRows", "column :");	
			int first = QInputDialog::getInteger(this, "removeRows", "first :");	
			int count = QInputDialog::getInteger(this, "removeRows", "count :");	
			column[col]->removeRows(first, count);
		}
		void removeRowsAll()
		{
			int first = QInputDialog::getInteger(this, "removeRowsAll", "first :");	
			int count = QInputDialog::getInteger(this, "removeRowsAll", "count :");	
			static_cast<TableModel *>(model())->removeRows(first, count);
		}

	protected:
		void contextMenuEvent ( QContextMenuEvent * e ) 
		{
			QMenu contextMenu(this);
			contextMenu.addAction("insertRows", this, SLOT(insertRows()));
			contextMenu.addAction("removeRows", this, SLOT(removeRows()));
			contextMenu.addAction("insertRowsAll", this, SLOT(insertRowsAll()));
			contextMenu.addAction("removeRowsAll", this, SLOT(removeRowsAll()));
			contextMenu.exec(e->globalPos());
		}
		
	private:
		shared_ptr<ColumnTestWrapper> *column;
};

