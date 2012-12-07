#ifndef CLASSIFIER_H
#define CLASSIFIER_H

#include "IntegralImage.h"

#include <QPoint>

class Classifier
{
public:
    Classifier();
    virtual ~Classifier();

    virtual bool classify(const IntegralImage& image,
                          const QPoint& origin,
                          qreal scale);
};

#endif // CLASSIFIER_H
