#include "TwoRectFeature.h"

TwoRectFeature::TwoRectFeature(const QRect &rect, TwoRectOrientation orientation) :
    _rect(rect), _orientation(orientation)
{
    if (_rect.top() < 0 || _rect.left() < 0 || _rect.right() >= 23 || _rect.bottom() >= 23)
    {
        qWarning() << "TwoRectFeature can't be constructed with rect outside 23x23 rect at origin. Rect will be intersected to fit.";
        _rect = _rect.intersected(QRect(0,0,23,23));
    }
}

TwoRectFeature::~TwoRectFeature()
{
}

//Virtual from Feature
qint64 TwoRectFeature::evaluate(const IntegralImage &image, const QPoint &origin, qreal scale)
{
    QPoint r1Origin = _rect.topLeft() + origin;

    int width = (_rect.width() + 1) * scale - 1;
    int height = (_rect.height() + 1) * scale - 1;

    QPoint r2Origin = r1Origin;
    if (_orientation == HorizontalOrientation)
    {
        width /= 2;
        r2Origin.setX(r2Origin.x() + width);
    }
    else
    {
        height /= 2;
        r2Origin.setY(r2Origin.y() + height);
    }

    qint64 r1Sum = image.sumInArea(r1Origin.x(), r1Origin.y(), width, height);
    qint64 r2Sum = image.sumInArea(r2Origin.x(), r2Origin.y(), width, height);

    return r1Sum - r2Sum;
}

//Virtual from Feature
QString TwoRectFeature::toString() const
{
    QString toRet = "TwoRect ";
    if (_orientation == HorizontalOrientation)
        toRet += "Horizontal ";
    else
        toRet += "Vertical ";
    toRet += QString::number(_rect.left()) + "," + QString::number(_rect.top()) + " - " + QString::number(_rect.width()) + "x" + QString::number(_rect.height());
    return toRet;
}

const QRect &TwoRectFeature::rect() const
{
    return _rect;
}

TwoRectFeature::TwoRectOrientation TwoRectFeature::orientation() const
{
    return _orientation;
}
