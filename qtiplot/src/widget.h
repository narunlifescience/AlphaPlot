/***************************************************************************
    File                 : widget.h
    Project              : QtiPlot
    --------------------------------------------------------------------
    Copyright            : (C) 2006 by Ion Vasilief,
                           Tilman Hoener zu Siederdissen,
                           Knut Franke
    Email                : ion_vasilief@yahoo.fr, thzs@gmx.net,
                           knut.franke@gmx.de
    Description          : MDI window widget
                           
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
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
class QEvent;
class QCloseEvent;
class QString;
class Folder;

/**
 * \brief Base class of all MDI client windows.
 *
 * These are the main objects of every Qtiplot project.
 * All content (apart from the directory structure) is managed by subclasses of MyWidget.
 *
 * \section future Future Plans
 * Rename to MDIWindow (or some yet-to-find better name; ideas anyone?).
 *
 * \sa Folder, ApplicationWindow
 */
class MyWidget: public QWidget
{
	Q_OBJECT

public:	
	
	//! Constructor
	/**
	 * \param label window label
	 * \param parent parent widget
	 * \param name window name
	 * \param f window flags
	 * \sa setCaptionPolicy(), captionPolicy()
	 */
	MyWidget(const QString& label = QString(), QWidget * parent = 0, const char * name = 0, Qt::WFlags f = 0);

	enum CaptionPolicy{Name = 0, Label = 1, Both = 2};
	enum Status{Hidden = -1, Normal = 0, Minimized = 1, Maximized = 2};

	//! Return the window label
	QString windowLabel(){return QString(w_label);};
	//! Set the window label
	void setWindowLabel(const QString& s) { w_label = s; updateCaption(); };

	//! Return the window name
	QString name(){return objectName();};
	//! Set the window name
	void setName(const QString& s){setObjectName(s);};

	//! Return the caption policy
	CaptionPolicy captionPolicy(){return caption_policy;};
	//! Set the caption policy
	/**
	 * The caption policy can be
	 * Name -> caption determined by the window name
	 * Label -> caption detemined by the window label
	 * Both -> caption = "name - label"
	 */
	void setCaptionPolicy(CaptionPolicy policy) { caption_policy = policy; updateCaption(); }

	//! Set the widget's name
	void setName(const char *newname) { QWidget::setName(newname); updateCaption(); }

	//! Return the creation date
	QString birthDate(){return birthdate;};
	//! Set the creation date
	void setBirthDate(const QString& s){birthdate = s;};

	//! Return the window status as a string
	QString aspect();
	//! Return the window status flag (hidden, normal, minimized or maximized)
	Status status(){return w_status;};
	//! Set the window status flag (hidden, normal, minimized or maximized)
	void setStatus(Status s){w_status = s;};

	// TODO:
	//! Not implemented yet
	virtual QString saveAsTemplate(const QString& ){return QString();};
	// TODO:
	//! Not implemented yet
	virtual void restore(const QStringList& ){};

	// TODO:
	//! Not implemented, yet
	virtual void print(){};
	// TODO:
	//! Not implemented, yet
	virtual QString saveToString(const QString &){return QString();};
	
	// TODO: make this return something useful
	//! Size of the widget as a string
	virtual QString sizeToString();

	//!Notifies a change in the status of a former maximized window after it was shown as normal as a result of an indirect user action (e.g.: another window was maximized)
	void setNormal();

	//!Notifies that a window was hidden by a direct user action
	void setHidden();

	//event handlers
	//! Close event handler 
	/**
	 * Ask the user "delete, hide, or cancel?" if the 
	 * "ask on close" flag is set.
	 */
	void closeEvent( QCloseEvent *);
	//! Toggle the "ask on close" flag
	void askOnCloseEvent(bool ask){askOnClose = ask;};
	//! General event handler (updates the window status if it changed)
	bool event( QEvent *e );
	//! Filters other object's events (customizes title bar's context menu)
	bool eventFilter(QObject *object, QEvent *e);

	//! Show the window maximized
	void showMaximized();
	//! Tells if a resize event was requested by the user or generated programatically
	bool userRequested(){return user_request;};

	//! Returns the pointer to the parent folder of the window
	Folder* folder(){return parentFolder;};

	//! Initializes the pointer to the parent folder of the window
	void setFolder(Folder* f){parentFolder = f;};

signals:  
	//! Emitted when the window was closed
	void closedWindow(MyWidget *);
	//! Emitted when the window was hidden
	void hiddenWindow(MyWidget *);
	void modifiedWindow(QWidget *);
	void resizedWindow(QWidget *);
	//! Emitted when the window status changed
	void statusChanged(MyWidget *);
	//! Emitted when the title bar recieves a QContextMenuEvent
	void showTitleBarMenu();

protected:
	//! Catches parent changes (in order to gain access to the title bar)
	virtual void changeEvent(QEvent *event);
	//! Title bar of this MDI window if it currently belongs to a QWorkspace, NULL else
	QWidget *titleBar;

private:
	//! set caption according to current CaptionPolicy, name and label
	void updateCaption();

	//!Pointer to the parent folder of the window
	Folder *parentFolder;
	//! The window label
	/**
	 * \sa setWindowLabel(), windowLabel(), setCaptionPolicy()
	 */
	QString w_label;
	//! The creation date
	QString birthdate;
	//! The window status
	Status w_status;
	//! The caption policy
	/**
	 * \sa setCaptionPolicy(), captionPolicy()
	 */
	CaptionPolicy caption_policy;
	//! Toggle on/off: Ask the user "delete, hide, or cancel?" on a close event
	bool askOnClose;
	//! Tells if the showMaximized action was requested by the user or generated programatically
	bool user_request;
};

#endif
