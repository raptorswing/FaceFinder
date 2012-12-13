#ifndef STRONGCLASSIFIER_H
#define STRONGCLASSIFIER_H

#include "Classifier.h"
#include "SimpleClassifier.h"

#include <QList>

class StrongClassifier : public Classifier
{
public:
    StrongClassifier();
    virtual ~StrongClassifier();

    //virtual from Classifier
    virtual bool classify(const IntegralImage& image,
                              const QPoint& origin,
                              qreal scale);

    bool appendClassifier(SimpleClassifier * classifier, qreal alphaWeight);

    qreal alphaThresh() const;
    void setAlphaThresh(qreal nAlpha);

    const QList<SimpleClassifier *>& classifiers() const;
    const QList<qreal>& alphaWeights() const;

private:
    QList<SimpleClassifier *> _classifiers;
    QList<qreal> _alphaWeights;

    qreal _alphaThresh;
};

#endif // STRONGCLASSIFIER_H
