#include "Trainer.h"

#include "StrongClassifier.h"
#include "TwoRectFeature.h"
#include "ThreeRectFeature.h"
#include "FourRectFeature.h"

#include <cmath>
#include <limits>
#include <QMap>
#include <QMapIterator>

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

Trainer::Trainer(qreal maxFalsePositiveRatePerLayer,
                 qreal minTruePositiveRatePerLayer,
                 qreal overallFalsePositiveTarget,
                 const QList<IntegralImage>& positives,
                 const QList<IntegralImage>& negatives) :
    _maxF(maxFalsePositiveRatePerLayer),
    _goalD(minTruePositiveRatePerLayer),
    _goalF(overallFalsePositiveTarget),
    _P(positives),
    _N(negatives)
{

    qsrand(50);

    //Put some of our training data aside for validation purposes...
    const qreal percentage = 0.5;

    const int numPos = _P.size() * percentage;
    const int numNeg = _N.size() * percentage;

    for (int i = 0; i < numPos; i++)
        _validationP.append(_P.takeAt(qrand() % _P.size()));

    for (int i = 0; i < numNeg; i++)
        _validationN.append(_N.takeAt(qrand() % _N.size()));

    qDebug() << "Validation set sizes:" << _validationP.size() << _validationN.size();
}

QSharedPointer<ClassifierChain> Trainer::train()
{
    QSharedPointer<ClassifierChain> toRet(new ClassifierChain());

    QList<qreal> falsePositiveRateByLayer;
    QList<qreal> truePositiveRateByLayer;

    falsePositiveRateByLayer.append(1.0);
    truePositiveRateByLayer.append(1.0);

    QList<IntegralImage> N = _N;

    int i = 0;

    while (falsePositiveRateByLayer.last() > _goalF
           && !N.isEmpty())
    {
        falsePositiveRateByLayer.append(falsePositiveRateByLayer[i]);
        truePositiveRateByLayer.append(truePositiveRateByLayer[i]);
        i++;
        int numFeatures = 0;

        StrongClassifier * newLayer = 0;
        while (falsePositiveRateByLayer[i] > _maxF * falsePositiveRateByLayer[i-1]
               && !N.isEmpty())
        {
            if (newLayer != 0)
                toRet->removeClassifier(toRet->size()-1);
            numFeatures++;

            qDebug() << "Building classifier for layer" << i << "with" << numFeatures << "features";

            //Train a Classifier layer using adaboost that has numFeatures layers. Use the _P and N sets
            newLayer = this->adaboostTrain(numFeatures, _P, N);

            /*
             * Evaluate the entire cascaded classifier (including proposed new layer) using
             * validation set.
            */
            qreal Di, Fi;
            toRet->appendClassifier(newLayer);
            this->testValidation(toRet, &Di, &Fi);
            truePositiveRateByLayer[i] = Di;
            falsePositiveRateByLayer[i] = Fi;

            qDebug() << "Layer" << i << "D:" << Di << " F:" << Fi;


            /*
             * Decrease the threshold of the latest Classifier layer until the entire cascaded
             * classifier has a detection rate of at least _minTruePositiveRatePerLayer * lastTruePositiveRate.
            */
            int failCount = 0;
            while (truePositiveRateByLayer[i] < _goalD * truePositiveRateByLayer[i-1])
            {
                //newLayer->setAlphaThresh(newLayer->alphaThresh() - qAbs<qreal>(newLayer->alphaThresh()*0.1));
                for (int j = 0; j < numFeatures; j++)
                {
                    this->testValidation(toRet, &Di, &Fi);
                    truePositiveRateByLayer[i] = Di;
                    falsePositiveRateByLayer[i] = Fi;
                    if (truePositiveRateByLayer[i] >= _goalD * truePositiveRateByLayer[i-1])
                        break;

                    SimpleClassifier * toAdjust = newLayer->classifiers()[j];
                    qint64 delta = qMax<qint64>(10,qAbs<qint64>(toAdjust->threshold()) * 0.1) * toAdjust->polarity();
                    toAdjust->setThreshold(toAdjust->threshold() + delta);
                }

                this->testValidation(toRet, &Di, &Fi);
                truePositiveRateByLayer[i] = Di;
                falsePositiveRateByLayer[i] = Fi;

                qDebug() << "Layer" << i << "D:" << Di << " F:" << Fi;

                if (failCount++ == 100)
                {
                    qDebug() << "FAILED";
                    break;
                }
            }

        } //End inner loop

        //toRet->appendClassifier(newLayer);

        N.clear();

        if (falsePositiveRateByLayer[i] > _goalF)
        {
            //Evaluate cascade on entire _N set. Place all false positives in N for next layer to sort out.
            foreach(const IntegralImage& img, _N)
            {
                if (toRet->classify(img, QPoint(0,0), 1.0))
                    N.append(img);
            }
        }

    } //End main loop

    return toRet;
}

//private
StrongClassifier *Trainer::adaboostTrain(int numFeatures,
                                         const QList<IntegralImage> &positives,
                                         const QList<IntegralImage> &negatives)
{
    Q_ASSERT(!positives.isEmpty());
    Q_ASSERT(!negatives.isEmpty());

    if (numFeatures == 0)
        qWarning() << "Trainer training StrongClassifier with zero features... this is foolish";

    //Combine positives and negatives into one list
    QList<IntegralImage> examples;
    foreach(const IntegralImage& img, positives)
        examples.append(img);
    foreach(const IntegralImage& img, negatives)
        examples.append(img);

    //Initialize weights based on number of positive/negative examples
    QList<qreal> weights;
    for (int i = 0; i < examples.size(); i++)
    {
        qreal weight;
        if (i < positives.size())
            weight = 1.0 / (qreal)positives.size();
        else
            weight = 1.0 / (qreal)negatives.size();
        weights.append(weight);
    }

    StrongClassifier * toRet = new StrongClassifier();

    for (int t = 0; t < numFeatures; t++)
    {
        //Normalize the weights
        qreal sumOfWeights = 0.0;
        foreach(qreal weight, weights)
            sumOfWeights += weight;

        for (int i = 0; i < weights.size(); i++)
            weights[i] = weights[i] / sumOfWeights;

        //Select best weak classifier based on weighted error
        SimpleClassifier * simple = new SimpleClassifier(QSharedPointer<Feature>(), 1, 1);
        qreal weightedError = this->chooseBestFeature(simple, examples, positives.size(), weights);

        qreal beta = weightedError / (1.0 - weightedError);
        qreal alpha = log(1.0 / beta);

        toRet->appendClassifier(simple, alpha);

        //Update weights based on how simple classifier handles them
        for (int i = 0; i < weights.size(); i++)
        {
            int ei = 1;
            bool faceDetected = simple->classify(examples[i], QPoint(0,0), 1.0);
            if ((faceDetected && i < positives.size()) || (!faceDetected && i >= positives.size()))
                ei = 0;
            weights[i] = weights[i] * pow(beta, 1.0 - ei);
            Q_ASSERT(weights[i] >= 0.0);
        }

    }

    return toRet;
}

//private
qreal Trainer::chooseBestFeature(SimpleClassifier *classifierToMod,
                                 const QList<IntegralImage> &examples,
                                 int negativeStartIndex,
                                 const QList<qreal> &weights)
{
    QList<QSharedPointer<Feature> > features;
    //Generate all practical features and we'll try them out on our examples
    for (int y = 0; y < 22; y+=4)
    {
        for (int x = 0; x < 22; x+=4)
        {
            for (int w = 4; w < 23 - x; w+=4)
            {
                for (int h = 4; h < 23 - y; h+=4)
                {
                    QRect rect(x, y, w, h);
                    //Two-rect, both orientations
                    features.append(QSharedPointer<Feature>(new TwoRectFeature(rect,TwoRectFeature::HorizontalOrientation)));
                    features.append(QSharedPointer<Feature>(new TwoRectFeature(rect,TwoRectFeature::VerticalOrientation)));

                    //Three-rect, both orientations
                    if (w >= 6)
                        features.append(QSharedPointer<Feature>(new ThreeRectFeature(rect, ThreeRectFeature::HorizontalOrientation)));
                    if (h >= 6)
                        features.append(QSharedPointer<Feature>(new ThreeRectFeature(rect, ThreeRectFeature::VerticalOrientation)));

                    //Four-rect, only orientation
                    if (w >= 8 && h >= 8)
                        features.append(QSharedPointer<Feature>(new FourRectFeature(rect)));
                }
            }
        }
    }
    qDebug() << "Generated" << features.size() << "features";

    qreal totalNegativeWeights = 0.0;
    qreal totalPositiveWeights = 0.0;

    for (int i = 0; i < examples.size(); i++)
    {
        qreal weight = weights[i];
        if (i < negativeStartIndex)
            totalPositiveWeights += weight;
        else
            totalNegativeWeights += weight;
    }

    qreal bestWeightedError = std::numeric_limits<float>::max();
    QSharedPointer<Feature> bestFeature;
    qint64 bestThreshold = 0;
    int bestPolarity = 1;
    QSet<qint64> valueSet;
    foreach(const QSharedPointer<Feature>& feat, features)
    {
        QMap<qint64, qreal> positiveWeight;
        QMap<qint64, qreal> negativeWeight;

        for (int i = 0; i < examples.size(); i++)
        {
            qint64 val = feat->evaluate(examples[i], QPoint(0,0), 1.0);
            valueSet.insert(val);
            if (i < negativeStartIndex)
            {
                if (positiveWeight.contains(val))
                    positiveWeight[val] = positiveWeight[val] + weights[i];
                else
                    positiveWeight[val] = weights[i];
            }
            else
            {
                if (negativeWeight.contains(val))
                    negativeWeight[val] = negativeWeight[val] + weights[i];
                else
                    negativeWeight[val] = weights[i];
            }
        }

        QList<qint64> keys1 = positiveWeight.keys();
        QList<qint64> keys2 = negativeWeight.keys();
        QList<qint64> values;

        while (!keys1.isEmpty() || !keys2.isEmpty())
        {
            if (keys1.isEmpty())
                values.append(keys2.takeFirst());
            else if (keys2.isEmpty())
                values.append(keys1.takeFirst());
            else
            {
                qint64 k1 = keys1.first();
                qint64 k2 = keys2.first();
                if (k1 < k2)
                {
                    values.append(k1);
                    keys1.removeFirst();
                }
                else if (k1 > k2)
                {
                    values.append(k2);
                    keys2.removeFirst();
                }
                else
                {
                    values.append(k1);
                    keys1.removeFirst();
                    keys2.removeFirst();
                }
            }
        }

        qreal negativeWeightBelow = 0.0;
        qreal positiveWeightBelow = 0.0;

        foreach(qint64 value, values)
        {
            qreal errorCallingBelowNeg = positiveWeightBelow + totalNegativeWeights - negativeWeightBelow;
            qreal errorCallingBelowPos = negativeWeightBelow + totalPositiveWeights - positiveWeightBelow;

            Q_ASSERT(errorCallingBelowNeg >= 0.0);
            Q_ASSERT(errorCallingBelowPos >= 0.0);

            qreal weightedError;
            int polarity;
            if (errorCallingBelowNeg < errorCallingBelowPos)
            {
                weightedError = errorCallingBelowNeg;
                polarity = -1;
            }
            else
            {
                weightedError = errorCallingBelowPos;
                polarity = 1;
            }

            if (weightedError < bestWeightedError)
            {
                bestWeightedError = weightedError;
                bestFeature = feat;
                bestThreshold = value-1;
                bestPolarity = polarity;
            }

            if (positiveWeight.contains(value))
                positiveWeightBelow += positiveWeight[value];
            if (negativeWeight.contains(value))
                negativeWeightBelow += negativeWeight[value];
        }
    }

    qDebug() << "Best feature's error" << bestWeightedError << "polarity" << bestPolarity << "threshold" << bestThreshold;

    classifierToMod->setFeature(bestFeature);
    qDebug() << bestFeature->toString();
    classifierToMod->setPolarity(bestPolarity);
    classifierToMod->setThreshold(bestThreshold);

    //Choose the feature with the best weighted error accompanying its optimal threshold
    return bestWeightedError;
}

//private
void Trainer::testValidation(const QSharedPointer<ClassifierChain> &chain, qreal *trueRateOut, qreal *falseRateOut)
{
    if (chain.isNull() || trueRateOut == 0 || falseRateOut == 0)
    {
        qWarning() << "Trainer testValidation invalid (null) parameter.";
        return;
    }

    int correctDetections = 0;
    int falseDetections = 0;

    foreach(const IntegralImage& img, _validationP)
    {
        if (chain->classify(img, QPoint(0,0), 1.0))
            correctDetections++;
    }

    foreach(const IntegralImage& img, _validationN)
    {
        if (chain->classify(img, QPoint(0,0), 1.0))
            falseDetections++;
    }

    *trueRateOut = (qreal) correctDetections / (qreal) _validationP.size();
    *falseRateOut = (qreal) falseDetections / (qreal) _validationN.size();
}
