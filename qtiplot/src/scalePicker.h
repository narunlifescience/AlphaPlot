/***************************************************************************
    File                 : scalePicker.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief, Tilman Hoener zu Siederdissen
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net
    Description          : Scale and title picker classes
                           
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
#include <qobject.h>

class QwtPlot;
class QwtScaleWidget;
class QwtTextLabel;
class QLabel;
#include <QPoint>
class QRect;
	
/*!\brief Handles user interaction with a QwtScaleWidget.
 *
 * This class is used by Graph to catch events for the scales on its Plot.
 * ScalePicker doesn't take any actions beyond emitting signals, which are then processed by Graph.
 */
class ScalePicker: public QObject
{
    Q_OBJECT
public:
    ScalePicker(QwtPlot *plot);
    virtual bool eventFilter(QObject *, QEvent *);

	//! The bounding rectangle of a scale without the title.
	QRect scaleRect(const QwtScaleWidget *) const;

	void mouseDblClicked(const QwtScaleWidget *, const QPoint &);
	void mouseClicked(const QwtScaleWidget *scale, const QPoint &pos) ;
	void mouseRightClicked(const QwtScaleWidget *scale, const QPoint &pos);

	/*! Install myself as event filter for all axes of my parent.
	 * For every axis of plot(), add myself to the corresponding QwtScaleWidget.
	 * \sa QwtPlot::axisWidget()
	 */
	void refresh();
	
	//! Return my parent casted to QwtPlot.
	QwtPlot *plot() { return (QwtPlot *)parent(); }

signals:
	//! Emitted when the user clicks on one of the monitored axes.
	void clicked();

	/*! Emitted when the user right-clicks on an axis (but not its title).
	 * The argument specifies the axis' QwtScaleDraw::Alignment.
	 */
	void axisRightClicked(int);
	/*! Emitted when the user right-clicks on the title of an axis.
	 * The argument specifies the axis' QwtScaleDraw::Alignment.
	 */
	void axisTitleRightClicked(int);

	/*! Emitted when the user double-clicks on an axis (but not its title).
	 * The argument specifies the axis' QwtScaleDraw::Alignment.
	 */
	void axisDblClicked(int);

	/*! Emitted when the user double-clicks on an the bottom-axis title.
	 * \sa QwtScaleDraw::Alignment
	 */
	void xAxisTitleDblClicked();
	/*! Emitted when the user double-clicks on an the left-axis title.
	 * \sa QwtScaleDraw::Alignment
	 */
	void yAxisTitleDblClicked();
	/*! Emitted when the user double-clicks on an the right-axis title.
	 * \sa QwtScaleDraw::Alignment
	 */
	void rightAxisTitleDblClicked();
	/*! Emitted when the user double-clicks on an the top-axis title.
	 * \sa QwtScaleDraw::Alignment
	 */
	void topAxisTitleDblClicked();
};

class TitlePicker: public QObject
{
    Q_OBJECT
public:
    TitlePicker(QwtPlot *plot);
    virtual bool eventFilter(QObject *, QEvent *);

signals:
	void clicked();
	void doubleClicked();
	void removeTitle();
	void showTitleMenu();

protected:
	QwtTextLabel *title;
};
