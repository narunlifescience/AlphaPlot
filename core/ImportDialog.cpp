/***************************************************************************
    File                 : ImportDialog.cpp
    Project              : SciDAVis
    --------------------------------------------------------------------
    Copyright            : (C) 2008 Knut Franke
    Email (use @ for *)  : Knut.Franke*gmx.net
    Description          : Select file(s) to import and import filter to use.

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

#include "ImportDialog.h"
#include "AbstractImportFilter.h"
#include <QGridLayout>
#include <QSpinBox>
#include <QCheckBox>
#include <QLineEdit>
#include <QSignalMapper>
#include <QMetaProperty>
#include <QVariant>
#include <QLabel>
#include <QComboBox>

ImportDialog::ImportDialog(QMap<QString, AbstractImportFilter*> filter_map, QWidget *parent)
	: ExtensibleFileDialog(parent), d_filter_map(filter_map), d_options_ui(0)
{
	setWindowTitle(tr("Import File(s)"));
	setFileMode( QFileDialog::ExistingFiles );

	QWidget * extension_widget = new QWidget();
	QGridLayout * extension_layout = new QGridLayout();
	extension_widget->setLayout(extension_layout);
	extension_layout->addWidget(new QLabel(tr("Destination:")), 0, 0);
	d_destination = new QComboBox(extension_widget);
	// IMPORTANT: keep this in sync with enum Destination
	d_destination->addItem(tr("Current project"));
	d_destination->addItem(tr("New project(s)"));
	extension_layout->addWidget(d_destination, 0, 1);
	setExtensionWidget(extension_widget);

	setFilters(d_filter_map.keys());
	filterChanged(selectedFilter());

	connect(this, SIGNAL(filterSelected(const QString&)),
			this, SLOT(filterChanged(const QString&)));
}

void ImportDialog::filterChanged(const QString& filter_name) {
	d_filter = d_filter_map[filter_name];
	if (!d_filter) {
		if (d_options_ui) delete d_options_ui;
		d_options_ui = 0;
		return;
	}
	if (!QMetaObject::invokeMethod(d_filter, "makeOptionsGui", Qt::DirectConnection,
			Q_RETURN_ARG(QWidget*, d_options_ui)))
		d_options_ui = generateOptionsGUI();
	static_cast<QGridLayout*>(extensionWidget()->layout())->addWidget(d_options_ui, 1, 0, 1, 2);
};

QWidget * ImportDialog::generateOptionsGUI() {
	qDeleteAll(d_settings_map);
	d_settings_map.clear();

	QWidget * settings = new QWidget();
	QGridLayout * layout = new QGridLayout();
	settings->setLayout(layout);
	int offset = d_filter->metaObject()->propertyOffset();
	// don't include properties of QObject
	int prop_count = d_filter->metaObject()->propertyCount() - offset;
	for (int i=0; i<prop_count; i++) {
		QMetaProperty * prop = new QMetaProperty(d_filter->metaObject()->property(offset+i));
		layout->addWidget(new QLabel(QString(prop->name()).replace("_", " "), settings), i/2, 2*(i%2));
		switch(prop->type()) {
			case QVariant::Int:
				{
					QSpinBox * box = new QSpinBox(settings);
					box->setValue(prop->read(d_filter).toInt());
					connect(box, SIGNAL(valueChanged(int)),
							this, SLOT(intValueChanged(int)));
					d_settings_map[static_cast<QObject*>(box)] = prop;
					layout->addWidget(box, i/2, 2*(i%2)+1);
					break;
				}
			case QVariant::Bool:
				{
					QCheckBox * box = new QCheckBox(settings);
					box->setCheckState(prop->read(d_filter).toBool() ? Qt::Checked : Qt::Unchecked);
					connect(box, SIGNAL(stateChanged(int)),
							this, SLOT(boolValueChanged(int)));
					d_settings_map[static_cast<QObject*>(box)] = prop;
					layout->addWidget(box, i/2, 2*(i%2)+1);
					break;
				}
			case QVariant::String:
				{
					QLineEdit * edit = new QLineEdit(settings);
					edit->setText(prop->read(d_filter).toString());
					connect(edit, SIGNAL(textChanged(const QString&)),
							this, SLOT(stringValueChanged(const QString&)));
					d_settings_map[static_cast<QObject*>(edit)] = prop;
					layout->addWidget(edit, i/2, 2*(i%2)+1);
					break;
				}
				// TODO: support more types
			default:
				break;
		}
	}
	return settings;
}

ImportDialog::Destination ImportDialog::destination() const
{
	return (Destination) d_destination->currentIndex();
}

void ImportDialog::intValueChanged(int value)
{
	d_settings_map[sender()]->write(d_filter, QVariant(value));
}

void ImportDialog::boolValueChanged(int state)
{
	switch (state) {
		case Qt::Checked:
			d_settings_map[sender()]->write(d_filter, QVariant(true));
			break;
		case Qt::Unchecked:
			d_settings_map[sender()]->write(d_filter, QVariant(false));
			break;
	}
}

void ImportDialog::stringValueChanged(const QString &value)
{
	d_settings_map[sender()]->write(d_filter, QVariant(value));
}
