#include "ArrangeLegend2D.h"

#include <QListWidget>
#include <QPushButton>

#include "2Dplot/AxisRect2D.h"
#include "2Dplot/Bar2D.h"
#include "2Dplot/Channel2D.h"
#include "2Dplot/ColorMap2D.h"
#include "2Dplot/Curve2D.h"
#include "2Dplot/Legend2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "2Dplot/StatBox2D.h"
#include "2Dplot/Vector2D.h"
#include "core/IconLoader.h"

ArrangeLegend2D::ArrangeLegend2D(QWidget *parent, Legend2D *legend,
                                 Qt::WindowFlags fl)
    : QDialog(parent, fl),
      legend_(legend),
      btnOK_(new QPushButton),
      btnCancel_(new QPushButton),
      btnUp_(new QPushButton),
      btnDown_(new QPushButton),
      plotlegendlist_(new QListWidget) {
  Q_ASSERT(legend_);

  plotlegendlist_->setSelectionMode(QAbstractItemView::SingleSelection);
  btnOK_->setText(tr("OK"));
  btnCancel_->setText(tr("Cancel"));
  btnUp_->setText(tr("Move Up"));
  btnDown_->setText(tr("Move Down"));
  btnUp_->setIcon(IconLoader::load("edit-up", IconLoader::LightDark));
  btnDown_->setIcon(IconLoader::load("edit-down", IconLoader::LightDark));
  setWindowTitle(tr("Arrange Legend 2D Plots"));
  setWindowIcon(IconLoader::load("edit-legend", IconLoader::LightDark));
  setSizeGripEnabled(true);
  setFocus();

  QHBoxLayout *hl = new QHBoxLayout();
  QVBoxLayout *vl = new QVBoxLayout();
  vl->addWidget(btnUp_);
  vl->addWidget(btnDown_);
  vl->addWidget(btnOK_);
  vl->addWidget(btnCancel_);
  vl->addStretch();
  hl->addWidget(plotlegendlist_);
  hl->addLayout(vl);
  setLayout(hl);
  hl->setContentsMargins(3, 3, 3, 3);
  hl->setSpacing(3);
  vl->setContentsMargins(0, 0, 0, 0);
  vl->setSpacing(3);

  // plottables
  cvec_ = legend_->getaxisrect_legend()->getCurveVec();
  lsvec_ = legend_->getaxisrect_legend()->getLsVec();
  lspecialvec_ = legend_->getaxisrect_legend()->getChannelVec();
  vvec_ = legend_->getaxisrect_legend()->getVectorVec();
  bvec_ = legend_->getaxisrect_legend()->getBarVec();
  svec_ = legend_->getaxisrect_legend()->getStatBoxVec();
  pvec_ = legend_->getaxisrect_legend()->getPieVec();
  covec_ = legend_->getaxisrect_legend()->getColorMapVec();

  btnUp_->setEnabled(false);
  btnDown_->setEnabled(false);
  connect(plotlegendlist_, &QListWidget::itemSelectionChanged, this, [&]() {
    if (plotlegendlist_->selectedItems().count() > 0) {
      if (plotlegendlist_->currentRow() == 0 && plotlegendlist_->count() == 1) {
        btnUp_->setEnabled(false);
        btnDown_->setEnabled(false);
      } else if (plotlegendlist_->currentRow() == 0) {
        btnUp_->setEnabled(false);
        btnDown_->setEnabled(true);
      } else if (plotlegendlist_->currentRow() ==
                 plotlegendlist_->count() - 1) {
        btnUp_->setEnabled(true);
        btnDown_->setEnabled(false);
      } else {
        btnUp_->setEnabled(true);
        btnDown_->setEnabled(true);
      }
    } else {
      btnUp_->setEnabled(false);
      btnDown_->setEnabled(false);
    }
  });
  connect(btnUp_, &QPushButton::clicked, this, [&]() {
    QListWidgetItem *citem = plotlegendlist_->currentItem();
    int crow = plotlegendlist_->currentRow();
    if (citem && crow > 0) {
      plotlegendlist_->takeItem(crow);
      plotlegendlist_->insertItem(crow - 1, citem);
      abstactvec_.swapItemsAt(crow, crow - 1);
      plotlegendlist_->setCurrentRow(crow - 1);
      // plotlegendlist_->itemSelectionChanged();
    } else
      qDebug() << "unable to move up";
  });
  connect(btnDown_, &QPushButton::clicked, this, [&]() {
    QListWidgetItem *citem = plotlegendlist_->currentItem();
    int crow = plotlegendlist_->currentRow();
    if (citem && crow < plotlegendlist_->count() - 1) {
      plotlegendlist_->takeItem(crow);
      plotlegendlist_->insertItem(crow + 1, citem);
      abstactvec_.swapItemsAt(crow, crow + 1);
      plotlegendlist_->setCurrentRow(crow + 1);
      // plotlegendlist_->itemSelectionChanged();
    } else
      qDebug() << "unable to move down";
  });
  connect(btnOK_, &QPushButton::clicked, this,
          &ArrangeLegend2D::rearrangeLegend2D);
  connect(btnCancel_, &QPushButton::clicked, this, &ArrangeLegend2D::close);

  abstactvec_.clear();
  bool once = true;
  for (int i = 0; i < legend_->itemCount(); i++) {
    // curve
    if (once)
      for (int j = 0; j < cvec_.count(); j++) {
        if (dynamic_cast<QCPAbstractLegendItem *>(legend_->item(i)) ==
            legend_->itemWithPlottable(cvec_.at(j))) {
          abstactvec_ << QPair<QCPAbstractLegendItem *, QString>(
              legend_->item(i), cvec_.at(j)->getlegendtext_cplot());
          once = false;
          break;
        }
      }
    // lsplot
    if (once)
      for (int j = 0; j < lsvec_.count(); j++) {
        if (dynamic_cast<QCPAbstractLegendItem *>(legend_->item(i)) ==
            legend_->itemWithPlottable(lsvec_.at(j))) {
          abstactvec_ << QPair<QCPAbstractLegendItem *, QString>(
              legend_->item(i), lsvec_.at(j)->getlegendtext_lsplot());
          once = false;
          break;
        }
      }
    // channelplot
    if (once)
      for (int j = 0; j < lspecialvec_.count(); j++) {
        if (dynamic_cast<QCPAbstractLegendItem *>(legend_->item(i)) ==
            legend_->itemWithPlottable(lspecialvec_.at(j)->getChannelFirst())) {
          abstactvec_ << QPair<QCPAbstractLegendItem *, QString>(
              legend_->item(i),
              lspecialvec_.at(j)->getChannelFirst()->getlegendtext_lsplot());
          once = false;
          break;
        }
      }
    // vectorplot
    if (once)
      for (int j = 0; j < vvec_.count(); j++) {
        if (dynamic_cast<QCPAbstractLegendItem *>(legend_->item(i)) ==
            legend_->itemWithPlottable(vvec_.at(j))) {
          plotlegendlist_->addItem(vvec_.at(j)->getlegendtext_vecplot());
          once = false;
          break;
        }
      }
    // barplot
    if (once)
      for (int j = 0; j < bvec_.count(); j++) {
        if (dynamic_cast<QCPAbstractLegendItem *>(legend_->item(i)) ==
            legend_->itemWithPlottable(bvec_.at(j))) {
          abstactvec_ << QPair<QCPAbstractLegendItem *, QString>(
              legend_->item(i), bvec_.at(j)->name());
          once = false;
          break;
        }
      }
    // sbplot
    if (once)
      for (int j = 0; j < svec_.count(); j++) {
        if (dynamic_cast<QCPAbstractLegendItem *>(legend_->item(i)) ==
            legend_->itemWithPlottable(svec_.at(j))) {
          abstactvec_ << QPair<QCPAbstractLegendItem *, QString>(
              legend_->item(i), svec_.at(j)->name());
          once = false;
          break;
        }
      }
    // colorplot
    if (once)
      for (int j = 0; j < covec_.count(); j++) {
        if (dynamic_cast<QCPAbstractLegendItem *>(legend_->item(i)) ==
            legend_->itemWithPlottable(covec_.at(j))) {
          abstactvec_ << QPair<QCPAbstractLegendItem *, QString>(
              legend_->item(i), covec_.at(j)->name());
          break;
        }
      }
    once = true;
  }

  for (int j = 0; j < abstactvec_.size(); j++) {
    plotlegendlist_->addItem(abstactvec_.at(j).second);
  }
}

ArrangeLegend2D::~ArrangeLegend2D() {}

void ArrangeLegend2D::rearrangeLegend2D() {
  for (int i = 0; i < abstactvec_.size(); i++)
    remakeLegendItem2D(abstactvec_.at(i).first);

  legend_->parentPlot()->replot();
  this->close();
}

void ArrangeLegend2D::remakeLegendItem2D(QCPAbstractLegendItem *item) {
  // curve
  for (int j = 0; j < cvec_.count(); j++) {
    if (item == legend_->itemWithPlottable(cvec_.at(j))) {
      QFont fnt = item->font();
      QColor clr = item->textColor();
      legend_->removeItem(item);
      if (cvec_.at(j)->getcurvetype_cplot() == Curve2D::Curve2DType::Spline) {
        SplineLegendItem2D *nitem =
            new SplineLegendItem2D(legend_, cvec_.at(j));
        nitem->setFont(fnt);
        nitem->setTextColor(clr);
        legend_->addItem(nitem);
      } else {
        LegendItem2D *nitem = new LegendItem2D(legend_, cvec_.at(j));
        nitem->setFont(fnt);
        nitem->setTextColor(clr);
        legend_->addItem(nitem);
      }
      return;
    }
  }
  // lsplot
  for (int j = 0; j < lsvec_.count(); j++) {
    if (item == legend_->itemWithPlottable(lsvec_.at(j))) {
      QFont fnt = item->font();
      QColor clr = item->textColor();
      legend_->removeItem(item);
      LegendItem2D *nitem = new LegendItem2D(legend_, lsvec_.at(j));
      nitem->setFont(fnt);
      nitem->setTextColor(clr);
      legend_->addItem(nitem);
      return;
    }
  }
  // channelplot
  for (int j = 0; j < lspecialvec_.count(); j++) {
    if (item ==
        legend_->itemWithPlottable(lspecialvec_.at(j)->getChannelFirst())) {
      QFont fnt = item->font();
      QColor clr = item->textColor();
      legend_->removeItem(item);
      LegendItem2D *nitem =
          new LegendItem2D(legend_, lspecialvec_.at(j)->getChannelFirst());
      nitem->setFont(fnt);
      nitem->setTextColor(clr);
      legend_->addItem(nitem);
      return;
    }
  }
  // vectorplot
  for (int j = 0; j < vvec_.count(); j++) {
    if (item == legend_->itemWithPlottable(vvec_.at(j))) {
      QFont fnt = item->font();
      QColor clr = item->textColor();
      legend_->removeItem(item);
      VectorLegendItem2D *nitem = new VectorLegendItem2D(legend_, vvec_.at(j));
      nitem->setFont(fnt);
      nitem->setTextColor(clr);
      legend_->addItem(nitem);
      return;
    }
  }
  // barplot
  for (int j = 0; j < bvec_.count(); j++) {
    if (item == legend_->itemWithPlottable(bvec_.at(j))) {
      QFont fnt = item->font();
      QColor clr = item->textColor();
      legend_->removeItem(item);
      LegendItem2D *nitem = new LegendItem2D(legend_, bvec_.at(j));
      nitem->setFont(fnt);
      nitem->setTextColor(clr);
      legend_->addItem(nitem);
      return;
    }
  }
  // sbplot
  for (int j = 0; j < svec_.count(); j++) {
    if (item == legend_->itemWithPlottable(svec_.at(j))) {
      QFont fnt = item->font();
      QColor clr = item->textColor();
      legend_->removeItem(item);
      LegendItem2D *nitem = new LegendItem2D(legend_, svec_.at(j));
      nitem->setFont(fnt);
      nitem->setTextColor(clr);
      legend_->addItem(nitem);
      return;
    }
  }
  // colorplot
  for (int j = 0; j < covec_.count(); j++) {
    if (item == legend_->itemWithPlottable(covec_.at(j))) {
      QFont fnt = item->font();
      QColor clr = item->textColor();
      legend_->removeItem(item);
      LegendItem2D *nitem = new LegendItem2D(legend_, covec_.at(j));
      nitem->setFont(fnt);
      nitem->setTextColor(clr);
      legend_->addItem(nitem);
      return;
    }
  }
}

QSize ArrangeLegend2D::sizeHint() const { return QSize(400, 400); }
