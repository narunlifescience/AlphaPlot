#include <cppunit/extensions/HelperMacros.h>
#include "assertion_traits.h"

#include "Column.h"
#include "TableModel.h"
#include "TableView.h"
#include "Table.h"
#include "Project.h"
#include <QtGlobal>
#include <QtDebug>
#include <QApplication>
#include <QMainWindow>
#include <QMdiArea>
#include <QUndoView>

#include "test_wrappers.h"

#define EPSILON (1e-6)

class TableTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(TableTest);
//		CPPUNIT_TEST(testTableModelGUI);
		CPPUNIT_TEST(testTableGUI);
		CPPUNIT_TEST(testTableModel);
		CPPUNIT_TEST(testTableModelConnections);
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

			column[0] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("col0", SciDAVis::Numeric));
			column[1] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("col1", double_temp, temp_validity));
			column[2] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("col2", SciDAVis::Text));
			column[3] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("col3", strl_temp, temp_validity));
			column[4] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("col4", SciDAVis::DateTime));
			column[5] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("col5", dtl_temp, temp_validity));
			column[6] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("col6", SciDAVis::Month));
			column[7] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("col7", SciDAVis::Month));
			column[8] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("col8", SciDAVis::Day));
			column[9] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("col9", SciDAVis::Day));
			column[10] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("col10", double_temp));
			column[1]->setMasked(Interval<int>(3,5));
		}
		
		void tearDown() 
		{
		}

	private:
		shared_ptr<ColumnTestWrapper> column[11];

/* ------------------------------------------------------------------------------ */
		void testTableModelGUI() 
		{
			int argc=0;
			char ** argv=0;
			QApplication app(argc,argv);
			QMainWindow mw;
			TableModel * table_model = new TableModel();
			TableViewTestWrapper * table_view = new TableViewTestWrapper(column, table_model);

			QList<  shared_ptr<Column> > list;

			// expand column 0 and 2 to 10 rows
			for(int i=0; i<10; i++)
				column[0]->setValueAt(i,i*11.0);
			for(int i=0; i<10; i++)
				column[2]->setTextAt(i,QString::number(i*11));

			// insert all columns into the model
			for(int i=0; i<11; i++)
				list << column[i];
			table_model->insertColumns(0,list);

			mw.setCentralWidget(table_view);

			mw.show();
			mw.resize(800,600);
			app.exec();
		}
		/* ----------------------------------------------------------- */
		void testTableGUI() 
		{
			int argc=0;
			char ** argv=0;
			QApplication app(argc,argv);
			QMainWindow mw;
			shared_ptr<Table> table(new Table(0, 5, 2, "table1"));
			Project prj;
			prj.addChild(table);

			QList<  shared_ptr<Column> > list;

			// expand column 0 and 2 to 10 rows
			for(int i=0; i<10; i++)
				column[0]->setValueAt(i,i*11.0);
			for(int i=0; i<10; i++)
				column[2]->setTextAt(i,QString::number(i*11));

			// insert all columns into the model
			for(int i=0; i<11; i++)
				list << column[i];
			table->insertColumns(0,list);

			QMdiArea mdiArea;
			mdiArea.addSubWindow(table->view());
			mdiArea.addSubWindow(new QUndoView(prj.undoStack()));	

			mw.setCentralWidget(&mdiArea);

			mw.show();
			mw.resize(800,600);
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

			shared_ptr<ColumnTestWrapper> temp_col[6];
			temp_col[0] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("temp_col0", SciDAVis::Numeric));
			temp_col[1] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("temp_col1", SciDAVis::Text));
			temp_col[2] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("temp_col2", SciDAVis::DateTime));
			list.clear();
			for(int i=0; i<3; i++)
				list << temp_col[i];

			table_model->replaceColumns(8, list);
			CPPUNIT_ASSERT_EQUAL(11, table_model->columnCount());
			CPPUNIT_ASSERT_EQUAL(10, table_model->rowCount());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Numeric, table_model->output(8)->columnMode());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Text, table_model->output(9)->columnMode());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::DateTime, table_model->output(10)->columnMode());

			temp_col[3] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("temp_col4", SciDAVis::Numeric));
			temp_col[4] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("temp_col5", SciDAVis::Text));
			temp_col[5] = shared_ptr<ColumnTestWrapper>(new ColumnTestWrapper("temp_col6", SciDAVis::DateTime));
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

			table_model->appendRows(2);
			CPPUNIT_ASSERT_EQUAL(17, table_model->rowCount());
			table_model->removeRows(6,10);
			CPPUNIT_ASSERT_EQUAL(7, table_model->rowCount());
			table_model->insertRows(1,2);
			int max_rows = 0;
			for(int i=0; i<table_model->columnCount(); i++)
				if(column[i]->rowCount() >  max_rows) max_rows = column[i]->rowCount();
			CPPUNIT_ASSERT( max_rows <= table_model->rowCount() );
			
		}
		/* ----------------------------------------------------------- */
		void testTableModelConnections() 
		{
			
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION( TableTest );

