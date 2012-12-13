#include "StrongClassifier.h"

#include <QtDebug>

StrongClassifier::StrongClassifier() :
    _alphaThresh(1.0)
{
}

StrongClassifier::~StrongClassifier()
{
    foreach(SimpleClassifier * classy, _classifiers)
        delete classy;
    _classifiers.clear();
}

//virtual from Classifier
bool StrongClassifier::classify(const IntegralImage &image,
                                const QPoint &origin,
                                qreal scale)
{
    if (_classifiers.isEmpty())
    {
        qWarning() << "StrongClassifier is empty --- classification will always return false";
        return false;
    }

    qreal sum = 0.0;
    for (int i = 0; i < _classifiers.size(); i++)
    {
        if (_classifiers[i]->classify(image, origin, scale))
            sum += _alphaWeights[i];
    }

    if (sum >= _alphaThresh)
        return true;
    return false;
}

bool StrongClassifier::appendClassifier(SimpleClassifier *classifier, qreal alphaWeight)
{
    _classifiers.append(classifier);
    _alphaWeights.append(alphaWeight);

    //Update "alpha threshold"
    qreal sum = 0.0;
    foreach(qreal alpha, _alphaWeights)
        sum += alpha;
    sum *= 0.5;
    _alphaThresh = sum;

    return true;
}

qreal StrongClassifier::alphaThresh() const
{
    return _alphaThresh;
}

void StrongClassifier::setAlphaThresh(qreal nAlpha)
{
    qDebug() << "New aThresh" << nAlpha;
    _alphaThresh = nAlpha;
}

const QList<SimpleClassifier *> &StrongClassifier::classifiers() const
{
    return _classifiers;
}

const QList<qreal> &StrongClassifier::alphaWeights() const
{
    return _alphaWeights;
}
