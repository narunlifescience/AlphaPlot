/***************************************************************************
    File                 : Column.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Tilman Hoener zu Siederdissen,
    Email (use @ for *)  : thzs*gmx.net
    Description          : Table column class

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

#include "Column.h"
#include "ColumnPrivate.h"
#include "columncommands.h"
#include <QIcon>

Column::Column(const QString& label, SciDAVis::ColumnMode mode)
 : AbstractAspect(label)
{
	d_column_private = shared_ptr<ColumnPrivate>(new ColumnPrivate(this, mode));
	init();
}

Column::Column(const QString& label, QVector<double> data, IntervalAttribute<bool> validity)
 : AbstractAspect(label)
{
	d_column_private = shared_ptr<ColumnPrivate>(new ColumnPrivate(this, SciDAVis::TypeDouble, 
		SciDAVis::Numeric, new QVector<double>(data), validity));
	init();
}

Column::Column(const QString& label, QStringList data, IntervalAttribute<bool> validity)
 : AbstractAspect(label)
{
	d_column_private = shared_ptr<ColumnPrivate>(new ColumnPrivate(this, SciDAVis::TypeQString,
		SciDAVis::Text, new QStringList(data), validity));
	init();
}

Column::Column(const QString& label, QList<QDateTime> data, IntervalAttribute<bool> validity)
 : AbstractAspect(label)
{
	d_column_private = shared_ptr<ColumnPrivate>(new ColumnPrivate(this, SciDAVis::TypeQDateTime, 
		SciDAVis::DateTime, new QList<QDateTime>(data), validity));
	init();
}

void Column::init()
{
	connect(abstractAspectSignalEmitter(), SIGNAL(aspectDescriptionAboutToChange(AbstractAspect *)),
		this, SLOT(emitDescriptionAboutToChange(AbstractAspect *)));
	connect(abstractAspectSignalEmitter(), SIGNAL(aspectDescriptionChanged(AbstractAspect *)),
		this, SLOT(emitDescriptionChanged(AbstractAspect *)));
}

void Column::emitDescriptionAboutToChange(AbstractAspect * aspect)
{
	if (aspect != static_cast<AbstractAspect *>(this)) return;
	emit abstractColumnSignalEmitter()->descriptionAboutToChange(this);
}

void Column::emitDescriptionChanged(AbstractAspect * aspect)
{
	if (aspect != static_cast<AbstractAspect *>(this)) return;
	emit abstractColumnSignalEmitter()->descriptionChanged(this);
}

Column::~Column()
{
}

void Column::setColumnMode(SciDAVis::ColumnMode mode)
{
	exec(new ColumnSetModeCmd(d_column_private, mode));
}


bool Column::copy(const AbstractColumn * other)
{
	if(other->dataType() != dataType()) return false;
	exec(new ColumnFullCopyCmd(d_column_private, other));
	return true;
}

bool Column::copy(const AbstractColumn * source, int source_start, int dest_start, int num_rows)
{
	if(source->dataType() != dataType()) return false;
	exec(new ColumnPartialCopyCmd(d_column_private, source, source_start, dest_start, num_rows));
	return true;
}

void Column::insertRows(int before, int count)
{
	exec(new ColumnInsertEmptyRowsCmd(d_column_private, before, count));
}

void Column::removeRows(int first, int count)
{
	exec(new ColumnRemoveRowsCmd(d_column_private, first, count));
}

void Column::setPlotDesignation(SciDAVis::PlotDesignation pd)
{
	exec(new ColumnSetPlotDesignationCmd(d_column_private, pd));
}

void Column::clear()
{
	exec(new ColumnClearCmd(d_column_private));
}

void Column::notifyReplacement(shared_ptr<AbstractColumn> replacement)
{
	emit abstractColumnSignalEmitter()->aboutToBeReplaced(this, replacement); 
}

void Column::clearValidity()
{
	exec(new ColumnClearValidityCmd(d_column_private));
}

void Column::clearMasks()
{
	exec(new ColumnClearMasksCmd(d_column_private));
}

void Column::setInvalid(Interval<int> i, bool invalid)
{
	exec(new ColumnSetInvalidCmd(d_column_private, i, invalid));
}

void Column::setInvalid(int row, bool invalid)
{
	setInvalid(Interval<int>(row,row), invalid);
}

void Column::setMasked(Interval<int> i, bool mask)
{
	exec(new ColumnSetMaskedCmd(d_column_private, i, mask));
}

void Column::setMasked(int row, bool mask)
{
	setMasked(Interval<int>(row,row), mask);
}

void Column::setFormula(Interval<int> i, QString formula)
{
	exec(new ColumnSetFormulaCmd(d_column_private, i, formula));
}

void Column::setFormula(int row, QString formula)
{
	setFormula(Interval<int>(row, row), formula);
}

void Column::clearFormulas()
{
	exec(new ColumnClearFormulasCmd(d_column_private));
}

void Column::setTextAt(int row, const QString& new_value)
{
	exec(new ColumnSetTextCmd(d_column_private, row, new_value));
}

void Column::replaceTexts(int first, const QStringList& new_values)
{
	exec(new ColumnReplaceTextsCmd(d_column_private, first, new_values));
}

void Column::setDateAt(int row, const QDate& new_value)
{
	setDateTimeAt(row, QDateTime(new_value, timeAt(row)));
}

void Column::setTimeAt(int row,const QTime& new_value)
{
	setDateTimeAt(row, QDateTime(dateAt(row), new_value));
}

void Column::setDateTimeAt(int row, const QDateTime& new_value)
{
	exec(new ColumnSetDateTimeCmd(d_column_private, row, new_value));
}

void Column::replaceDateTimes(int first, const QList<QDateTime>& new_values)
{
	exec(new ColumnReplaceDateTimesCmd(d_column_private, first, new_values));
}

void Column::setValueAt(int row, double new_value)
{
	exec(new ColumnSetValueCmd(d_column_private, row, new_value));
}

void Column::replaceValues(int first, const QVector<double>& new_values)
{
	exec(new ColumnReplaceValuesCmd(d_column_private, first, new_values));
}

QString Column::textAt(int row) const
{
	return d_column_private->textAt(row);
}

QDate Column::dateAt(int row) const
{
	return d_column_private->dateAt(row);
}

QTime Column::timeAt(int row) const
{
	return d_column_private->timeAt(row);
}

QDateTime Column::dateTimeAt(int row) const
{
	return d_column_private->dateTimeAt(row);
}

double Column::valueAt(int row) const
{
	return d_column_private->valueAt(row);
}

void Column::setColumnLabel(const QString& label) 
{ 
	setName(label); 
}

void Column::setColumnComment(const QString& comment) 
{ 
	setComment(comment); 
}

QIcon Column::icon() const
{
	switch(dataType())
	{
		case SciDAVis::TypeDouble:
			return QIcon(QPixmap(":/numerictype.png"));
		case SciDAVis::TypeQString:
			return QIcon(QPixmap(":/texttype.png"));
		case SciDAVis::TypeQDateTime:
			return QIcon(QPixmap(":/datetype.png"));
	}
	return QIcon();
}

void Column::save(QXmlStreamWriter * writer)
{
	writer->writeStartElement("column");
	// TODO: write AbstractAspect stuff
	// TODO: add saving of in/out filters
	writer->writeAttribute("label", columnLabel());
	writer->writeAttribute("type", SciDAVis::enumValueToString(dataType(), "ColumnDataType"));
	writer->writeAttribute("mode", SciDAVis::enumValueToString(columnMode(), "ColumnMode"));
	writer->writeAttribute("plot_designation", SciDAVis::enumValueToString(plotDesignation(), "PlotDesignation"));
	writer->writeStartElement("comment");
	writer->writeCharacters(columnComment());
	writer->writeEndElement();
	QList< Interval<int> > masks = maskedIntervals();
	foreach(Interval<int> interval, masks)
	{
		writer->writeStartElement("mask");
		writer->writeAttribute("start_row", QString::number(interval.start()));
		writer->writeAttribute("end_row", QString::number(interval.end()));
		writer->writeEndElement();
	}
	QList< Interval<int> > formulas = formulaIntervals();
	foreach(Interval<int> interval, formulas)
	{
		writer->writeStartElement("formula");
		writer->writeAttribute("start_row", QString::number(interval.start()));
		writer->writeAttribute("end_row", QString::number(interval.end()));
		writer->writeCharacters(formula(interval.start()));
		writer->writeEndElement();
	}
	int i;
	switch(dataType())
	{
		case SciDAVis::TypeDouble:
			for(i=0; i<rowCount(); i++)
			{
				writer->writeStartElement("row");
				writer->writeAttribute("type", SciDAVis::enumValueToString(dataType(), "ColumnDataType"));
				writer->writeAttribute("index", QString::number(i));
				writer->writeAttribute("invalid", isInvalid(i) ? "yes" : "no");
				writer->writeCharacters(QString::number(valueAt(i), 'e', 16));
				writer->writeEndElement();
			}
			break;
		case SciDAVis::TypeQString:
			for(i=0; i<rowCount(); i++)
			{
				writer->writeStartElement("row");
				writer->writeAttribute("type", SciDAVis::enumValueToString(dataType(), "ColumnDataType"));
				writer->writeAttribute("index", QString::number(i));
				writer->writeAttribute("invalid", isInvalid(i) ? "yes" : "no");
				writer->writeCharacters(textAt(i));
				writer->writeEndElement();
			}
			break;

		case SciDAVis::TypeQDateTime:
			for(i=0; i<rowCount(); i++)
			{
				writer->writeStartElement("row");
				writer->writeAttribute("type", SciDAVis::enumValueToString(dataType(), "ColumnDataType"));
				writer->writeAttribute("index", QString::number(i));
				writer->writeAttribute("invalid", isInvalid(i) ? "yes" : "no");
				writer->writeCharacters(dateTimeAt(i).toString("yyyy-dd-MM hh:mm:ss:zzz"));
				writer->writeEndElement();
			}
			break;
	}
	writer->writeEndElement(); // "column"
}


bool Column::load(QXmlStreamReader * reader)
{
	QString prefix(tr("XML read error: ","prefix for XML error messages"));
	QString postfix(tr(" (loading failed)", "postfix for XML error messages"));

	clear();
	clearValidity();
	clearFormulas();
	clearMasks();
	setColumnComment(QString());
	if(reader->isStartElement() && reader->name() == "column") 
	{
		QXmlStreamAttributes attribs = reader->attributes();
		QString str;

		// read label
		str = attribs.value(reader->namespaceUri().toString(), "label").toString();
		if(str.isEmpty())
		{
			reader->raiseError(prefix+tr("column label missing")+postfix);
			return false;
		}
		setColumnLabel(str);
		// read type
		str = attribs.value(reader->namespaceUri().toString(), "type").toString();
		if(str.isEmpty())
		{
			reader->raiseError(prefix+tr("column type missing")+postfix);
			return false;
		}
		int type_code = SciDAVis::enumStringToValue(str, "ColumnDataType");
		if(type_code == -1)
		{
			reader->raiseError(prefix+tr("column type invalid")+postfix);
			return false;
		}
		// read mode
		str = attribs.value(reader->namespaceUri().toString(), "mode").toString();
		if(str.isEmpty())
		{
			reader->raiseError(prefix+tr("column mode missing")+postfix);
			return false;
		}
		int mode_code = SciDAVis::enumStringToValue(str, "ColumnMode");
		if(mode_code == -1)
		{
			reader->raiseError(prefix+tr("column mode invalid")+postfix);
			return false;
		}
		setColumnMode((SciDAVis::ColumnMode)mode_code);
		if(type_code != int(dataType()))
		{
			reader->raiseError(prefix+tr("column type or mode invalid")+postfix);
			return false;
		}
		// read plot designation
		str = attribs.value(reader->namespaceUri().toString(), "plot_designation").toString();
		int pd_code = SciDAVis::enumStringToValue(str, "PlotDesignation");
		if(str.isEmpty())
			setPlotDesignation(SciDAVis::noDesignation);
		else if(pd_code == -1)
		{
			reader->raiseError(prefix+tr("column plot designation invalid")+postfix);
			return false;
		}
		else
			setPlotDesignation((SciDAVis::PlotDesignation)pd_code);

		// read child elements
		while (!reader->atEnd()) 
		{
			reader->readNext();

			if (reader->isEndElement()) break;

			if (reader->isStartElement()) 
			{
				bool ret_val = true;
				if (reader->name() == "comment")
					ret_val = XmlReadComment(reader);
				else if(reader->name() == "mask")
					ret_val = XmlReadMask(reader);
				else if(reader->name() == "formula")
					ret_val = XmlReadFormula(reader);
				else if(reader->name() == "row")
					ret_val = XmlReadRow(reader);
				else
					reader->readElementText(); // unknown element
				if(!ret_val)
					return false;
			} 
		}
	}
	else // no column element
		reader->raiseError(prefix+tr("no column element found")+postfix);

	return !reader->error();
}
					
bool Column::XmlReadComment(QXmlStreamReader * reader)
{
	Q_ASSERT(reader->isStartElement() && reader->name() == "comment");
	setColumnComment(reader->readElementText());
	return true;
}

bool Column::XmlReadMask(QXmlStreamReader * reader)
{
	QString prefix(tr("XML read error: ","prefix for XML error messages"));
	QString postfix(tr(" (loading failed)", "postfix for XML error messages"));

	Q_ASSERT(reader->isStartElement() && reader->name() == "mask");

	QXmlStreamAttributes attribs = reader->attributes();
	QString str_start,str_end;

	str_start = attribs.value(reader->namespaceUri().toString(), "start_row").toString();
	str_end = attribs.value(reader->namespaceUri().toString(), "end_row").toString();
	if(str_start.isEmpty() || str_end.isEmpty()) 
		reader->raiseError(prefix+tr("missing start or end row")+postfix);

	bool ok1,ok2;
	int start,end;
	start = str_start.toInt(&ok1);
	end = str_start.toInt(&ok1);
	if(!ok1 || !ok2) 
		reader->raiseError(prefix+tr("invalid start or end row")+postfix);

	setMasked(Interval<int>(start,end));
	return true;
}

bool Column::XmlReadFormula(QXmlStreamReader * reader)
{
	QString prefix(tr("XML read error: ","prefix for XML error messages"));
	QString postfix(tr(" (loading failed)", "postfix for XML error messages"));

	Q_ASSERT(reader->isStartElement() && reader->name() == "formula");

	QXmlStreamAttributes attribs = reader->attributes();
	QString str_start,str_end;

	str_start = attribs.value(reader->namespaceUri().toString(), "start_row").toString();
	str_end = attribs.value(reader->namespaceUri().toString(), "end_row").toString();
	if(str_start.isEmpty() || str_end.isEmpty()) 
		reader->raiseError(prefix+tr("missing start or end row")+postfix);

	bool ok1,ok2;
	int start,end;
	start = str_start.toInt(&ok1);
	end = str_start.toInt(&ok1);
	if(!ok1 || !ok2) 
		reader->raiseError(prefix+tr("invalid start or end row")+postfix);

	setFormula(Interval<int>(start,end),reader->readElementText());
	return true;
}

bool Column::XmlReadRow(QXmlStreamReader * reader)
{
	QString prefix(tr("XML read error: ","prefix for XML error messages"));
	QString postfix(tr(" (loading failed)", "postfix for XML error messages"));

	Q_ASSERT(reader->isStartElement() && reader->name() == "row");
	
	QString str;
	int type_code;

	QXmlStreamAttributes attribs = reader->attributes();
	// verfiy type
	str = attribs.value(reader->namespaceUri().toString(), "type").toString();
	type_code = SciDAVis::enumStringToValue(str, "ColumnDataType");
	bool ok;
	if(str.isEmpty() || type_code == -1 || type_code != int(dataType()))
	{
		reader->raiseError(prefix+tr("invalid or missing row type")+postfix);
		return false;
	}
	
	str = attribs.value(reader->namespaceUri().toString(), "index").toString();
	int index = str.toInt(&ok);
	if(str.isEmpty() || !ok)
	{
		reader->raiseError(prefix+tr("invalid or missing row index")+postfix);
		return false;
	}

	str = attribs.value(reader->namespaceUri().toString(), "invalid").toString();
	if(str == "yes") setInvalid(index);

	switch(dataType())
	{
		case SciDAVis::TypeDouble:
			{
			str = reader->readElementText();
			double value = str.toDouble(&ok);
			if(!ok)
			{
				reader->raiseError(prefix+tr("invalid row value")+postfix);
				return false;
			}
			setValueAt(index, value);
			break;
			}
		case SciDAVis::TypeQString:
			str = reader->readElementText();
			setTextAt(index, str);
			break;

		case SciDAVis::TypeQDateTime:
			str = reader->readElementText();
			QDateTime date_time = QDateTime::fromString(str,"yyyy-dd-MM hh:mm:ss:zzz");
			setDateTimeAt(index, date_time);
			break;
	}

	return true;
}


