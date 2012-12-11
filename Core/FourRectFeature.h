#ifndef FOURRECTFEATURE_H
#define FOURRECTFEATURE_H

#include "Feature.h"

class FourRectFeature : public Feature
{
public:
    FourRectFeature(const QRect &rect);
    virtual ~FourRectFeature();

    //Virtual from Feature
    virtual qint64 evaluate(const IntegralImage& image,
                            const QPoint& origin,
                            qreal scale);

    //Virtual from Feature
    virtual QString toString() const;

    const QRect& rect() const;

private:
    QRect _rect;
};

#endif // FOURRECTFEATURE_H
