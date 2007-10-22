#ifndef ASSERTION_TRATS_H
#define ASSERTION_TRATS_H

#include <cppunit/TestAssert.h>
#include <QString>
#include <QDateTime>
#include "Interval.h"

namespace CppUnit {

	template<> struct assertion_traits<QString>
	{
		static bool equal( const QString& x, const QString& y ) {
			return x == y;
		}
		static std::string toString( const QString& x ) {
			return QString("\"%1\"").arg(x).toStdString();
		}
	};

	template<> struct assertion_traits<QDateTime>
	{
		static bool equal( const QDateTime& x, const QDateTime& y) {
			return qAbs(x.secsTo(y)) < 2;
		}
		static std::string toString( const QDateTime& x) {
			return x.toString("yyyy-MM-dd hh:mm:ss.zzz").toStdString();
		}
	};

	template<> struct assertion_traits<QDate>
	{
		static bool equal( const QDate& x, const QDate& y) {
			return x == y;
		}
		static std::string toString( const QDate& x) {
			return x.toString("yyyy-MM-dd").toStdString();
		}
	};

	template<> struct assertion_traits< Interval<int> >
	{
		static bool equal( const Interval<int> &x, const Interval<int> &y) {
			return x == y;
		}
		static std::string toString( const Interval<int> &x) {
			return x.toString().toStdString();
		}
	};

	template<> struct assertion_traits< QList< Interval<int> > >
	{
		static bool equal( const QList< Interval<int> > &x, const QList< Interval<int> > &y) {
			return x == y;
		}
		static std::string toString( const QList< Interval<int> > &x) {
			QString result;
			foreach(Interval<int> i, x)
				result += i.toString() + " ";
			return result.toStdString();
		}
	};
};

#endif // ifndef ASSERTION_TRATS_H
