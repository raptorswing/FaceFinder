#include "FourRectFeature.h"

FourRectFeature::FourRectFeature(const QRect &rect) :
    _rect(rect)
{
    if (_rect.top() < 0 || _rect.left() < 0 || _rect.right() >= 23 || _rect.bottom() >= 23)
    {
        qWarning() << "FourRectFeature can't be constructed with rect outside 23x23 rect at origin. Rect will be intersected to fit.";
        _rect = _rect.intersected(QRect(0,0,23,23));
    }
}

FourRectFeature::~FourRectFeature()
{
}


//Virtual from Feature
qint64 FourRectFeature::evaluate(const IntegralImage &image, const QPoint &origin, qreal scale)
{
    /*
     * We could optimize this method greatly. Do it later.
    */
    QPoint r1Origin = _rect.topLeft() + origin;

    int width = (_rect.width() + 1) * scale - 1;
    int height = (_rect.height() + 1) * scale - 1;

    width /= 2;
    height /= 2;

    QPoint r2Origin = r1Origin;
    QPoint r3Origin = r1Origin;
    QPoint r4Origin = r1Origin;

    r2Origin.setX(r2Origin.x() + width);
    r3Origin.setY(r3Origin.y() + height);
    r4Origin.setX(r4Origin.x() + width);
    r4Origin.setY(r4Origin.y() + height);

    qint64 sumR1 = image.sumInArea(r1Origin.x(), r1Origin.y(), width, height);
    qint64 sumR2 = image.sumInArea(r2Origin.x(), r2Origin.y(), width + 1, height + 1);
    qint64 sumR3 = image.sumInArea(r3Origin.x(), r3Origin.y(), width + 1, height + 1);
    qint64 sumR4 = image.sumInArea(r4Origin.x(), r4Origin.y(), width + 1, height + 1);

    //qDebug() << sumR1 << "+" << sumR4 << "-" << sumR3 << "-" << sumR2;

    return sumR1 + sumR4 - sumR2 - sumR3;
}

const QRect &FourRectFeature::rect() const
{
    return _rect;
}
