#ifndef CHANNEL2D_H
#define CHANNEL2D_H

#include <QObject>

class LineSpecial2D;

class Channel2D : public QObject {
  Q_OBJECT
 public:
  explicit Channel2D(LineSpecial2D *ls1, LineSpecial2D *ls2);

  QString getItemName();
  QIcon getItemIcon();
  QString getItemTooltip();

  LineSpecial2D *getChannelFirst() { return ls1_; }
  LineSpecial2D *getChannelSecond() { return ls2_; }
  QPair<LineSpecial2D *, LineSpecial2D *> getChannelPair() {
    return QPair<LineSpecial2D *, LineSpecial2D *>(ls1_, ls2_);
  }

 private:
  LineSpecial2D *ls1_;
  LineSpecial2D *ls2_;
};

Q_DECLARE_METATYPE(Channel2D *);
#endif  // CHANNEL2D_H
