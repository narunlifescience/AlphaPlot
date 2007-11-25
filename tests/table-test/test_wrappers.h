#include "Column.h"
#include "TableModel.h"
#include "TableView.h"
#include "Table.h"
#include <QTableView>
#include <QInputDialog>
#include <QMessageBox>
#include <QContextMenuEvent>
#include <QApplication>
#include <QMainWindow>
#include <QMenu>


#ifndef TESTWRAPPERS_H
#define TESTWRAPPERS_H

class globals
{
	public:
		static QApplication * app;
		static QMainWindow * mw;
		
};

#endif 
