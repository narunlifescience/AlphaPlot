/***************************************************************************
    File                 : SelectionMoveResizer.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke
    Email (use @ for *)  : knut.franke*gmx.de
    Description          : Selection of Widgets and QwtPlotMarkers

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

#ifndef SELECTION_MOVE_RESIZER_H
#define SELECTION_MOVE_RESIZER_H

#include <QWidget>
#include <QList>
#include <QRect>

class QPoint;
class QwtPlotMarker;
class TextEnrichment;
class LineEnrichment;
class ImageEnrichment;

/*!\brief Handler for modifying one or more QWidget and/or QwtPlotMarker with mouse or keyboard.
 *
 * During initialization, the SelectionMoveResizer adds itself as a new child widget to
 * the target's parentWidget() or QwtPlotCanvas. Because Qt 4.1 and beyond propagate a widget's
 * background by default, frame and resize handlers can then simply be drawn on top of
 * the usual content of the parent.
 * Also, the SelectionMoveResizer gets to see mouse and key events first without the need to filter
 * events for its parent.
 * %Note however that the latter must still be done in order to catch geometry changes and
 * stay the same size as the parent.
 *
 * At every point in time, the list of managed targets must be non-empty,
 * so that the SelectionMoveResizer has a well-defined bounding rectangle.
 * When there are no more selected targets, it deletes itself, so references to a SelectionMoveResizer
 * should be maintained by QPointer and checked for validity before use.
 *
 * QWidget targets are automatically removed when they are deleted (possibly also deleting the
 * SelectionMoveResizer), using the QObject::destroyed() signal. With QwtPlotMarker, this is not
 * possible since it's not a QObject. Thus, care has to be taken to remove them before or shortly after deletion,
 * lest the wrath of Segmentation Fault shall come upon you.
 *
 * \section design Design Ideas
 *   - Keep as much of the select/move/resize code as possible in one class to ease maintanance.
 *   - Use the same class for layer and marker handling to avoid duplicating code.
 *     Good for bugfixing and for maintaining a consistent user interface.
 *   - Keep this class generic so that it can later be used for other parents
 *     (than Graph's canvas QWidget and Plot's QwtPlotCanvas)
 *     and/or targets (e.g. new QwtPlotMarker subclasses) with little to no modifications.
 *   - Differentiate between move/resize, which are generic operations, and editing (like moving the
 *     endpoints of a LineEnrichment), which requires detailed knowledge about the target.
 *     This way, new types of markers or Graph children support moving/resizing without further
 *     work. Editing can then be implemented in the targets themselves, as shown in LineEnrichment.
 *   .
 *
 * \section future_plans Future Plans
 * See the documentation of ImageEnrichment for ideas about replacing QwtPlotMarker with a new QwtPlotItem subclass.
 * For SelectionMoveResizer this would mean supporting new enrichments without modifications and being able
 * to automatically remove them upon deletion (if the new class inherits QObject).
 * Not to mention that the implementation would be a lot cleaner if we would only have to handle two target classes
 * instead of four.
 *
 * Think about turning SelectionMoveResizer into an input method for general affine transformations.
 * Particularly, this would add rotation, flipping and shearing.
 * Markers could implement this using QPainter's coordinate transformations.
 * Definitely not a priority, though.
 */
class SelectionMoveResizer : public QWidget
{
	Q_OBJECT

	public:
		//! Construct a new MoveResizer with the given marker as the only target.
		SelectionMoveResizer(TextEnrichment *target);
		//! Construct a new MoveResizer with the given marker as the only target.
		SelectionMoveResizer(LineEnrichment *target);
		//! Construct a new MoveResizer with the given marker as the only target.
		SelectionMoveResizer(ImageEnrichment *target);
		//! Construct a new MoveResizer with the given widget as the only target.
		SelectionMoveResizer(QWidget *target);
		//! Clean up after myself.
		~SelectionMoveResizer();
		/*!\brief React on geometry changes of parent and targets.
		 */
		virtual bool eventFilter(QObject *o, QEvent *e);
		//! Returns true if w is one of targets, false else.
		bool contains(QWidget *w) const { return m_widgets.contains(w); };
		//! Returns true if m is one of targets, false else.
		bool contains(TextEnrichment *m) const { return m_legend_markers.contains(m); };
		//! Returns true if m is one of targets, false else.
		bool contains(LineEnrichment *m) const { return m_line_markers.contains(m); };
		//! Returns true if m is one of targets, false else.
		bool contains(ImageEnrichment *m) const { return m_image_markers.contains(m); };

	public slots:
		//! Add target to the list of items to be moved/resized together.
		void add(TextEnrichment *target);
		//! Add target to the list of items to be moved/resized together.
		void add(LineEnrichment *target);
		//! Add target to the list of items to be moved/resized together.
		void add(ImageEnrichment *target);
		//! Add target to the list of items to be moved/resized together.
		void add(QWidget *target);
		//! Remove target from the list of items to be moved/resized together and returns the number of occurences removed.
		int removeAll(TextEnrichment *target);
		//! Remove target from the list of items to be moved/resized together and returns the number of occurences removed.
		int removeAll(LineEnrichment *target);
		//! Remove target from the list of items to be moved/resized together and returns the number of occurences removed.
		int removeAll(ImageEnrichment *target);
		//! Remove target from the list of items to be moved/resized together and returns the number of occurences removed.
		int removeAll(QWidget *target);
		//! Calculate #m_bounding_rect based on the bounding rectangles of all targets.
		void recalcBoundingRect();

	signals:
		//! Emitted when the targets are modified (moved or resized).
		void targetsChanged();

	protected:
		//! Available modes of operation.
		enum Operation { None=-2, Move, Resize_N, Resize_NE, Resize_E, Resize_SE, Resize_S, Resize_SW, Resize_W, Resize_NW };

		/*!\brief Draw frame and resize handlers.
		 *
		 * Besides managing resize operations, this also provides a visual feedback
		 * on the selection state.
		 */
		virtual void paintEvent(QPaintEvent *e);
		/*!\brief Mouse button presses start move/resize operations.
		 *
		 * Clicks outside of #m_bounding_rect or with anything else than the left button
		 * are propagated to the parent as usual.
		 */
		virtual void mousePressEvent(QMouseEvent *e);
		/*!\brief Mouse movements need to be monitored for updating the frame during operation.
		 *
		 * When no operation is in progress, the mouse cursor is updated based on its position
		 * before the event is passed on to the parent.
		 */
		virtual void mouseMoveEvent(QMouseEvent *e);
		/*!\brief Mouse releases end the current operation and apply it to the targets.
		 *
		 * When there is no operation in progress, the event is passed on to the parent.
		 */
		virtual void mouseReleaseEvent(QMouseEvent *e);
		/*!\brief Allow keyboard-based moving of the selection.
		 *
		 * Unused keys are passed on to the parent.
		 */
		virtual void keyPressEvent(QKeyEvent *e);
		//! Ignore double-clicks so that they can be used by my parent.
		virtual void mouseDoubleClickEvent(QMouseEvent *e);

	private:
		//! Size of resize handle in pixels.
		static const int handler_size = 10;
		//! Return one of the rectangles to be filled for starting resize operations.
		static const QRect handlerRect(QRect rect, Operation op);
		//! Apply operation specified by #m_op and #m_op_dp to in.
		QRect operateOn(const QRect in);
		//! Apply operation specified by #m_op and #m_op_dp to all targets.
		void operateOnTargets();
		//! The initialization part common to all constructors.
		void init();
		//! Get the bounding rectangle of a marker in paint coordinates.
		QRect boundingRectOf(QwtPlotMarker *target) const;

		//! Target markers I'm handling.
		QList <TextEnrichment *> m_legend_markers;
		//! Target markers I'm handling.
		QList <LineEnrichment *> m_line_markers;
		//! Target markers I'm handling.
		QList <ImageEnrichment *> m_image_markers;
		//! Target widgets I'm handling.
		QList <QWidget *> m_widgets;
		//! Bounding rectangle of all targets taken together, in drawing coordinates.
		QRect m_bounding_rect;
		//! The type of operation currently in progress.
		Operation m_op;
		//! Mouse position where an operation started; (-1,-1) means started by keyboard.
		QPoint m_op_start;
		//! Difference between current and start position during operation.
		QPoint m_op_dp;

	private slots:
		//! A non-typesafe version of remvoveAll(QWidget*) needed for QObject::destroyed().
		void removeWidget(QObject* w) { removeAll((QWidget*) w); }
};

#endif // ifndef SELECTION_MOVE_RESIZER_H
