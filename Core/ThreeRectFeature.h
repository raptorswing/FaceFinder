#ifndef THREERECTFEATURE_H
#define THREERECTFEATURE_H

#include "Feature.h"

class ThreeRectFeature : public Feature
{
public:
    enum ThreeRectOrientation
    {
        VerticalOrientation,
        HorizontalOrientation
    };

public:
    ThreeRectFeature(const QRect &rect, ThreeRectOrientation orientation);
    virtual ~ThreeRectFeature();

    //Virtual from Feature
    virtual qint64 evaluate(const IntegralImage& image,
                            const QPoint& origin,
                            qreal scale);

    const QRect& rect() const;

    ThreeRectOrientation orientation() const;

private:
    QRect _rect;
    ThreeRectOrientation _orientation;
};

#endif // THREERECTFEATURE_H
