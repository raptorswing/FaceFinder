#include "Trainer.h"

#include "StrongClassifier.h"
#include "TwoRectFeature.h"
#include "ThreeRectFeature.h"
#include "FourRectFeature.h"

#include <cmath>
#include <limits>
#include <QMap>
#include <QMapIterator>

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

    //Put some of our training data aside for validation purposes...
    const qreal percentage = 0.3;

    const int numPos = _P.size() * percentage;
    const int numNeg = _N.size() * percentage;

    for (int i = 0; i < numPos; i++)
        _validationP.append(_P.takeFirst());

    for (int i = 0; i < numNeg; i++)
        _validationN.append(_N.takeFirst());
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

    while (falsePositiveRateByLayer.last() > _goalF)
    {
        falsePositiveRateByLayer.append(falsePositiveRateByLayer[i]);
        truePositiveRateByLayer.append(truePositiveRateByLayer[i]);
        i++;
        int numFeatures = 0;

        StrongClassifier * newLayer = 0;
        while (falsePositiveRateByLayer[i] > _maxF * falsePositiveRateByLayer[i-1])
        {
            if (newLayer != 0)
                toRet->removeClassifier(toRet->size()-1);
            numFeatures++;
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


            /*
             * Decrease the threshold of the latest Classifier layer until the entire cascaded
             * classifier has a detection rate of at least _minTruePositiveRatePerLayer * lastTruePositiveRate.
            */
            while (truePositiveRateByLayer[i] < _goalD * truePositiveRateByLayer[i-1])
            {
                newLayer->setAlphaThresh(newLayer->alphaThresh()*0.95);
                this->testValidation(toRet, &Di, &Fi);
                truePositiveRateByLayer[i] = Di;
                falsePositiveRateByLayer[i] = Fi;
            }

        } //End inner loop

        toRet->appendClassifier(newLayer);

        N.clear();

        if (falsePositiveRateByLayer[i] > _goalF)
        {
            //Evaluate cascade on entire _N set. Place all false positives in N for next layer to sort out.
        }

    } //End main loop

    return toRet;
}

//private
StrongClassifier *Trainer::adaboostTrain(int numFeatures,
                                   const QList<IntegralImage> &positives,
                                   const QList<IntegralImage> &negatives)
{
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
    for (int y = 0; y < 22; y++)
    {
        for (int x = 0; x < 22; x++)
        {
            for (int w = 2; w < 23 - x; w++)
            {
                for (int h = 2; h < 23 - y; h++)
                {
                    QRect rect(x, y, w, h);
                    //Two-rect, both orientations
                    features.append(QSharedPointer<Feature>(new TwoRectFeature(rect,TwoRectFeature::HorizontalOrientation)));
                    features.append(QSharedPointer<Feature>(new TwoRectFeature(rect,TwoRectFeature::VerticalOrientation)));

                    //Three-rect, both orientations
                    features.append(QSharedPointer<Feature>(new ThreeRectFeature(rect, ThreeRectFeature::HorizontalOrientation)));
                    features.append(QSharedPointer<Feature>(new ThreeRectFeature(rect, ThreeRectFeature::VerticalOrientation)));

                    //Four-rect, only orientation
                    features.append(QSharedPointer<Feature>(new FourRectFeature(rect)));
                }
            }
        }
    }

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
    foreach(const QSharedPointer<Feature>& feat, features)
    {
        //Sort the examples by feature value
        QMap<qint64, int> featureValuesToExampleIndices;

        for (int i = 0; i < examples.size(); i++)
        {
            const IntegralImage& example = examples[i];
            qint64 val = feat->evaluate(example, QPoint(0,0), 1.0);
            featureValuesToExampleIndices.insert(val, i);
        }

        qreal negativeWeightsBelow = 0.0;
        qreal positiveWeightsBelow = 0.0;

        QMapIterator<qint64, int> iter(featureValuesToExampleIndices);
        while (iter.hasNext())
        {
            iter.next();
            qint64 featureVal = iter.key();
            int exampleIndex = iter.value();

            qreal errorMisclassifyPos = positiveWeightsBelow + (totalNegativeWeights - negativeWeightsBelow);
            qreal errorMisclassifyNeg = negativeWeightsBelow + (totalPositiveWeights - positiveWeightsBelow);

            qreal weightedError;
            int polarity;
            if (errorMisclassifyPos < errorMisclassifyNeg)
            {
                weightedError = errorMisclassifyPos;
                polarity = -1;
            }
            else
            {
                weightedError = errorMisclassifyNeg;
                polarity = 1;
            }

            qreal weight = weights[exampleIndex];
            if (exampleIndex >= negativeStartIndex)
                negativeWeightsBelow += weight;
            else
                positiveWeightsBelow += weight;

            if (weightedError < bestWeightedError)
            {
                bestWeightedError = weightedError;
                bestFeature = feat;
                bestThreshold = featureVal - 1;
                bestPolarity = polarity;
            }
        }
    }

    classifierToMod->setFeature(bestFeature);
    classifierToMod->setPolarity(bestPolarity); //WHAT DO I DO WITH POLARITY?
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
