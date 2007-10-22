#include <cppunit/extensions/HelperMacros.h>
#include "assertion_traits.h"

#include "Column.h"
#include "TableModel.h"
#include <QtGlobal>
#include <QtDebug>
#include <QApplication>
#include <QMainWindow>
#include <QTableView>

#define EPSILON (1e-6)

class ColumnWrapper : public Column
{
	
	public:
		virtual QUndoStack *undoStack() const 
		{ 
			static QUndoStack * undo_stack = 0;
			if(!undo_stack) undo_stack = new QUndoStack();
			return undo_stack; 
		}


		ColumnWrapper(const QString& label, SciDAVis::ColumnMode mode) : Column(label, mode) {};
		ColumnWrapper(const QString& label, QVector<double> data, IntervalAttribute<bool> validity = IntervalAttribute<bool>())
			: Column(label, data, validity) {};
		ColumnWrapper(const QString& label, QStringList data, IntervalAttribute<bool> validity = IntervalAttribute<bool>()) 
			: Column(label, data, validity) {};
		ColumnWrapper(const QString& label, QList<QDateTime> data, IntervalAttribute<bool> validity = IntervalAttribute<bool>())
			: Column(label, data, validity) {};
};

class TableTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(TableTest);
		CPPUNIT_TEST(testTableModelUI);
		CPPUNIT_TEST(testTableModel);
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp() 
		{
			QVector<double> double_temp;
			double_temp << 1.1 << 2.2 << 3.3;
			QStringList strl_temp;
			strl_temp << "foo" << "bar" << "qt4";
			QList<QDateTime> dtl_temp;
			dtl_temp << QDateTime(QDate(2000,1,1),QTime(1,2,3,4));
			dtl_temp << QDateTime(QDate(2000,2,2),QTime(1,2,3,4));
			dtl_temp << QDateTime(QDate(2000,3,3),QTime(1,2,3,4));
			IntervalAttribute<bool> temp_validity;
			temp_validity.setValue(Interval<int>(1,2));

			column[0] = shared_ptr<ColumnWrapper>(new ColumnWrapper("col0", SciDAVis::Numeric));
			column[1] = shared_ptr<ColumnWrapper>(new ColumnWrapper("col1", double_temp, temp_validity));
			column[2] = shared_ptr<ColumnWrapper>(new ColumnWrapper("col2", SciDAVis::Text));
			column[3] = shared_ptr<ColumnWrapper>(new ColumnWrapper("col3", strl_temp, temp_validity));
			column[4] = shared_ptr<ColumnWrapper>(new ColumnWrapper("col4", SciDAVis::DateTime));
			column[5] = shared_ptr<ColumnWrapper>(new ColumnWrapper("col5", dtl_temp, temp_validity));
			column[6] = shared_ptr<ColumnWrapper>(new ColumnWrapper("col6", SciDAVis::Month));
			column[7] = shared_ptr<ColumnWrapper>(new ColumnWrapper("col7", SciDAVis::Month));
			column[8] = shared_ptr<ColumnWrapper>(new ColumnWrapper("col8", SciDAVis::Day));
			column[9] = shared_ptr<ColumnWrapper>(new ColumnWrapper("col9", SciDAVis::Day));
			column[10] = shared_ptr<ColumnWrapper>(new ColumnWrapper("col10", double_temp));
		}
		
		void tearDown() 
		{
		}

	private:
		shared_ptr<ColumnWrapper> column[11];

/* ------------------------------------------------------------------------------ */
		void testTableModelUI() 
		{
			int argc=0;
			char ** argv=0;
			QApplication app(argc,argv);
			QMainWindow mw;
			TableModel * table_model = new TableModel();
			QTableView * table_view = new QTableView();
			table_view->setModel(table_model);

			QList<  shared_ptr<Column> > list;

			// expand column 0 to 10 rows
			for(int i=0; i<10; i++)
				column[0]->setValueAt(i,i*11.0);

			// insert all columns into the model
			for(int i=0; i<11; i++)
				list << column[i];
			table_model->insertColumns(0,list);
			
			mw.setCentralWidget(table_view);

			mw.show();
			app.exec();
		}
		/* ----------------------------------------------------------- */
		void testTableModel() 
		{
			shared_ptr<TableModel> table_model(new TableModel());

			QList<  shared_ptr<Column> > list;

			// expand column 0 to 10 rows
			for(int i=0; i<10; i++)
				column[0]->setValueAt(i,i*11.0);

			// insert all columns into the model
			for(int i=0; i<11; i++)
				list << column[i];
			table_model->insertColumns(0,list);
			CPPUNIT_ASSERT_EQUAL(11, table_model->columnCount());
			CPPUNIT_ASSERT_EQUAL(10, table_model->rowCount());

			shared_ptr<ColumnWrapper> temp_col[6];
			temp_col[0] = shared_ptr<ColumnWrapper>(new ColumnWrapper("temp_col0", SciDAVis::Numeric));
			temp_col[1] = shared_ptr<ColumnWrapper>(new ColumnWrapper("temp_col1", SciDAVis::Text));
			temp_col[2] = shared_ptr<ColumnWrapper>(new ColumnWrapper("temp_col2", SciDAVis::DateTime));
			list.clear();
			for(int i=0; i<3; i++)
				list << temp_col[i];

			table_model->replaceColumns(8, list);
			CPPUNIT_ASSERT_EQUAL(11, table_model->columnCount());
			CPPUNIT_ASSERT_EQUAL(10, table_model->rowCount());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Numeric, table_model->output(8)->columnMode());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Text, table_model->output(9)->columnMode());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::DateTime, table_model->output(10)->columnMode());

			temp_col[3] = shared_ptr<ColumnWrapper>(new ColumnWrapper("temp_col4", SciDAVis::Numeric));
			temp_col[4] = shared_ptr<ColumnWrapper>(new ColumnWrapper("temp_col5", SciDAVis::Text));
			temp_col[5] = shared_ptr<ColumnWrapper>(new ColumnWrapper("temp_col6", SciDAVis::DateTime));
			for(int i=0; i<15; i++)
				temp_col[3]->setValueAt(i,i*11.0);
			list.clear();
			for(int i=3; i<6; i++)
				list << temp_col[i];

			table_model->removeColumns(8, 3);
			CPPUNIT_ASSERT_EQUAL(8, table_model->columnCount());
			CPPUNIT_ASSERT_EQUAL(10, table_model->rowCount());
			table_model->appendColumns(list);
			CPPUNIT_ASSERT_EQUAL(11, table_model->columnCount());
			CPPUNIT_ASSERT_EQUAL(15, table_model->rowCount());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Numeric, table_model->output(8)->columnMode());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Text, table_model->output(9)->columnMode());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::DateTime, table_model->output(10)->columnMode());

			
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION( TableTest );

