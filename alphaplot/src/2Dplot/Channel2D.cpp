#include "Channel2D.h"

#include "2Dplot/DataManager2D.h"
#include "2Dplot/LineSpecial2D.h"
#include "Table.h"
#include "core/IconLoader.h"
#include "core/Utilities.h"
#include "future/core/column/Column.h"

Channel2D::Channel2D(LineSpecial2D *ls1, LineSpecial2D *ls2)
    : QObject(ls1), ls1_(ls1), ls2_(ls2) {}

QString Channel2D::getItemName() {
  DataBlockGraph *data1 = ls1_->getdatablock_lsplot();
  DataBlockGraph *data2 = ls2_->getdatablock_lsplot();
  QString lschanneltext =
      data1->gettable()->name() + "_" + data1->getxcolumn()->name() + "_" +
      data1->getycolumn()->name() + "_" + data2->getycolumn()->name() + "[" +
      QString::number(data1->getfrom() + 1) + ":" +
      QString::number(data1->getto() + 1) + "]";
  return lschanneltext;
}

QIcon Channel2D::getItemIcon() {
  return IconLoader::load("graph2d-channel", IconLoader::LightDark);
}

QString Channel2D::getItemTooltip() {
  QString tooltip = Utilities::getTooltipText(Utilities::TooltipType::xyy);
  DataBlockGraph *data1 = ls1_->getdatablock_lsplot();
  DataBlockGraph *data2 = ls2_->getdatablock_lsplot();
  tooltip =
      tooltip.arg(data1->gettable()->name(), data1->getxcolumn()->name(),
                  data1->getycolumn()->name(), data2->getycolumn()->name(),
                  QString::number(data1->getfrom() + 1),
                  QString::number(data1->getto() + 1));
  return tooltip;
}
