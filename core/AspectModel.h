/***************************************************************************
    File                 : AspectModel.h
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2007 by Knut Franke, Tilman Hoener zu Siederdissen
    Email (use @ for *)  : knut.franke*gmx.de, thzs*gmx.net
    Description          : Private model data managed by AbstractAspect.

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
#ifndef ASPECT_MODEL_H
#define ASPECT_MODEL_H

#include <QString>
#include <QDateTime>
#include <QList>

#ifndef _NO_TR1_
#include "tr1/memory"
using std::tr1::shared_ptr;
#else // if your compiler does not have TR1 support, you can use boost instead:
#include <boost/shared_ptr.hpp>
using boost::shared_ptr;
#endif

class AbstractAspect;

//! Private model data managed by AbstractAspect.
class AspectModel
{
	public:
		AspectModel(const QString& name);

		void addChild(shared_ptr<AbstractAspect> child);
		void insertChild(int index, shared_ptr<AbstractAspect> child);
		int indexOfChild(const AbstractAspect *child) const;
		int indexOfChild(shared_ptr<AbstractAspect> child) const { return indexOfChild(child.get()); }
		void removeChild(shared_ptr<AbstractAspect> child);
		int childCount() const;
		shared_ptr<AbstractAspect> child(int index);

		QString name() const;
		void setName(const QString &value);
		QString comment() const;
		void setComment(const QString &value);
		QString captionSpec() const;
		void setCaptionSpec(const QString &value);
		QDateTime creationTime() const;

		QString caption() const;
	
	private:
		static int indexOfMatchingBrace(const QString &str, int start);
		QList< shared_ptr<AbstractAspect> > d_children;
		QString d_name, d_comment, d_caption_spec;
		QDateTime d_creation_time;

		// Undo commands need access to the signals
		friend class AspectNameChangeCmd;
		friend class AspectCommentChangeCmd;
		friend class AspectCaptionSpecChangeCmd;
		friend class AspectChildRemoveCmd;
		friend class AspectChildAddCmd;
};

#endif // ifndef ASPECT_MODEL_H
