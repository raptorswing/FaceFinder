#include "Feature.h"

Feature::Feature()
{
}

Feature::~Feature()
{
}

qint64 Feature::evaluate(const IntegralImage &image, const QPoint &origin, qreal scale)
{
    Q_UNUSED(image)
    Q_UNUSED(origin)
    Q_UNUSED(scale)

    //Base feature returns zero. I could have made it pure-virtual instead.
    return 0;
}
