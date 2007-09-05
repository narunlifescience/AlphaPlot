#include <cppunit/extensions/HelperMacros.h>
#include "assertion_traits.h"

#include "DoubleColumnData.h"
#include "StringColumnData.h"
#include "DateTimeColumnData.h"
#include "Interval.h"

class ColumnDataTest : public CppUnit::TestFixture {
		CPPUNIT_TEST_SUITE(ColumnDataTest);
		CPPUNIT_TEST(testDouble);
		CPPUNIT_TEST(testString);
		CPPUNIT_TEST(testIsValid);
		CPPUNIT_TEST(testIntervals);
		CPPUNIT_TEST_SUITE_END();
	public:
		void setUp() {
			dc1 = new DoubleColumnData(100);
			dc2 = new DoubleColumnData(100);
		}
		void tearDown() {
			delete dc1;
			delete dc2;
		}
	private:
		DoubleColumnData *dc1, *dc2;
		void testDouble() {
			DoubleColumnData dc_const(4);
			for(int i=0;i<dc_const.size();i++)
				dc_const[i] = i;
			DoubleColumnData dc_var;

			dc_var.copy(&dc_const);
			CPPUNIT_ASSERT_EQUAL(dc_const.numRows(), dc_var.numRows());
			for (int i=0; i<dc_var.size();i++)
				CPPUNIT_ASSERT_EQUAL(dc_const.valueAt(i), dc_var.valueAt(i));
		}
		void testString() {
			QStringList dummy1, dummy2;
			dummy1 << "nice" << "wheather" << "today";
			dummy2 << "1.5" << "100.25" << "1000.1";
			StringColumnData sc_const(dummy2);
			StringColumnData sc_var(dummy1);

			CPPUNIT_ASSERT_EQUAL(dummy1.size(), sc_var.numRows());
			for (int i=0; i<sc_var.numRows(); i++)
				CPPUNIT_ASSERT_EQUAL(dummy1[i], sc_var.textAt(i));
		}
		void testIsValid() {
			CPPUNIT_ASSERT(dc1->isInvalid(10) == false);
			CPPUNIT_ASSERT(dc1->isInvalid(Interval<int>(10,10)) == false);
			dc1->setInvalid(Interval<int>(10,19), true);
			CPPUNIT_ASSERT(dc1->isInvalid(10) == true);
			dc1->removeRows(10,5);
			dc1->insertEmptyRows(5,5);
			dc1->setInvalid(Interval<int>(10,20), false);
			CPPUNIT_ASSERT(dc1->isInvalid(Interval<int>(12,13)) == false);
			dc1->setInvalid(Interval<int>(12,13), true);
			CPPUNIT_ASSERT(dc1->isInvalid(Interval<int>(12,13)) == true);
		}
		void testIntervals() {
			dc2->setSelected(Interval<int>(1,20));
			dc2->setFormula(Interval<int>(10,20), "foo bar");
			dc2->setFormula(Interval<int>(1,9), "bar baz");
			CPPUNIT_ASSERT_EQUAL(QList< Interval<int> >() << Interval<int>(1,20), dc2->selectedIntervals());
			dc2->removeRows(10,5);
			CPPUNIT_ASSERT_EQUAL(QList< Interval<int> >() << Interval<int>(1,15), dc2->selectedIntervals());
			dc2->setSelected(Interval<int>(20,30));
			CPPUNIT_ASSERT_EQUAL(2, dc2->selectedIntervals().size());
			CPPUNIT_ASSERT(dc2->selectedIntervals().contains(Interval<int>(1,15)));
			CPPUNIT_ASSERT(dc2->selectedIntervals().contains(Interval<int>(20,30)));
			dc2->removeRows(15,5);
			CPPUNIT_ASSERT_EQUAL(QList< Interval<int> >() << Interval<int>(1,25), dc2->selectedIntervals());
			dc2->insertEmptyRows(5,5);
			CPPUNIT_ASSERT_EQUAL(2, dc2->selectedIntervals().size());
			CPPUNIT_ASSERT(dc2->selectedIntervals().contains(Interval<int>(1,4)));
			CPPUNIT_ASSERT(dc2->selectedIntervals().contains(Interval<int>(10,30)));
			CPPUNIT_ASSERT_EQUAL(QString("bar baz"), dc2->formula(1));
			CPPUNIT_ASSERT_EQUAL(QString("bar baz"), dc2->formula(4));
			CPPUNIT_ASSERT_EQUAL(QString(""), dc2->formula(5));
			CPPUNIT_ASSERT_EQUAL(QString("bar baz"), dc2->formula(10));
			CPPUNIT_ASSERT_EQUAL(QString("bar baz"), dc2->formula(14));
			CPPUNIT_ASSERT_EQUAL(QString("foo bar"), dc2->formula(15));
			CPPUNIT_ASSERT_EQUAL(QString("foo bar"), dc2->formula(17));
			CPPUNIT_ASSERT_EQUAL(QString("foo bar"), dc2->formula(19));
			CPPUNIT_ASSERT_EQUAL(QString(""), dc2->formula(20));
			CPPUNIT_ASSERT_EQUAL(QString(""), dc2->formula(30));
		}
};

CPPUNIT_TEST_SUITE_REGISTRATION( ColumnDataTest );

