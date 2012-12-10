#include "Classifier.h"

Classifier::Classifier()
{
}

Classifier::~Classifier()
{
}

bool Classifier::classify(const IntegralImage &image, const QPoint &origin, qreal scale)
{
    Q_UNUSED(image)
    Q_UNUSED(origin)
    Q_UNUSED(scale)

    //I could have made this class pure-virtual but instead we'll just return false every time
    return false;
}
