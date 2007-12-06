#include <cppunit/extensions/HelperMacros.h>
#include "assertion_traits.h"

#include "Column.h"
#include "Interval.h"
#include "SimpleMappingFilter.h"
#include "Double2StringFilter.h"
#include <QtGlobal>
#include <QLocale>
#include <QtDebug>
#include <QUndoStack>

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
	
		bool equals(AbstractColumn * other)
		{
			if(plotDesignation() != other->plotDesignation()) return false;
			if(rowCount() != other->rowCount()) return false;
			if(dataType() != other->dataType()) return false;
			if(columnMode() != other->columnMode()) return false;
			for(int i=0; i<rowCount(); i++)
			{
				if((valueAt(i) - other->valueAt(i)) > EPSILON) return false;
				if(textAt(i) != other->textAt(i)) return false;
				if(dateTimeAt(i) != other->dateTimeAt(i)) return false;
				if(isInvalid(i) != other->isInvalid(i)) return false;
				if(isMasked(i) != other->isMasked(i)) return false;
				if(formula(i) != other->formula(i)) return false;
			}

			return true;
		}
		bool equalsDebug(AbstractColumn * other)
		{
			if(plotDesignation() != other->plotDesignation())
			{
				qDebug() << "plot designation differs";
				return false;
			}
			if(rowCount() != other->rowCount())
			{
				qDebug() << "row count differs";
				return false;
			}
			if(dataType() != other->dataType())
			{
				qDebug() << "data type differs";
				return false;
			}
			if(columnMode() != other->columnMode()) 
			{
				qDebug() << "column mode differs";
				return false;
			}
			for(int i=0; i<rowCount(); i++)
			{
				if((valueAt(i) - other->valueAt(i)) > EPSILON)
				{
					qDebug() << QString("double value differs in row %1").arg(i);
					return false;
				}
				if(textAt(i) != other->textAt(i))
				{
					qDebug() << QString("text differs in row %1").arg(i);
					return false;
				}
				if(dateTimeAt(i) != other->dateTimeAt(i))
				{
					qDebug() << QString("date-time differs in row %1").arg(i);
					return false;
				}
				if(isInvalid(i) != other->isInvalid(i))
				{
					qDebug() << QString("validity differs in row %1").arg(i);
					return false;
				}
				if(isMasked(i) != other->isMasked(i))
				{
					qDebug() << QString("masking differs in row %1").arg(i);
					return false;
				}
				if(formula(i) != other->formula(i))
				{
					qDebug() << QString("formula differs in row %1").arg(i);
					return false;
				}
			}

			return true;
		}

};

class ColumnTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ColumnTest);
		CPPUNIT_TEST(testGeneralMethods);
		CPPUNIT_TEST(testIntervalAttributes);
		CPPUNIT_TEST(testDoubleColumn);
		CPPUNIT_TEST(testStringColumn);
		CPPUNIT_TEST(testDateTimeColumn);
		CPPUNIT_TEST(testConversion);
		CPPUNIT_TEST(testMappingFilter);
		CPPUNIT_TEST(testUndo);
		CPPUNIT_TEST(testSave);
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
		void testGeneralMethods() 
		{
			// test plot designation
			column[0]->setPlotDesignation(SciDAVis::Z);
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Z, column[0]->plotDesignation());

			// test clear
			for(int i=0; i<3; i++)
				column[2*i+1]->clear();
			for(int i=0; i<3; i++)
				CPPUNIT_ASSERT_EQUAL(0, column[2*i+1]->rowCount());


			// test label and comment
			column[0]->setColumnLabel("foo bar!");
			CPPUNIT_ASSERT_EQUAL(QString("foo bar!"), column[0]->columnLabel());
			column[0]->setColumnComment("comment!");
			CPPUNIT_ASSERT_EQUAL(QString("comment!"), column[0]->columnComment());


		}
/* ------------------------------------------------------------------------------ */
		void testIntervalAttributes() 
		{
			// test formulas
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

			// test validity
			CPPUNIT_ASSERT(!column[0]->isInvalid(10));
			CPPUNIT_ASSERT(!column[0]->isInvalid(Interval<int>(0,10)));
			column[0]->setInvalid(Interval<int>(10,19), true);
			CPPUNIT_ASSERT(column[0]->isInvalid(10));
			column[0]->removeRows(10,5);
			CPPUNIT_ASSERT(!column[0]->isInvalid(19));
			column[0]->insertRows(5,5);
			CPPUNIT_ASSERT(!column[0]->isInvalid(5));
			CPPUNIT_ASSERT(column[0]->isInvalid(19));
			column[0]->setInvalid(Interval<int>(10,20), false);
			CPPUNIT_ASSERT(!column[0]->isInvalid(Interval<int>(12,13)));
			column[0]->setInvalid(Interval<int>(12,13), true);
			CPPUNIT_ASSERT(column[0]->isInvalid(Interval<int>(12,13)));
			column[0]->setInvalid(Interval<int>(0,10));
			column[0]->clearValidity();
			CPPUNIT_ASSERT(!column[0]->isInvalid(0));

			// test masking
			CPPUNIT_ASSERT(!column[0]->isMasked(10));
			CPPUNIT_ASSERT(!column[0]->isMasked(Interval<int>(0,10)));
			column[0]->setMasked(Interval<int>(10,19), true);
			CPPUNIT_ASSERT(column[0]->isMasked(10));
			column[0]->removeRows(10,5);
			CPPUNIT_ASSERT(!column[0]->isMasked(19));
			column[0]->insertRows(5,5);
			CPPUNIT_ASSERT(!column[0]->isMasked(5));
			CPPUNIT_ASSERT(column[0]->isMasked(19));
			column[0]->setMasked(Interval<int>(10,20), false);
			CPPUNIT_ASSERT(!column[0]->isMasked(Interval<int>(12,13)));
			column[0]->setMasked(Interval<int>(12,13), true);
			CPPUNIT_ASSERT(column[0]->isMasked(Interval<int>(12,13)));
			column[0]->setMasked(Interval<int>(0,10));
			column[0]->clearMasks();
			CPPUNIT_ASSERT(!column[0]->isMasked(0));

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
			column[0]->copy(column[1].get());
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
			column[1]->copy(column[0].get(), 0, 1, 2);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(1.1 , column[1]->valueAt(0), EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(column[0]->valueAt(0) , column[1]->valueAt(1), EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(column[0]->valueAt(1) , column[1]->valueAt(2), EPSILON);

			column[1]->copy(column[0], column[1]->rowCount()-1, 1, 2);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(12.75 , column[1]->valueAt(column[1]->rowCount()-1), EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(12.75 , column[1]->valueAt(column[1]->rowCount()-2), EPSILON);

			// test insert/delete
			column[0]->insertRows(0, 5);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, column[0]->valueAt(0), EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, column[0]->valueAt(4), EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(12.75, column[0]->valueAt(5), EPSILON);
			temp = column[0]->rowCount();
			column[0]->insertRows(temp, 1); // append 1 row
			column[0]->insertRows(temp + 3, 2); // should to nothing to the number of rows 
			column[0]->insertRows(temp + 99, 2); // should to nothing to the number of rows 
			CPPUNIT_ASSERT_EQUAL(temp + 1, column[0]->rowCount());
			temp = column[0]->rowCount();
			column[0]->removeRows(temp, 10); // should do nothing
			CPPUNIT_ASSERT_EQUAL(temp, column[0]->rowCount());
			column[0]->removeRows(temp-2, 10); // should remove 2 rows
			CPPUNIT_ASSERT_EQUAL(temp-2, column[0]->rowCount());
			column[0]->removeRows(0, 5);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(12.75, column[0]->valueAt(0), EPSILON);
			
			// test replace function
			QVector<double> temp2;
			temp2 << 0.11 << 0.22 << 0.33;
			column[0]->replaceValues(20, temp2);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.11, column[0]->valueAt(20), EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.22, column[0]->valueAt(21), EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.33, column[0]->valueAt(22), EPSILON);



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

			// test insert/delete
			column[2]->insertRows(0, 5);
			CPPUNIT_ASSERT_EQUAL(QString(""), column[2]->textAt(0));
			CPPUNIT_ASSERT_EQUAL(QString(""), column[2]->textAt(4));
			CPPUNIT_ASSERT_EQUAL(QString("bla"), column[2]->textAt(5));
			temp = column[2]->rowCount();
			column[2]->insertRows(temp, 1); // append 1 row
			column[2]->insertRows(temp + 3, 2); // should to nothing to the number of rows 
			column[2]->insertRows(temp + 99, 2); // should to nothing to the number of rows 
			CPPUNIT_ASSERT_EQUAL(temp + 1, column[2]->rowCount());
			temp = column[2]->rowCount();
			column[2]->removeRows(temp, 10); // should do nothing
			CPPUNIT_ASSERT_EQUAL(temp, column[2]->rowCount());
			column[2]->removeRows(temp-2, 10); // should remove 2 rows
			CPPUNIT_ASSERT_EQUAL(temp-2, column[2]->rowCount());
			column[2]->removeRows(0, 5);
			CPPUNIT_ASSERT_EQUAL(QString("bla"), column[2]->textAt(0));

			// test replace function
			QStringList temp2;
			temp2 << "one" << "two" << "three";
			column[2]->replaceTexts(20, temp2);
			CPPUNIT_ASSERT_EQUAL(QString("one"), column[2]->textAt(20));
			CPPUNIT_ASSERT_EQUAL(QString("two"), column[2]->textAt(21));
			CPPUNIT_ASSERT_EQUAL(QString("three"), column[2]->textAt(22));
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

			// test insert/delete
			column[4]->insertRows(0, 5);
			CPPUNIT_ASSERT_EQUAL(QDateTime(), column[4]->dateTimeAt(0));
			CPPUNIT_ASSERT_EQUAL(QDateTime(), column[4]->dateTimeAt(4));
			CPPUNIT_ASSERT_EQUAL(QDateTime(QDate(1955,5,5),QTime(10,10,10,10)), column[4]->dateTimeAt(5));
			temp = column[4]->rowCount();
			column[4]->insertRows(temp, 1); // append 1 row
			column[4]->insertRows(temp + 3, 2); // should to nothing to the number of rows 
			column[4]->insertRows(temp + 99, 2); // should to nothing to the number of rows 
			CPPUNIT_ASSERT_EQUAL(temp + 1, column[4]->rowCount());
			temp = column[4]->rowCount();
			column[4]->removeRows(temp, 10); // should do nothing
			CPPUNIT_ASSERT_EQUAL(temp, column[4]->rowCount());
			column[4]->removeRows(temp-2, 10); // should remove 2 rows
			CPPUNIT_ASSERT_EQUAL(temp-2, column[4]->rowCount());
			column[4]->removeRows(0, 5);
			CPPUNIT_ASSERT_EQUAL(QDateTime(QDate(1955,5,5),QTime(10,10,10,10)), column[4]->dateTimeAt(0));

			// test replace function
			QList<QDateTime> temp2;
			for(temp = 20; temp <= 25; temp++)
				temp2 << QDateTime(QDate(2000,1,temp),QTime(10,11,12,13));
			column[4]->replaceDateTimes(20, temp2);
			for(temp = 20; temp <= 25; temp++)
				CPPUNIT_ASSERT_EQUAL(QDateTime(QDate(2000,1,temp),QTime(10,11,12,13)), column[4]->dateTimeAt(temp));

		}
/* ------------------------------------------------------------------------------ */
		void testConversion() 
		{
			double dbl_temp;
			QVector<double> dbls;
			QDateTime date_temp;
			QList<QDateTime> dts;
			QString str_temp;
			QStringList strs;

			// test setColumnMode, inputFilter, outputFilter
			// 1) double -> string
			dbls.clear();
			dbls << 1.231 << 0.99 << -15.1 << -10.8;
			foreach(dbl_temp, dbls)
			{
				column[0]->setColumnMode(SciDAVis::Numeric);
				column[0]->setValueAt(0, dbl_temp);
				dynamic_cast<Double2StringFilter*>(column[0]->outputFilter().get())->setNumericFormat('f');
				dynamic_cast<Double2StringFilter*>(column[0]->outputFilter().get())->setNumDigits(2);
				column[0]->setColumnMode(SciDAVis::Text);
				CPPUNIT_ASSERT_EQUAL(QLocale().toString(dbl_temp, 'f', 2), column[0]->textAt(0));
				CPPUNIT_ASSERT_EQUAL(column[0]->columnMode(), SciDAVis::Text);
				CPPUNIT_ASSERT_EQUAL(column[0]->dataType(), SciDAVis::TypeQString);
				column[0]->setTextAt(0, QString("foo bar"));
				CPPUNIT_ASSERT_EQUAL(column[0]->textAt(0), QString("foo bar"));
				column[0]->inputFilter()->input(0, column[2]);
				CPPUNIT_ASSERT_EQUAL(column[0]->dataType(), column[0]->inputFilter()->output(0)->dataType());
				column[0]->outputFilter()->input(0, column[0]);
				CPPUNIT_ASSERT_EQUAL(SciDAVis::TypeQString, column[0]->outputFilter()->output(0)->dataType());
			}

			// 2) double -> date-time
			dts.clear();
			dts << QDateTime(QDate(1888,7,6), QTime(12,0,0,100));
			dts << QDateTime(QDate(888,12,12), QTime(1,0,0,200));
			dts << QDateTime(QDate(2111,11,11),QTime(11,11,11,11));
			foreach(date_temp, dts)
			{
				dbl_temp = double(date_temp.date().toJulianDay()) +
					double( -date_temp.time().msecsTo(QTime(12,0,0,0)) ) / 86400000.0;
				column[1]->setColumnMode(SciDAVis::Numeric);
				column[1]->setValueAt(2, dbl_temp);
				column[1]->setColumnMode(SciDAVis::DateTime);
				CPPUNIT_ASSERT_EQUAL(column[1]->columnMode(), SciDAVis::DateTime);
				CPPUNIT_ASSERT_EQUAL(column[1]->dataType(), SciDAVis::TypeQDateTime);
				column[1]->inputFilter()->input(0, column[2]);
				CPPUNIT_ASSERT_EQUAL(column[1]->dataType(), column[1]->inputFilter()->output(0)->dataType());
				column[1]->outputFilter()->input(0, column[1]);
				CPPUNIT_ASSERT_EQUAL(SciDAVis::TypeQString, column[1]->outputFilter()->output(0)->dataType());
				CPPUNIT_ASSERT_EQUAL(date_temp, column[1]->dateTimeAt(2));
			}

			// 3) string -> month
			strs.clear();
			strs << QLocale().monthName(12);
			strs << QLocale().monthName(1);
			strs << QLocale().monthName(6);
			foreach(str_temp, strs)
			{
				column[2]->setColumnMode(SciDAVis::Text);
				column[2]->setTextAt(5, str_temp);
				column[2]->setColumnMode(SciDAVis::Month);
				CPPUNIT_ASSERT_EQUAL(column[2]->columnMode(), SciDAVis::Month);
				CPPUNIT_ASSERT_EQUAL(column[2]->dataType(), SciDAVis::TypeQDateTime);
				column[2]->inputFilter()->input(0, column[2]);
				CPPUNIT_ASSERT_EQUAL(column[2]->dataType(), column[2]->inputFilter()->output(0)->dataType());
				column[2]->outputFilter()->input(0, column[2]);
				CPPUNIT_ASSERT_EQUAL(SciDAVis::TypeQString, column[2]->outputFilter()->output(0)->dataType());
				CPPUNIT_ASSERT_EQUAL(QDateTime::fromString(str_temp, "MMMM"), column[2]->dateTimeAt(5));
			}

			// 4) string -> day
			strs.clear();
			strs << QLocale().dayName(5);
			strs << QLocale().dayName(1);
			strs << QLocale().dayName(7);
			foreach(str_temp, strs)
			{
				column[3]->setColumnMode(SciDAVis::Text);
				column[3]->setTextAt(4, str_temp);
				column[3]->setColumnMode(SciDAVis::Day);
				CPPUNIT_ASSERT_EQUAL(column[3]->columnMode(), SciDAVis::Day);
				CPPUNIT_ASSERT_EQUAL(column[3]->dataType(), SciDAVis::TypeQDateTime);
				column[3]->inputFilter()->input(0, column[2]);
				CPPUNIT_ASSERT_EQUAL(column[3]->dataType(), column[3]->inputFilter()->output(0)->dataType());
				column[3]->outputFilter()->input(0, column[3]);
				CPPUNIT_ASSERT_EQUAL(SciDAVis::TypeQString, column[3]->outputFilter()->output(0)->dataType());
				CPPUNIT_ASSERT_EQUAL(QDateTime::fromString(str_temp, "dddd"), column[3]->dateTimeAt(4));
			}

			// 5) day -> string
			strs.clear();
			strs << QLocale().dayName(1);
			strs << QLocale().dayName(2);
			strs << QLocale().dayName(7);
			foreach(str_temp, strs)
			{
				column[3]->setColumnMode(SciDAVis::Day);
				column[3]->setDateTimeAt(0, QDateTime::fromString(str_temp, "dddd"));
				column[3]->setColumnMode(SciDAVis::Text);
				CPPUNIT_ASSERT_EQUAL(str_temp, column[3]->textAt(0));
			}

			// 6) month -> string
			strs.clear();
			strs << QLocale().monthName(1);
			strs << QLocale().monthName(11);
			strs << QLocale().monthName(12);
			foreach(str_temp, strs)
			{
				column[2]->setColumnMode(SciDAVis::Month);
				column[2]->setDateTimeAt(0, QDateTime::fromString(str_temp, "MMMM"));
				column[2]->setColumnMode(SciDAVis::Text);
				CPPUNIT_ASSERT_EQUAL(str_temp, column[2]->textAt(0));
			}

			// 7) date-time -> double
			dts.clear();
			dts << QDateTime(QDate(1888,7,6), QTime(12,0,0,100));
			dts << QDateTime(QDate(888,12,12), QTime(1,0,0,200));
			dts << QDateTime(QDate(2111,11,11),QTime(11,11,11,11));
			foreach(date_temp, dts)
			{
				dbl_temp = double(date_temp.date().toJulianDay()) +
					double( -date_temp.time().msecsTo(QTime(12,0,0,0)) ) / 86400000.0;
				column[4]->setColumnMode(SciDAVis::DateTime);
				column[4]->setDateTimeAt(1, date_temp);
				column[4]->setColumnMode(SciDAVis::Numeric);
				CPPUNIT_ASSERT_EQUAL(column[4]->columnMode(), SciDAVis::Numeric);
				CPPUNIT_ASSERT_EQUAL(column[4]->dataType(), SciDAVis::TypeDouble);
				column[4]->inputFilter()->input(0, column[2]);
				CPPUNIT_ASSERT_EQUAL(column[4]->dataType(), column[4]->inputFilter()->output(0)->dataType());
				column[4]->outputFilter()->input(0, column[4]);
				CPPUNIT_ASSERT_EQUAL(SciDAVis::TypeQString, column[4]->outputFilter()->output(0)->dataType());
				column[4]->setValueAt(0, 11.11);
				CPPUNIT_ASSERT_DOUBLES_EQUAL(column[4]->valueAt(0), 11.11, EPSILON);
				CPPUNIT_ASSERT_DOUBLES_EQUAL(dbl_temp, column[4]->valueAt(1), EPSILON);
			}

			// 8) day -> double
			dbls.clear();
			dbls << 0.8 << 1.1 << 1.9 << 4.1 << 5.7 << 14.3 << 12.5 << 99.7;
			foreach(dbl_temp, dbls)
			{
				column[8]->setColumnMode(SciDAVis::Day);
				str_temp = QLocale().dayName((qRound(dbl_temp)-1)%7+1);
				column[8]->setDateTimeAt(0, QDateTime::fromString(str_temp, "dddd"));
				column[8]->setColumnMode(SciDAVis::Numeric);
				CPPUNIT_ASSERT_DOUBLES_EQUAL(double((qRound(dbl_temp)-1)%7+1), column[8]->valueAt(0), EPSILON);
			}
			
			// 9) month -> double
			dbls.clear();
			dbls << 0.8 << 1.1 << 1.9 << 4.1 << 5.7 << 14.3 << 12.5 << 99.7;
			foreach(dbl_temp, dbls)
			{
				str_temp = QLocale().monthName((qRound(dbl_temp)-1)%12+1);
				column[6]->setColumnMode(SciDAVis::Month);
				column[6]->setDateTimeAt(0, QDateTime::fromString(str_temp, "MMMM"));
				column[6]->setColumnMode(SciDAVis::Numeric);
				CPPUNIT_ASSERT_DOUBLES_EQUAL(double((qRound(dbl_temp)-1)%12+1), column[6]->valueAt(0), EPSILON);
			}

			// 10) double -> day
			dbls.clear();
			dbls << 0.8 << 1.1 << 1.9 << 4.1 << 5.7 << 31.3 << 33.5 << 40.7 << -5.0;
			foreach(dbl_temp, dbls)
			{
				column[10]->setColumnMode(SciDAVis::Numeric);
				column[10]->setValueAt(0, dbl_temp);
				column[10]->setColumnMode(SciDAVis::Day);
				CPPUNIT_ASSERT_EQUAL(QDateTime(QDate(1900,1,1).addDays(qRound(dbl_temp - 1.0)), QTime()), column[10]->dateTimeAt(0));
				int temp = (qRound(dbl_temp)-1)%7+1;
				while(temp < 0) temp += 7;
				CPPUNIT_ASSERT_EQUAL(temp, column[10]->dateTimeAt(0).date().dayOfWeek());
			}

			// 11) double -> month
			dbls.clear();
			dbls << 0.8 << 1.1 << 4.9 << 6.1 << 12.4 << 15.3 << 31.3 << 33.5 << 40.7 << -5.0;
			foreach(dbl_temp, dbls)
			{
				column[10]->setColumnMode(SciDAVis::Numeric);
				column[10]->setValueAt(0, dbl_temp);
				column[10]->setColumnMode(SciDAVis::Month);
				CPPUNIT_ASSERT_EQUAL(QDateTime(QDate(1900,1,1).addMonths(qRound(dbl_temp - 1.0)),QTime(0,0,0,0)), column[10]->dateTimeAt(0));
				int temp = (qRound(dbl_temp)-1)%12+1;
				while(temp < 0) temp += 12;
					CPPUNIT_ASSERT_EQUAL(temp, column[10]->dateTimeAt(0).date().month());
			}

			// 12) string -> double
			dbls.clear();
			dbls << 7.7829 << -12.331233 << 100.123123 << -47.123123 << 0.0;
			foreach(dbl_temp, dbls)
			{
				column[0]->setColumnMode(SciDAVis::Text);
				column[0]->setTextAt(99, QLocale().toString(dbl_temp, 'f', 3));
				column[0]->setColumnMode(SciDAVis::Numeric);
				CPPUNIT_ASSERT_DOUBLES_EQUAL(double(qRound(dbl_temp*1000.0))/1000.0, column[0]->valueAt(99), EPSILON);
			}

			// 13) text -> month -> date-time -> month -> text
			for(int int_temp = 1; int_temp <= 12; int_temp++)
			{
				str_temp = QLocale().monthName(int_temp);
				column[10]->setColumnMode(SciDAVis::Text);
				column[10]->setTextAt(0, str_temp);
				column[10]->setColumnMode(SciDAVis::Month);
				CPPUNIT_ASSERT_EQUAL(QDateTime::fromString(str_temp, "MMMM"), column[10]->dateTimeAt(0));
				column[10]->setColumnMode(SciDAVis::DateTime);
				CPPUNIT_ASSERT_EQUAL(QDateTime(QDate(1900,int_temp,1),QTime(0,0,0,0)), column[10]->dateTimeAt(0));
				column[10]->setColumnMode(SciDAVis::Month);
				CPPUNIT_ASSERT_EQUAL(QDateTime::fromString(str_temp, "MMMM"), column[10]->dateTimeAt(0));
				column[10]->setColumnMode(SciDAVis::Text);
				CPPUNIT_ASSERT_EQUAL(QLocale().monthName(int_temp), column[10]->textAt(0));
			}

			// 13) double -> day -> date-time -> day -> double
			dbls.clear();
			dbls << 7.7829 << 12.331233 << 100.123123 << 47.123123 << 1.0;
			foreach(dbl_temp, dbls)
			{
				column[10]->setColumnMode(SciDAVis::Numeric);
				column[10]->setValueAt(0, dbl_temp);
				column[10]->setColumnMode(SciDAVis::Day);
				CPPUNIT_ASSERT_EQUAL(QDateTime::fromString(QLocale().dayName((qRound(dbl_temp)-1)%7+1), "dddd").date().dayOfWeek(), 
						column[10]->dateTimeAt(0).date().dayOfWeek());
				column[10]->setColumnMode(SciDAVis::DateTime);
				CPPUNIT_ASSERT_EQUAL(QDateTime(QDate(1900,1,1).addDays(qRound(dbl_temp - 1.0)),QTime(0,0,0,0)), column[10]->dateTimeAt(0));
				column[10]->setColumnMode(SciDAVis::Day);
				CPPUNIT_ASSERT_EQUAL(QDateTime::fromString(QLocale().dayName((qRound(dbl_temp)-1)%7+1), "dddd").date().dayOfWeek(), 
						column[10]->dateTimeAt(0).date().dayOfWeek());
				column[10]->setColumnMode(SciDAVis::Numeric);
				CPPUNIT_ASSERT_DOUBLES_EQUAL(double((qRound(dbl_temp)-1)%7+1), column[10]->valueAt(0), EPSILON);
			}

				// apply all conversions twice
				column[0]->setColumnMode(SciDAVis::Numeric);
				column[0]->setColumnMode(SciDAVis::Numeric);
				column[0]->setColumnMode(SciDAVis::Text);
				column[0]->setColumnMode(SciDAVis::Text);
				column[0]->setColumnMode(SciDAVis::DateTime);
				column[0]->setColumnMode(SciDAVis::DateTime);
				column[0]->setColumnMode(SciDAVis::Month);
				column[0]->setColumnMode(SciDAVis::Month);
				column[0]->setColumnMode(SciDAVis::Day);
				column[0]->setColumnMode(SciDAVis::Day);
				column[0]->setColumnMode(SciDAVis::Numeric);
		}
/* ------------------------------------------------------------------------------ */
		void testUndo()
		{
			// initialization
			column[0]->setValueAt(0, 100.1);
			column[0]->setValueAt(1, 200.2);
			column[1]->setPlotDesignation(SciDAVis::X);
			column[1]->setMasked(2);
			column[2]->setTextAt(0,"foo foo");
			column[2]->setTextAt(1,"bar bar");
			column[3]->setPlotDesignation(SciDAVis::Y);
			column[3]->setMasked(2);
			column[4]->setDateTimeAt(0, QDateTime());
			column[4]->setDateTimeAt(1, QDateTime());
			column[5]->setPlotDesignation(SciDAVis::Z);
			column[5]->setMasked(2);
			column[1]->setFormula(Interval<int>(0,20), "foo bar");
			column[1]->setFormula(Interval<int>(5,15), "bar baz");
			column[3]->setFormula(Interval<int>(0,20), "foo bar bla");
			column[3]->setFormula(Interval<int>(5,15), "bar baz bla");
			column[5]->setFormula(Interval<int>(0,20), "foo bar foo");
			column[5]->setFormula(Interval<int>(5,15), "bar baz foo");


			// test set mode
			column[1]->setColumnMode(SciDAVis::Text);
			undoTestInternal();

			column[3]->setColumnMode(SciDAVis::DateTime);
			undoTestInternal();

			column[5]->setColumnMode(SciDAVis::Numeric);
			undoTestInternal();

			// test full copy
			column[1]->copy(column[0]);
			undoTestInternal();
			
			column[3]->copy(column[2]);
			undoTestInternal();

			column[5]->copy(column[4]);
			undoTestInternal();

			// test partial copy
			column[1]->copy(column[0], 1, 0, 1);
			undoTestInternal();
			
			column[3]->copy(column[2], 1, 0, 1);
			undoTestInternal();

			column[5]->copy(column[4], 1, 0, 1);
			undoTestInternal();

			// test insert rows
			column[1]->insertRows(0, 100);
			undoTestInternal();
			
			column[3]->insertRows(0, 100);
			undoTestInternal();

			column[5]->insertRows(0, 100);
			undoTestInternal();

			// test remove rows
			column[1]->removeRows(1, 100);
			undoTestInternal();
			
			column[3]->removeRows(1, 100);
			undoTestInternal();

			column[5]->removeRows(1, 100);
			undoTestInternal();

			// test plot designation
			column[0]->setPlotDesignation(SciDAVis::xErr);
			undoTestInternal();

			// test clear
			column[1]->clear();
			undoTestInternal();
			
			column[3]->clear();
			undoTestInternal();

			column[5]->clear();
			undoTestInternal();

			// test clear validity
			column[1]->clearValidity();
			undoTestInternal();
			
			column[3]->clearValidity();
			undoTestInternal();

			column[5]->clearValidity();
			undoTestInternal();

			// test clear masking
			column[1]->clearMasks();
			undoTestInternal();
			
			column[3]->clearMasks();
			undoTestInternal();

			column[5]->clearMasks();
			undoTestInternal();

			// test set invalid
			column[1]->setInvalid(Interval<int>(0,20), true);
			undoTestInternal();

			// test set masked
			column[1]->setMasked(Interval<int>(0,20), true);
			undoTestInternal();

			// test set formula
			column[1]->setFormula(Interval<int>(0,20), QString("cos(exp(x))"));
			undoTestInternal();

			// test clear formulas
			column[1]->clearFormulas();
			undoTestInternal();

			// test set value 
			column[1]->setValueAt(200, 1.5);
			undoTestInternal();

			// test replace values
			QVector<double> vec;
			vec << 123.13 << 123.123 << 0.123123;
			column[1]->replaceValues(2, vec);
			undoTestInternal();

			// test set text 
			column[3]->setTextAt(200, QString("foo foo"));
			undoTestInternal();

			// test replace text 
			QStringList list;
			list << "foo 1" << "bar 2" << "baz 3";
			column[3]->replaceTexts(2, list);
			undoTestInternal();

			// test set date-time
			column[5]->setDateTimeAt(200, QDateTime(QDate(1919,9,9),QTime(1,1,1,1)));
			undoTestInternal();
						
			// test replace date-time
			QList<QDateTime> dtlist;
			dtlist << QDateTime(QDate(1919,9,9),QTime(1,1,1,1));
			dtlist << QDateTime(QDate(1919,9,9),QTime(1,1,1,1));
			dtlist << QDateTime(QDate(1919,9,9),QTime(1,1,1,1));
			column[5]->replaceDateTimes(2, dtlist);
			undoTestInternal();

		}

		void undoTestInternal()
		{
			QUndoStack * us = column[0]->undoStack();

			us->redo();
			us->undo();
			undoTestInternal2();
			us->redo();
			us->undo();
			us->clear();
			undoTestInternal2();
		}

		void undoTestInternal2()
		{
			// check column 1
			CPPUNIT_ASSERT_EQUAL(3, column[1]->rowCount());
			CPPUNIT_ASSERT_EQUAL(column[1]->columnMode(), SciDAVis::Numeric);	
			CPPUNIT_ASSERT_EQUAL(column[1]->dataType(), SciDAVis::TypeDouble);	
			for(int i=1; i<=3; i++)
				CPPUNIT_ASSERT_DOUBLES_EQUAL(1.1*(double)i, column[1]->valueAt(i-1), EPSILON);
			CPPUNIT_ASSERT_EQUAL(SciDAVis::X, column[1]->plotDesignation());
			CPPUNIT_ASSERT(!column[1]->isInvalid(0));
			CPPUNIT_ASSERT(column[1]->isInvalid(1));
			CPPUNIT_ASSERT(column[1]->isMasked(2));
			CPPUNIT_ASSERT_EQUAL(column[1]->formula(10),  QString("bar baz"));
			CPPUNIT_ASSERT_EQUAL(column[1]->formula(5),  QString("bar baz"));
			CPPUNIT_ASSERT_EQUAL(column[1]->formula(15),  QString("bar baz"));
			CPPUNIT_ASSERT_EQUAL(column[1]->formula(0),  QString("foo bar"));
			CPPUNIT_ASSERT_EQUAL(column[1]->formula(4),  QString("foo bar"));
			CPPUNIT_ASSERT_EQUAL(column[1]->formula(16),  QString("foo bar"));
			CPPUNIT_ASSERT_EQUAL(column[1]->formula(20),  QString("foo bar"));
			
			// check column 3
			CPPUNIT_ASSERT_EQUAL(3, column[3]->rowCount());
			CPPUNIT_ASSERT_EQUAL(column[3]->columnMode(), SciDAVis::Text);	
			CPPUNIT_ASSERT_EQUAL(column[3]->dataType(), SciDAVis::TypeQString);	
			CPPUNIT_ASSERT_EQUAL(column[3]->textAt(0), QString("foo"));
			CPPUNIT_ASSERT_EQUAL(column[3]->textAt(1), QString("bar"));
			CPPUNIT_ASSERT_EQUAL(column[3]->textAt(2), QString("qt4"));
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Y, column[3]->plotDesignation());
			CPPUNIT_ASSERT(!column[3]->isInvalid(0));
			CPPUNIT_ASSERT(column[3]->isInvalid(1));
			CPPUNIT_ASSERT(column[3]->isMasked(2));
			CPPUNIT_ASSERT_EQUAL(column[3]->formula(10),  QString("bar baz bla"));
			CPPUNIT_ASSERT_EQUAL(column[3]->formula(5),  QString("bar baz bla"));
			CPPUNIT_ASSERT_EQUAL(column[3]->formula(15),  QString("bar baz bla"));
			CPPUNIT_ASSERT_EQUAL(column[3]->formula(0),  QString("foo bar bla"));
			CPPUNIT_ASSERT_EQUAL(column[3]->formula(4),  QString("foo bar bla"));
			CPPUNIT_ASSERT_EQUAL(column[3]->formula(16),  QString("foo bar bla"));
			CPPUNIT_ASSERT_EQUAL(column[3]->formula(20),  QString("foo bar bla"));

			// check column 5
			CPPUNIT_ASSERT_EQUAL(3, column[5]->rowCount());
			CPPUNIT_ASSERT_EQUAL(column[5]->columnMode(), SciDAVis::DateTime);	
			CPPUNIT_ASSERT_EQUAL(column[5]->dataType(), SciDAVis::TypeQDateTime);	
			CPPUNIT_ASSERT_EQUAL(column[5]->dateTimeAt(0), QDateTime(QDate(2000,1,1),QTime(1,2,3,4)));
			CPPUNIT_ASSERT_EQUAL(column[5]->dateTimeAt(1), QDateTime(QDate(2000,2,2),QTime(1,2,3,4)));
			CPPUNIT_ASSERT_EQUAL(column[5]->dateTimeAt(2), QDateTime(QDate(2000,3,3),QTime(1,2,3,4)));
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Z, column[5]->plotDesignation());
			CPPUNIT_ASSERT(!column[5]->isInvalid(0));
			CPPUNIT_ASSERT(column[5]->isInvalid(1));
			CPPUNIT_ASSERT(column[5]->isMasked(2));
			CPPUNIT_ASSERT_EQUAL(column[5]->formula(10),  QString("bar baz foo"));
			CPPUNIT_ASSERT_EQUAL(column[5]->formula(5),  QString("bar baz foo"));
			CPPUNIT_ASSERT_EQUAL(column[5]->formula(15),  QString("bar baz foo"));
			CPPUNIT_ASSERT_EQUAL(column[5]->formula(0),  QString("foo bar foo"));
			CPPUNIT_ASSERT_EQUAL(column[5]->formula(4),  QString("foo bar foo"));
			CPPUNIT_ASSERT_EQUAL(column[5]->formula(16),  QString("foo bar foo"));
			CPPUNIT_ASSERT_EQUAL(column[5]->formula(20),  QString("foo bar foo"));	
		}
/* ------------------------------------------------------------------------------ */
		void testSave() 
		{
			shared_ptr<ColumnWrapper> temp_col = shared_ptr<ColumnWrapper>(new ColumnWrapper("temp_col", SciDAVis::Numeric));

			QString output;
			QXmlStreamWriter * writer;
			QXmlStreamReader * reader;

			for(int i=0; i<11; i++)
			{
				writer = new QXmlStreamWriter(&output);
				column[i]->save(writer);
				// qDebug() << "now reading column " << column[i]->columnLabel();
				// qDebug() << output;
				reader = new QXmlStreamReader(output);
				reader->readNext();
				CPPUNIT_ASSERT(reader->isStartDocument());
				reader->readNext();
				if(!temp_col->load(reader))
				{
                          qDebug() << QString("Parse error in column %1 at line %2, column %3:\n%4")
							  .arg(column[i]->columnLabel())
                              .arg(reader->lineNumber())
                              .arg(reader->columnNumber())
                              .arg(reader->errorString());
				}
				CPPUNIT_ASSERT(column[i]->equals(temp_col.get()));
				delete writer;
				delete reader;
				output.clear();
			}
				
		}
/* ------------------------------------------------------------------------------ */
		void testMappingFilter()
		{
			shared_ptr<ColumnWrapper> col1 = shared_ptr<ColumnWrapper>(new ColumnWrapper("col1", SciDAVis::Text));
			shared_ptr<SimpleMappingFilter> col2 = shared_ptr<SimpleMappingFilter>(new SimpleMappingFilter());
			shared_ptr<ColumnWrapper> col3 = shared_ptr<ColumnWrapper>(new ColumnWrapper("col3", SciDAVis::Numeric));

			col2->input(0, dynamic_pointer_cast<AbstractColumn>(col1));

			CPPUNIT_ASSERT_EQUAL(col1->dataType(), col2->dataType());
			CPPUNIT_ASSERT_EQUAL(col1->columnMode(), col2->columnMode());
			CPPUNIT_ASSERT(col2->isReadOnly() == false);

			col2->setColumnMode(SciDAVis::Numeric);
			CPPUNIT_ASSERT_EQUAL(col1->dataType(), col2->dataType());
			CPPUNIT_ASSERT_EQUAL(col1->columnMode(), col2->columnMode());
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Numeric, col2->columnMode());

			for(int i=0; i<10; i++)
				col3->setValueAt(i, (double)i);
			
			for(int i=0; i<10; i++)
				col2->addMapping(i, i);

			col2->copy(col3.get());
			CPPUNIT_ASSERT_EQUAL(col1->rowCount(), col2->rowCount());
			CPPUNIT_ASSERT(col1->equalsDebug(col3.get()));
			CPPUNIT_ASSERT(col1->equalsDebug(col2.get()));

			col2->clearMappings();
			col2->addMapping(1,0);
			col2->addMapping(3,1);
			col2->addMapping(5,2);
			col2->addMapping(6,4);
			
			CPPUNIT_ASSERT_EQUAL(5, col2->rowCount());
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col2->valueAt(3),EPSILON);
			
			col2->copy(col3.get(), 2, 1, 3);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col1->valueAt(0),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, col1->valueAt(1),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, col1->valueAt(2),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, col1->valueAt(3),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, col1->valueAt(4),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, col1->valueAt(5),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, col1->valueAt(6),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(7.0, col1->valueAt(7),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, col1->valueAt(8),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0, col1->valueAt(9),EPSILON);
			
			col1->copy(col3);
			col2->insertRows(4, 2);
			CPPUNIT_ASSERT_EQUAL(7, col2->rowCount());
			CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, col2->valueAt(0),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, col2->valueAt(1),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, col2->valueAt(2),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col2->valueAt(3),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col2->valueAt(4),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col2->valueAt(5),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, col2->valueAt(6),EPSILON);

			CPPUNIT_ASSERT_EQUAL(12, col1->rowCount());
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col1->valueAt(0),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, col1->valueAt(1),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, col1->valueAt(2),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, col1->valueAt(3),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, col1->valueAt(4),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, col1->valueAt(5),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col1->valueAt(6),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col1->valueAt(7),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, col1->valueAt(8),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(7.0, col1->valueAt(9),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, col1->valueAt(10),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0, col1->valueAt(11),EPSILON);

			col2->removeRows(1, 2);
			CPPUNIT_ASSERT_EQUAL(5, col2->rowCount());
			CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, col2->valueAt(0),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col2->valueAt(1),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col2->valueAt(2),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col2->valueAt(3),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, col2->valueAt(4),EPSILON);

			CPPUNIT_ASSERT_EQUAL(10, col1->rowCount());
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col1->valueAt(0),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, col1->valueAt(1),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, col1->valueAt(2),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, col1->valueAt(3),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col1->valueAt(4),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col1->valueAt(5),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, col1->valueAt(6),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(7.0, col1->valueAt(7),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, col1->valueAt(8),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0, col1->valueAt(9),EPSILON);

			CPPUNIT_ASSERT_EQUAL(col1->columnLabel(), col2->columnLabel());
			col2->setColumnLabel("foo");
			CPPUNIT_ASSERT_EQUAL(QString("foo"), col1->columnLabel());
			col2->setColumnComment("bar");
			CPPUNIT_ASSERT_EQUAL(QString("bar"), col1->columnComment());
			col2->setPlotDesignation(SciDAVis::Z);
			CPPUNIT_ASSERT_EQUAL(SciDAVis::Z, col1->plotDesignation());

			col1->copy(col3);
			col2->clearMappings();
			col2->addMapping(1,0);
			col2->addMapping(3,1);
			col2->addMapping(5,2);
			col2->addMapping(6,4);

			col2->insertRows(5, 2);  // insert 2 rows after row 4
			CPPUNIT_ASSERT_EQUAL(7, col2->rowCount());
			CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, col2->valueAt(0),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, col2->valueAt(1),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, col2->valueAt(2),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col2->valueAt(3),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, col2->valueAt(4),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col2->valueAt(5),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col2->valueAt(6),EPSILON);

			CPPUNIT_ASSERT_EQUAL(12, col1->rowCount());
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col1->valueAt(0),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(1.0, col1->valueAt(1),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, col1->valueAt(2),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(3.0, col1->valueAt(3),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, col1->valueAt(4),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(5.0, col1->valueAt(5),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(6.0, col1->valueAt(6),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col1->valueAt(7),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col1->valueAt(8),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(7.0, col1->valueAt(9),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, col1->valueAt(10),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0, col1->valueAt(11),EPSILON);

			col1->copy(col3);
			col2->clearMappings();
			col2->addMapping(1,0);
			col2->addMapping(3,1);
			col2->addMapping(5,2);
			col2->addMapping(6,4);

			col2->clear();
			CPPUNIT_ASSERT_EQUAL(6, col1->rowCount());
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col1->valueAt(0),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(2.0, col1->valueAt(1),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(4.0, col1->valueAt(2),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(7.0, col1->valueAt(3),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(8.0, col1->valueAt(4),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(9.0, col1->valueAt(5),EPSILON);

			col1->copy(col3);
			col2->clearMappings();
			col2->addMapping(1,0);
			col2->addMapping(3,1);
			col2->addMapping(5,2);
			col2->addMapping(6,4);
			
			col2->setInvalid(Interval<int>(1,2));
			CPPUNIT_ASSERT(!col1->isInvalid(0));
			CPPUNIT_ASSERT(!col1->isInvalid(1));
			CPPUNIT_ASSERT(!col1->isInvalid(2));
			CPPUNIT_ASSERT(col1->isInvalid(3));
			CPPUNIT_ASSERT(!col1->isInvalid(4));
			CPPUNIT_ASSERT(col1->isInvalid(5));
			CPPUNIT_ASSERT(!col1->isInvalid(6));
			CPPUNIT_ASSERT(!col1->isInvalid(7));
			CPPUNIT_ASSERT(!col1->isInvalid(8));
			CPPUNIT_ASSERT(!col1->isInvalid(9));

			QList< Interval<int> > ivs = col2->invalidIntervals();
			CPPUNIT_ASSERT_EQUAL(1, ivs.count());
			CPPUNIT_ASSERT_EQUAL(Interval<int>(1,2), ivs.at(0));
			
			col1->copy(col3);
			col2->clearMappings();
			col2->addMapping(1,0);
			col2->addMapping(3,1);
			col2->addMapping(5,2);
			col2->addMapping(6,4);
			
			col2->setMasked(Interval<int>(1,4));
			CPPUNIT_ASSERT(!col1->isMasked(0));
			CPPUNIT_ASSERT(!col1->isMasked(1));
			CPPUNIT_ASSERT(!col1->isMasked(2));
			CPPUNIT_ASSERT(col1->isMasked(3));
			CPPUNIT_ASSERT(!col1->isMasked(4));
			CPPUNIT_ASSERT(col1->isMasked(5));
			CPPUNIT_ASSERT(col1->isMasked(6));
			CPPUNIT_ASSERT(!col1->isMasked(7));
			CPPUNIT_ASSERT(!col1->isMasked(8));
			CPPUNIT_ASSERT(!col1->isMasked(9));

			ivs = col2->maskedIntervals();
			CPPUNIT_ASSERT_EQUAL(2, ivs.count());
			Interval<int> i1 = ivs.at(0);
			Interval<int> i2 = ivs.at(1);
			if(i1.start() > i2.start())
			{
				i1 = ivs.at(1);
				i2 = ivs.at(0);
			}
			CPPUNIT_ASSERT_EQUAL(Interval<int>(1,2), i1);
			CPPUNIT_ASSERT_EQUAL(Interval<int>(4,4), i2);
			
			
			col1->copy(col3);
			col2->clearMappings();
			col2->addMapping(1,0);
			col2->addMapping(3,1);
			col2->addMapping(5,2);
			col2->addMapping(6,4);

			col1->setInvalid(Interval<int>(0,9));
			col1->setMasked(Interval<int>(0,9));
			col2->clearValidity();
			col2->clearMasks();

			CPPUNIT_ASSERT(col1->isMasked(0));
			CPPUNIT_ASSERT(!col1->isMasked(1));
			CPPUNIT_ASSERT(col1->isMasked(2));
			CPPUNIT_ASSERT(!col1->isMasked(3));
			CPPUNIT_ASSERT(col1->isMasked(4));
			CPPUNIT_ASSERT(!col1->isMasked(5));
			CPPUNIT_ASSERT(!col1->isMasked(6));
			CPPUNIT_ASSERT(col1->isMasked(7));
			CPPUNIT_ASSERT(col1->isMasked(8));
			CPPUNIT_ASSERT(col1->isMasked(9));

			CPPUNIT_ASSERT(col1->isInvalid(0));
			CPPUNIT_ASSERT(!col1->isInvalid(1));
			CPPUNIT_ASSERT(col1->isInvalid(2));
			CPPUNIT_ASSERT(!col1->isInvalid(3));
			CPPUNIT_ASSERT(col1->isInvalid(4));
			CPPUNIT_ASSERT(!col1->isInvalid(5));
			CPPUNIT_ASSERT(!col1->isInvalid(6));
			CPPUNIT_ASSERT(col1->isInvalid(7));
			CPPUNIT_ASSERT(col1->isInvalid(8));
			CPPUNIT_ASSERT(col1->isInvalid(9));

			col1->clearFormulas();
			col2->setFormula(Interval<int>(0,4), "foo");

			CPPUNIT_ASSERT_EQUAL(QString(""), col1->formula(0));
			CPPUNIT_ASSERT_EQUAL(QString("foo"), col1->formula(1));
			CPPUNIT_ASSERT_EQUAL(QString(""), col1->formula(2));
			CPPUNIT_ASSERT_EQUAL(QString("foo"), col1->formula(3));
			CPPUNIT_ASSERT_EQUAL(QString(""), col1->formula(4));
			CPPUNIT_ASSERT_EQUAL(QString("foo"), col1->formula(5));
			CPPUNIT_ASSERT_EQUAL(QString("foo"), col1->formula(6));
			CPPUNIT_ASSERT_EQUAL(QString(""), col1->formula(7));
			CPPUNIT_ASSERT_EQUAL(QString(""), col1->formula(8));
			CPPUNIT_ASSERT_EQUAL(QString(""), col1->formula(9));

			ivs = col2->formulaIntervals();
			CPPUNIT_ASSERT_EQUAL(2, ivs.count());
			ivs = col1->formulaIntervals();
			CPPUNIT_ASSERT_EQUAL(3, ivs.count());

			col1->setFormula(Interval<int>(0,9), "bar");
			col2->clearFormulas();

			CPPUNIT_ASSERT_EQUAL(QString("bar"), col1->formula(0));
			CPPUNIT_ASSERT_EQUAL(QString(""), col1->formula(1));
			CPPUNIT_ASSERT_EQUAL(QString("bar"), col1->formula(2));
			CPPUNIT_ASSERT_EQUAL(QString(""), col1->formula(3));
			CPPUNIT_ASSERT_EQUAL(QString("bar"), col1->formula(4));
			CPPUNIT_ASSERT_EQUAL(QString(""), col1->formula(5));
			CPPUNIT_ASSERT_EQUAL(QString(""), col1->formula(6));
			CPPUNIT_ASSERT_EQUAL(QString("bar"), col1->formula(7));
			CPPUNIT_ASSERT_EQUAL(QString("bar"), col1->formula(8));
			CPPUNIT_ASSERT_EQUAL(QString("bar"), col1->formula(9));
			
			for(int i=0; i<col2->rowCount(); i++)
				col2->setValueAt(i, (double)i+200.14342);

			CPPUNIT_ASSERT_DOUBLES_EQUAL(col1->valueAt(1), col2->valueAt(0),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(col1->valueAt(3), col2->valueAt(1),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(col1->valueAt(5), col2->valueAt(2),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(col1->valueAt(6), col2->valueAt(4),EPSILON);

			QVector<double> values;
			values << 15.5 << -16.7 << 37.22 << 22.2 << 100.22;

			col2->replaceValues(0, values);

			CPPUNIT_ASSERT_DOUBLES_EQUAL(15.5, col1->valueAt(1),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(-16.7, col1->valueAt(3),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(37.22, col1->valueAt(5),EPSILON);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(100.22, col1->valueAt(6),EPSILON);
			
			col1->copy(col3);
			col2->clearMappings();
			col2->addMapping(1,0);
			col2->addMapping(3,1);
			col2->addMapping(5,2);
			col2->addMapping(6,4);

			col2->removeMappingFrom(6);
			CPPUNIT_ASSERT_EQUAL(3, col2->rowCount());
			col1->setValueAt(1, 1.3);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(1.3, col2->valueAt(0),EPSILON);
			col2->removeMappingTo(0);
			CPPUNIT_ASSERT_DOUBLES_EQUAL(0.0, col2->valueAt(0),EPSILON);
			CPPUNIT_ASSERT_EQUAL(3, col2->rowCount());
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ColumnTest );

