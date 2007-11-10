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
#include <QDockWidget>
#include <QMdiSubWindow>

#include "test_wrappers.h"

#define EPSILON (1e-6)

class TableTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(TableTest);
		CPPUNIT_TEST(testTableModel);
		CPPUNIT_TEST(testTableGUI);
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp() 
		{
			int argc = 1;
			char *argv[1] = { "./table-test" };
			app = new QApplication(argc,argv);
			mw = new QMainWindow();

			QVector<double> double_temp;
			for(int i=0; i<10; i++)
				double_temp << i*11.0;
			QStringList strl_temp;
			for(int i=0; i<7; i++)
				strl_temp << QString::number(i*11);

			strl_temp << "foo" << "bar" << "qt4";
			QList<QDateTime> dtl_temp;
			dtl_temp << QDateTime(QDate(2000,1,1),QTime(1,2,3,4));
			dtl_temp << QDateTime(QDate(2000,2,2),QTime(1,2,3,4));
			dtl_temp << QDateTime(QDate(2000,3,3),QTime(1,2,3,4));
			IntervalAttribute<bool> temp_validity;
			temp_validity.setValue(Interval<int>(1,2));

			column[0] = shared_ptr<Column>(new Column("col0", double_temp));
			column[1] = shared_ptr<Column>(new Column("col1", double_temp, temp_validity));
			column[2] = shared_ptr<Column>(new Column("col2", strl_temp));
			column[3] = shared_ptr<Column>(new Column("col3", strl_temp, temp_validity));
			column[4] = shared_ptr<Column>(new Column("col4", dtl_temp));
			column[5] = shared_ptr<Column>(new Column("col5", dtl_temp, temp_validity));
			column[6] = shared_ptr<Column>(new Column("col6", SciDAVis::Month));
			column[7] = shared_ptr<Column>(new Column("col7", SciDAVis::Month));
			column[8] = shared_ptr<Column>(new Column("col8", SciDAVis::Day));
			column[9] = shared_ptr<Column>(new Column("col9", SciDAVis::Day));
			column[10] = shared_ptr<Column>(new Column("col10", double_temp));
			column[1]->setMasked(Interval<int>(3,5));
			
			for(int i=0; i<11; i++)
			{
				if(!(i%4))
					column[i]->setPlotDesignation(SciDAVis::X);
				else
					column[i]->setPlotDesignation(SciDAVis::Y);
				column[i]->setComment(QString("comment %1").arg(i));
			}

			table = shared_ptr<Table>(new Table(0, 5, 2, "table1"));
			prj = shared_ptr<Project>(new Project());
			prj->setName("project");
			prj->addChild(table);

			column[4]->copy(column[2]);
			dtl_temp << QDateTime(QDate(2000,1,1),QTime(1,2,3,4));
			dtl_temp << QDateTime(QDate(2000,2,2),QTime(1,2,3,4));
			dtl_temp << QDateTime(QDate(2000,3,3),QTime(1,2,3,4));
			column[6]->replaceDateTimes(0, dtl_temp);

			QList<  shared_ptr<Column> > list;
			// insert all columns into the model
			for(int i=0; i<11; i++)
				list << column[i];
			table->insertColumns(0,list);
		}
		
		void tearDown() 
		{
			delete app;
			delete mw;
		}

	private:
		shared_ptr<Column> column[11];
		shared_ptr<Project> prj;
		shared_ptr<Table> table;
		QApplication * app;
		QMainWindow * mw;

/* ------------------------------------------------------------------------------ */
		void testTableModel() 
		{
			TableModel * table_model = static_cast<TableModel *>(static_cast<TableView *>(table->view())->model());

			QList<  shared_ptr<Column> > list;

			CPPUNIT_ASSERT_EQUAL(13, table_model->columnCount());
			table_model->removeColumns(11,2);
			CPPUNIT_ASSERT_EQUAL(11, table_model->columnCount());
			CPPUNIT_ASSERT_EQUAL(10, table_model->rowCount());

			shared_ptr<Column> temp_col[6];
			temp_col[0] = shared_ptr<Column>(new Column("temp_col0", SciDAVis::Numeric));
			temp_col[1] = shared_ptr<Column>(new Column("temp_col1", SciDAVis::Text));
			temp_col[2] = shared_ptr<Column>(new Column("temp_col2", SciDAVis::DateTime));
			list.clear();
			for(int i=0; i<3; i++)
				list << temp_col[i];

			table_model->replaceColumns(8, list);
			CPPUNIT_ASSERT_EQUAL(11, table_model->columnCount());
			CPPUNIT_ASSERT_EQUAL(10, table_model->rowCount());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Numeric, table_model->output(8)->columnMode());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Text, table_model->output(9)->columnMode());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::DateTime, table_model->output(10)->columnMode());

			temp_col[3] = shared_ptr<Column>(new Column("temp_col4", SciDAVis::Numeric));
			temp_col[4] = shared_ptr<Column>(new Column("temp_col5", SciDAVis::Text));
			temp_col[5] = shared_ptr<Column>(new Column("temp_col6", SciDAVis::DateTime));
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
		/* ----------------------------------------------------------- */
		void testTableGUI() 
		{
			QMdiArea * mdiArea = new QMdiArea(mw);
			QMdiSubWindow * table_window = mdiArea->addSubWindow(table->view());

			mw->setCentralWidget(mdiArea);
			QDockWidget * undo_dock = new QDockWidget("History", mw);
			QUndoView * undo_view = new QUndoView(prj->undoStack());
			undo_dock->setWidget(undo_view);
			mw->addDockWidget(Qt::RightDockWidgetArea, undo_dock);

			mw->show();
			table_window->showMaximized();
			mw->showMaximized();

			table->showComments(true);
			table->showComments(false);
			table->showComments(true);
			table->setRowCount(11);
			CPPUNIT_ASSERT_EQUAL(11, table->rowCount());
			table->setRowCount(12);
			CPPUNIT_ASSERT_EQUAL(12, table->rowCount());
			table->setRowCount(10);
			CPPUNIT_ASSERT_EQUAL(10, table->rowCount());
			column[0]->insertRows(2,2);
			CPPUNIT_ASSERT_EQUAL(12, table->rowCount());
			table->removeRows(0,2);
			table->removeRows(5,5);
			table->insertRows(5,3);
			table->removeColumns(6,table->columnCount()-1-6);
			table->showComments(true);
			table->column(1)->setColumnLabel("column one");
			table->column(0)->setColumnComment("this is column zero\nnew line");
			CPPUNIT_ASSERT_EQUAL(QString("column one"), table->column(1)->columnLabel());
			CPPUNIT_ASSERT_EQUAL(QString("this is column zero\nnew line"), table->column(0)->columnComment());
			CPPUNIT_ASSERT_EQUAL(2, table->columnCount(SciDAVis::X));
			CPPUNIT_ASSERT_EQUAL(4, table->columnCount(SciDAVis::Y));
			CPPUNIT_ASSERT_EQUAL(0, table->columnCount(SciDAVis::Z));
			int temp = table->columnCount();
			table->setColumnCount(temp-1);
			CPPUNIT_ASSERT_EQUAL(temp-1, table->columnCount());
			table->setColumnCount(temp+1);
  			CPPUNIT_ASSERT_EQUAL(SciDAVis::Numeric, table->column(temp-1)->columnMode());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Numeric, table->column(temp)->columnMode());
			table->clear();
			table->clearMasks();

			app->exec();
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION( TableTest );

