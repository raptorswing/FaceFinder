#ifndef TRAINER_H
#define TRAINER_H

#include <QtCore>
#include <QSet>
#include <QList>
#include <QSharedPointer>

#include "IntegralImage.h"
#include "ClassifierChain.h"
#include "SimpleClassifier.h"
#include "StrongClassifier.h"

class TestTrainer;

class Trainer
{
public:
    Trainer(qreal maxFalsePositiveRatePerLayer,
            qreal minTruePositiveRatePerLayer,
            qreal overallFalsePositiveTarget,
            const QList<IntegralImage>& positives,
            const QList<IntegralImage>& negatives,
            qreal validationSetPortion = 0.3);

    QSharedPointer<ClassifierChain> train();

    friend class TestTrainer;
private:
    StrongClassifier *adaboostTrain(int numFeatures,
                               const QList<IntegralImage>& positives,
                               const QList<IntegralImage>& negatives);
    qreal chooseBestFeature(SimpleClassifier * classifierToMod,
                            const QList<IntegralImage>& examples,
                            int negativeStartIndex,
                            const QList<qreal>& weights);
    void testValidation(const QSharedPointer<ClassifierChain>& chain, qreal * trueRateOut, qreal * falseRateOut);

    qreal _maxF;
    qreal _goalD;
    qreal _goalF;
    QList<IntegralImage> _P;
    QList<IntegralImage> _N;
    QList<IntegralImage> _validationP;
    QList<IntegralImage> _validationN;
};

#endif // TRAINER_H
