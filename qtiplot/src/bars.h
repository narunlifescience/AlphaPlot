#ifndef BARS_H
#define BARS_H

#include <qwt3d_plot.h>

class Bar : public Qwt3D::VertexEnrichment
{
public:
  Bar();
  Bar(double rad);

  Qwt3D::Enrichment* clone() const {return new Bar(*this);}
  
  void configure(double rad);
  void drawBegin();
  void drawEnd();
  void draw(Qwt3D::Triple const&);

private:
  double radius_;
  double diag_;
};


#endif
