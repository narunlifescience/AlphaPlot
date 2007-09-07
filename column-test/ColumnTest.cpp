#include <cppunit/extensions/HelperMacros.h>
#include "assertion_traits.h"

#include "Column.h"
#include "Interval.h"
#include <QtGlobal>

#define EPSILON (1e-6)

class ColumnTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ColumnTest);
		CPPUNIT_TEST(testIntervalAttributes);
		CPPUNIT_TEST(testDoubleColumn);
		CPPUNIT_TEST(testStringColumn);
		CPPUNIT_TEST(testDateTimeColumn);
		CPPUNIT_TEST(testConversion);
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
		void testIntervalAttributes() 
		{
			column[0]->setFormula(Interval<int>(0,20), "foo bar");
			CPPUNIT_ASSERT_EQUAL(column[0]->formula(10), QString("foo bar"));
			column[0]->setFormula(Interval<int>(5,15), "bar baz");
			CPPUNIT_ASSERT_EQUAL(column[0]->formula(10),  QString("bar baz"));
			CPPUNIT_ASSERT_EQUAL(column[0]->formula(5),  QString("bar baz"));
			CPPUNIT_ASSERT_EQUAL(column[0]->formula(15),  QString("bar baz"));
			CPPUNIT_ASSERT_EQUAL(column[0]->formula(0),  QString("foo bar"));
			CPPUNIT_ASSERT_EQUAL(column[0]->formula(4),  QString("foo bar"));
			CPPUNIT_ASSERT_EQUAL(column[0]->formula(16),  QString("foo bar"));
			CPPUNIT_ASSERT_EQUAL(column[0]->formula(20),  QString("foo bar"));

			CPPUNIT_ASSERT(!column[0]->isInvalid(10));
			CPPUNIT_ASSERT(!column[0]->isInvalid(Interval<int>(0,10)));
			column[0]->setInvalid(Interval<int>(10,19), true);
			CPPUNIT_ASSERT(column[0]->isInvalid(10));
			column[0]->removeRows(10,5);
			CPPUNIT_ASSERT(!column[0]->isInvalid(19));
			column[0]->insertEmptyRows(5,5);
			CPPUNIT_ASSERT(!column[0]->isInvalid(5));
			CPPUNIT_ASSERT(column[0]->isInvalid(19));
			column[0]->setInvalid(Interval<int>(10,20), false);
			CPPUNIT_ASSERT(!column[0]->isInvalid(Interval<int>(12,13)));
			column[0]->setInvalid(Interval<int>(12,13), true);
			CPPUNIT_ASSERT(column[0]->isInvalid(Interval<int>(12,13)));

			// test validity from ctor
			for(int i=0; i<3; i++)
			{
				CPPUNIT_ASSERT(column[2*i+1]->isInvalid(Interval<int>(1,1)));
				CPPUNIT_ASSERT(column[2*i+1]->isInvalid(Interval<int>(2,2)));
			}
			CPPUNIT_ASSERT(!column[10]->isInvalid(Interval<int>(1,1)));
			CPPUNIT_ASSERT(!column[10]->isInvalid(Interval<int>(2,2)));
		}
/* ------------------------------------------------------------------------------ */
		void testDoubleColumn() 
		{
			CPPUNIT_ASSERT_EQUAL(column[0]->columnLabel(), QString("col0"));	
			CPPUNIT_ASSERT_EQUAL(column[0]->columnMode(), SciDAVis::Numeric);	
			CPPUNIT_ASSERT_EQUAL(column[0]->dataType(), SciDAVis::TypeDouble);	
			CPPUNIT_ASSERT_EQUAL(column[1]->columnLabel(), QString("col1"));	
			CPPUNIT_ASSERT_EQUAL(column[1]->columnMode(), SciDAVis::Numeric);	
			CPPUNIT_ASSERT_EQUAL(column[1]->dataType(), SciDAVis::TypeDouble);	

			// test 3 param ctor
			for(int i=1; i<=3; i++)
				CPPUNIT_ASSERT_DOUBLES_EQUAL(1.1*(double)i, column[1]->valueAt(i-1), EPSILON);

			// test full copy
			column[0]->copy(column[1]);
			CPPUNIT_ASSERT_EQUAL(column[0]->rowCount(), column[1]->rowCount());
			for(int i=0; i<column[0]->rowCount(); i++)
				CPPUNIT_ASSERT_DOUBLES_EQUAL(column[1]->valueAt(i) , column[0]->valueAt(i), EPSILON);

			int temp = column[0]->rowCount();
			for(int i=0; i<temp+2; i++)
				column[0]->setValueAt(i, 12.75);
			CPPUNIT_ASSERT_EQUAL(column[0]->rowCount() - 2, column[1]->rowCount());
			for(int i=0; i<column[0]->rowCount(); i++)
				CPPUNIT_ASSERT(column[1]->valueAt(i) != column[0]->valueAt(i)); 

			// test partial copy
			column[1]->copy(column[0], 0, 1, 2);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(1.1 , column[1]->valueAt(0), EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(column[0]->valueAt(0) , column[1]->valueAt(1), EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(column[0]->valueAt(1) , column[1]->valueAt(2), EPSILON);

			column[1]->copy(column[0], column[1]->rowCount()-1, 1, 2);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(12.75 , column[1]->valueAt(column[1]->rowCount()-1), EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(12.75 , column[1]->valueAt(column[1]->rowCount()-2), EPSILON);

			// test insert/delete
			column[0]->insertEmptyRows(0, 5);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, column[0]->valueAt(0), EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, column[0]->valueAt(4), EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(12.75, column[0]->valueAt(5), EPSILON);
			temp = column[0]->rowCount();
			column[0]->insertEmptyRows(temp, 1); // append 1 row
			column[0]->insertEmptyRows(temp + 3, 2); // should to nothing to the number of rows 
			column[0]->insertEmptyRows(temp + 99, 2); // should to nothing to the number of rows 
			CPPUNIT_ASSERT_EQUAL(temp + 1, column[0]->rowCount());
			temp = column[0]->rowCount();
			column[0]->removeRows(temp, 10); // should do nothing
			CPPUNIT_ASSERT_EQUAL(temp, column[0]->rowCount());
			column[0]->removeRows(temp-2, 10); // should remove 2 rows
			CPPUNIT_ASSERT_EQUAL(temp-2, column[0]->rowCount());
			column[0]->removeRows(0, 5);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(12.75, column[0]->valueAt(0), EPSILON);


		}
/* ------------------------------------------------------------------------------ */
		void testStringColumn() 
		{
			CPPUNIT_ASSERT_EQUAL(column[2]->columnLabel(), QString("col2"));	
			CPPUNIT_ASSERT_EQUAL(column[2]->columnMode(), SciDAVis::Text);	
			CPPUNIT_ASSERT_EQUAL(column[2]->dataType(), SciDAVis::TypeQString);	
			CPPUNIT_ASSERT_EQUAL(column[3]->columnLabel(), QString("col3"));	
			CPPUNIT_ASSERT_EQUAL(column[3]->columnMode(), SciDAVis::Text);	
			CPPUNIT_ASSERT_EQUAL(column[3]->dataType(), SciDAVis::TypeQString);	

			// test 3 param ctor
			CPPUNIT_ASSERT_EQUAL(column[3]->textAt(0), QString("foo"));
			CPPUNIT_ASSERT_EQUAL(column[3]->textAt(1), QString("bar"));
			CPPUNIT_ASSERT_EQUAL(column[3]->textAt(2), QString("qt4"));

			// test full copy
			column[2]->copy(column[3]);
			CPPUNIT_ASSERT_EQUAL(column[2]->rowCount(), column[3]->rowCount());
			for(int i=0; i<column[2]->rowCount(); i++)
				CPPUNIT_ASSERT_EQUAL(column[2]->textAt(i), column[3]->textAt(i) );

			int temp = column[2]->rowCount();
			for(int i=0; i<temp+2; i++)
				column[2]->setTextAt(i, "bla");
			CPPUNIT_ASSERT_EQUAL(column[2]->rowCount() - 2, column[3]->rowCount());
			for(int i=0; i<column[2]->rowCount(); i++)
				CPPUNIT_ASSERT(column[2]->textAt(i) != column[3]->textAt(i));

			// test partial copy
			column[3]->copy(column[2], 0, 1, 2);
			CPPUNIT_ASSERT_EQUAL(column[3]->textAt(0), QString("foo"));
			CPPUNIT_ASSERT_EQUAL(column[3]->textAt(1), QString("bla"));
			CPPUNIT_ASSERT_EQUAL(column[3]->textAt(2), QString("bla"));

			column[3]->copy(column[2], column[3]->rowCount()-1, 1, 2);
			CPPUNIT_ASSERT_EQUAL(column[3]->textAt(column[3]->rowCount()-1), QString("bla"));
			CPPUNIT_ASSERT_EQUAL(column[3]->textAt(column[3]->rowCount()-2), QString("bla"));

		}
/* ------------------------------------------------------------------------------ */
		void testDateTimeColumn() 
		{
			CPPUNIT_ASSERT_EQUAL(column[4]->columnLabel(), QString("col4"));	
			CPPUNIT_ASSERT_EQUAL(column[5]->columnLabel(), QString("col5"));	
			CPPUNIT_ASSERT_EQUAL(column[6]->columnLabel(), QString("col6"));	
			CPPUNIT_ASSERT_EQUAL(column[7]->columnLabel(), QString("col7"));	
			CPPUNIT_ASSERT_EQUAL(column[8]->columnLabel(), QString("col8"));	
			CPPUNIT_ASSERT_EQUAL(column[9]->columnLabel(), QString("col9"));	

			CPPUNIT_ASSERT_EQUAL(column[4]->columnMode(), SciDAVis::DateTime);	
			CPPUNIT_ASSERT_EQUAL(column[5]->columnMode(), SciDAVis::DateTime);	
			CPPUNIT_ASSERT_EQUAL(column[6]->columnMode(), SciDAVis::Month);	
			CPPUNIT_ASSERT_EQUAL(column[7]->columnMode(), SciDAVis::Month);	
			CPPUNIT_ASSERT_EQUAL(column[8]->columnMode(), SciDAVis::Day);	
			CPPUNIT_ASSERT_EQUAL(column[9]->columnMode(), SciDAVis::Day);	
			for(int i=4; i<=9; i++)
				CPPUNIT_ASSERT_EQUAL(column[i]->dataType(), SciDAVis::TypeQDateTime);	

			// test 3 param ctor
			CPPUNIT_ASSERT_EQUAL(column[5]->dateTimeAt(0), QDateTime(QDate(2000,1,1),QTime(1,2,3,4)));
			CPPUNIT_ASSERT_EQUAL(column[5]->dateTimeAt(1), QDateTime(QDate(2000,2,2),QTime(1,2,3,4)));
			CPPUNIT_ASSERT_EQUAL(column[5]->dateTimeAt(2), QDateTime(QDate(2000,3,3),QTime(1,2,3,4)));

			// test full copy
			column[4]->copy(column[5]);
			CPPUNIT_ASSERT_EQUAL(column[4]->rowCount(), column[5]->rowCount());
			for(int i=0; i<column[4]->rowCount(); i++)
				CPPUNIT_ASSERT_EQUAL(column[4]->dateTimeAt(i), column[5]->dateTimeAt(i) );

			int temp = column[4]->rowCount();
			for(int i=0; i<temp+2; i++)
				column[4]->setDateTimeAt(i, QDateTime(QDate(1955,5,5),QTime(10,10,10,10)));
			CPPUNIT_ASSERT_EQUAL(column[4]->rowCount() - 2, column[5]->rowCount());
			for(int i=0; i<column[4]->rowCount(); i++)
				CPPUNIT_ASSERT(column[4]->dateTimeAt(i) != column[5]->dateTimeAt(i));

			// test partial copy
			column[5]->copy(column[4], 0, 1, 2);
			CPPUNIT_ASSERT_EQUAL(column[5]->dateTimeAt(0), QDateTime(QDate(2000,1,1),QTime(1,2,3,4)));
			CPPUNIT_ASSERT_EQUAL(column[5]->dateTimeAt(1), QDateTime(QDate(1955,5,5),QTime(10,10,10,10)));
			CPPUNIT_ASSERT_EQUAL(column[5]->dateTimeAt(2), QDateTime(QDate(1955,5,5),QTime(10,10,10,10)));

			column[5]->copy(column[4], column[5]->rowCount()-1, 1, 2);
			CPPUNIT_ASSERT_EQUAL(column[5]->dateTimeAt(column[5]->rowCount()-1), QDateTime(QDate(1955,5,5),QTime(10,10,10,10)));
			CPPUNIT_ASSERT_EQUAL(column[5]->dateTimeAt(column[5]->rowCount()-2), QDateTime(QDate(1955,5,5),QTime(10,10,10,10)));
		}
/* ------------------------------------------------------------------------------ */
		void testConversion() 
		{
			// test setColumnMode
			column[0]->setColumnMode(SciDAVis::Text);
			CPPUNIT_ASSERT_EQUAL(column[0]->columnMode(), SciDAVis::Text);
			CPPUNIT_ASSERT_EQUAL(column[0]->dataType(), SciDAVis::TypeQString);
			column[1]->setColumnMode(SciDAVis::DateTime);
			CPPUNIT_ASSERT_EQUAL(column[1]->columnMode(), SciDAVis::DateTime);
			CPPUNIT_ASSERT_EQUAL(column[1]->dataType(), SciDAVis::TypeQDateTime);
			column[2]->setColumnMode(SciDAVis::Month);
			CPPUNIT_ASSERT_EQUAL(column[2]->columnMode(), SciDAVis::Month);
			CPPUNIT_ASSERT_EQUAL(column[2]->dataType(), SciDAVis::TypeQDateTime);
			column[3]->setColumnMode(SciDAVis::Day);
			CPPUNIT_ASSERT_EQUAL(column[3]->columnMode(), SciDAVis::Day);
			CPPUNIT_ASSERT_EQUAL(column[3]->dataType(), SciDAVis::TypeQDateTime);
			column[4]->setColumnMode(SciDAVis::Numeric);
			CPPUNIT_ASSERT_EQUAL(column[4]->columnMode(), SciDAVis::Numeric);
			CPPUNIT_ASSERT_EQUAL(column[4]->dataType(), SciDAVis::TypeDouble);
		}
/* ------------------------------------------------------------------------------ */
};

CPPUNIT_TEST_SUITE_REGISTRATION( ColumnTest );

