#include "ThreeRectFeature.h"

ThreeRectFeature::ThreeRectFeature(const QRect &rect, ThreeRectOrientation orientation) :
    _rect(rect), _orientation(orientation)
{
    if (_rect.top() < 0 || _rect.left() < 0 || _rect.right() >= 23 || _rect.bottom() >= 23)
    {
        qWarning() << "ThreeRectFeature can't be constructed with rect outside 23x23 rect at origin. Rect will be intersected to fit.";
        _rect = _rect.intersected(QRect(0,0,23,23));
    }
}

ThreeRectFeature::~ThreeRectFeature()
{
}

//Virtual from Feature
qint64 ThreeRectFeature::evaluate(const IntegralImage &image, const QPoint &origin, qreal scale)
{
    /*
     * This method could be optimized. We should just compute this once at initialization and then store it.
     * Do that later.
    */
    QPoint r1Origin = _rect.topLeft() + origin;

    int width = (_rect.width() + 1) * scale - 1;
    int height = (_rect.height() + 1) * scale - 1;

    QPoint r2Origin = r1Origin;
    QPoint r3Origin = r1Origin;
    if (_orientation == HorizontalOrientation)
    {
        width /= 3;
        r2Origin.setX(r2Origin.x() + width);
        r3Origin.setX(r3Origin.x() + 2 * width + 1);
    }
    else
    {
        height /= 3;
        r2Origin.setY(r2Origin.y() + height);
        r3Origin.setY(r3Origin.y() + 2 * height + 1);
    }

    qint64 r1Sum = image.sumInArea(r1Origin.x(), r1Origin.y(), width, height);
    qint64 r2Sum = image.sumInArea(r2Origin.x(), r2Origin.y(), width, height);
    qint64 r3Sum = image.sumInArea(r3Origin.x(), r3Origin.y(), width, height);

    return r2Sum - r1Sum - r3Sum;
}

//Virtual from Feature
QString ThreeRectFeature::toString() const
{
    QString toRet = "ThreeRect ";
    if (_orientation == HorizontalOrientation)
        toRet += "Horizontal ";
    else
        toRet += "Vertical ";
    toRet += QString::number(_rect.left()) + "," + QString::number(_rect.top()) + " - " + QString::number(_rect.width()) + "x" + QString::number(_rect.height());
    return toRet;
}

const QRect &ThreeRectFeature::rect() const
{
    return _rect;
}

ThreeRectFeature::ThreeRectOrientation ThreeRectFeature::orientation() const
{
    return _orientation;
}
