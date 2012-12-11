#ifndef TWORECTFEATURE_H
#define TWORECTFEATURE_H

#include "Feature.h"

class TwoRectFeature : public Feature
{
public:
    enum TwoRectOrientation
    {
        VerticalOrientation,
        HorizontalOrientation
    };

public:
    TwoRectFeature(const QRect& rect, TwoRectOrientation orientation);
    virtual ~TwoRectFeature();

    //Virtual from Feature
    virtual qint64 evaluate(const IntegralImage& image,
                            const QPoint& origin,
                            qreal scale);

    //Virtual from Feature
    virtual QString toString() const;

    const QRect& rect() const;

    TwoRectOrientation orientation() const;

private:
    QRect _rect;
    TwoRectOrientation _orientation;
};

#endif // TWORECTFEATURE_H
