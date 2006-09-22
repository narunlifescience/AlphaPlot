/***************************************************************************
	File                 : ScriptingLangDialog.cpp
	Project              : QtiPlot
--------------------------------------------------------------------
	Copyright            : (C) 2006 by Knut Franke
	Email                : knut.franke@gmx.de
	Description          : Dialog for changing the current scripting
	                       language

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
#include "ScriptingLangDialog.h"
#include "application.h"

#include <Q3ListBox>
#include <QPushButton>
#include <Q3HBox>
#include <QLayout>
#include <QApplication>
#include <QMessageBox>
#include <QObjectList>

ScriptingLangDialog::ScriptingLangDialog(ScriptingEnv *env, ApplicationWindow *parent, const char *name,
		bool modal, Qt::WFlags fl )
: QDialog(parent, name, modal, fl), scripted(env)
{
	setCaption(tr("QtiPlot - Select scripting language"));

	langList = new Q3ListBox(this, "langList");

	Q3HBox *box1 = new Q3HBox(this, "box1");
	btnOK = new QPushButton(box1, "btnOK");
	btnOK->setText(tr("OK"));
	btnCancel = new QPushButton(box1, "btnCancel");
	btnCancel->setText(tr("Cancel"));

	QVBoxLayout *layout = new QVBoxLayout(this, 5, 5, "layout");
	layout->addWidget(langList);
	layout->addWidget(box1);

	connect(btnOK, SIGNAL(clicked()), this, SLOT(accept()));
	connect(btnCancel, SIGNAL(clicked()), this, SLOT(close()));

	updateLangList();
}

void ScriptingLangDialog::updateLangList()
{
	langList->clear();
	langList->insertStringList(ScriptingLangManager::languages());
	Q3ListBoxItem *current = langList->findItem(scriptEnv->name());
	if (current)
		langList->setCurrentItem(current);
}

void ScriptingLangDialog::accept()
{
	ApplicationWindow *app = (ApplicationWindow*) parent();
	if (app->setScriptingLang(langList->currentText()))
		close();
	else
		QMessageBox::critical(this, tr("QtiPlot - Scripting Error"),
				tr("Scripting language \"%1\" failed to initialize.").arg(langList->currentText()));
}

