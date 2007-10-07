#include <cppunit/extensions/HelperMacros.h>

#include "Column.h"
#include "TableModel.h"
#include <QtGlobal>
#include <QtDebug>
#include <QApplication>
#include <QMainWindow>
#include <QTableView>

#define EPSILON (1e-6)

class TableTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(TableTest);
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

			column[0] = new Column("col0", SciDAVis::Numeric);
			column[1] = new Column("col1", double_temp, temp_validity);
			column[2] = new Column("col2", SciDAVis::Text);
			column[3] = new Column("col3", strl_temp, temp_validity);
			column[4] = new Column("col4", SciDAVis::DateTime);
			column[5] = new Column("col5", dtl_temp, temp_validity);
			column[6] = new Column("col6", SciDAVis::Month);
			column[7] = new Column("col7", SciDAVis::Month);
			column[8] = new Column("col8", SciDAVis::Day);
			column[9] = new Column("col9", SciDAVis::Day);
			column[10] = new Column("col10", double_temp);
		}
		
		void tearDown() 
		{
			for(int i=0; i<=9; i++) delete column[i];
		}

	private:
		Column * column[11];

/* ------------------------------------------------------------------------------ */
		void testTableModel() 
		{
			int argc=0;
			char ** argv=0;
			QApplication app(argc,argv);
			QMainWindow mw;
			TableModel * table_model = new TableModel();
			QTableView * table_view = new QTableView();
			table_view->setModel(table_model);

			QList< Column *> list;

			// expand column 0 to 10 rows
			for(int i=0; i<10; i++)
				column[0]->setValueAt(i,i*11.0);

			// insert all columns into the model
			for(int i=0; i<11; i++)
				list << column[i];
			table_model->insertColumns(0,list);
			CPPUNIT_ASSERT_EQUAL(11, table_model->columnCount());
			CPPUNIT_ASSERT_EQUAL(10, table_model->rowCount());
			
			mw.setCentralWidget(table_view);

			mw.show();
			app.exec();
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION( TableTest );

