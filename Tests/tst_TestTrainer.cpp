#include "tst_TestTrainer.h"

#include <QtTest>
#include <QtDebug>
#include <QSharedPointer>

#include "Trainer.h"

class SwitchingClassifier : public Classifier
{
public:
    SwitchingClassifier(bool toRet) : _toRet(toRet) {}
    virtual ~SwitchingClassifier()
    {
    }

    virtual bool classify(const IntegralImage &image, const QPoint &origin, qreal scale)
    {
        Q_UNUSED(image)
        Q_UNUSED(origin)
        Q_UNUSED(scale)

        _toRet = !_toRet;
        return _toRet;
    }

private:
    bool _toRet;
};

TestTrainer::TestTrainer()
{
}

void TestTrainer::testChooseBestFeature()
{
    QList<IntegralImage> positives;
    QList<IntegralImage> negatives;
    QList<IntegralImage> examples;
    QList<qreal> weights;

    positives.append(IntegralImage(":/testImages/07.png"));
    positives.append(IntegralImage(":/testImages/07a.png"));
    positives.append(IntegralImage(":/testImages/07b.png"));
    positives.append(IntegralImage(":/testImages/07c.png"));
    positives.append(IntegralImage(":/testImages/07a.png"));
    positives.append(IntegralImage(":/testImages/07b.png"));
    positives.append(IntegralImage(":/testImages/07c.png"));
    positives.append(IntegralImage(":/testImages/07a.png"));

    negatives.append(IntegralImage(":/testImages/08.png"));
    negatives.append(IntegralImage(":/testImages/08a.png"));
    negatives.append(IntegralImage(":/testImages/08b.png"));
    negatives.append(IntegralImage(":/testImages/08c.png"));
    negatives.append(IntegralImage(":/testImages/08a.png"));
    negatives.append(IntegralImage(":/testImages/08b.png"));
    negatives.append(IntegralImage(":/testImages/08c.png"));
    negatives.append(IntegralImage(":/testImages/08a.png"));

    examples.append(IntegralImage(":/testImages/07.png"));
    examples.append(IntegralImage(":/testImages/07a.png"));
    examples.append(IntegralImage(":/testImages/07b.png"));
    examples.append(IntegralImage(":/testImages/07c.png"));
    examples.append(IntegralImage(":/testImages/08.png"));
    examples.append(IntegralImage(":/testImages/08a.png"));
    examples.append(IntegralImage(":/testImages/08b.png"));
    examples.append(IntegralImage(":/testImages/08c.png"));

    for (int i = 0; i < examples.size(); i++)
        weights.append(1.0);

    Trainer asdf(0.4, 0.9, 0.01,
                 positives, negatives);

    SimpleClassifier * simple = new SimpleClassifier(QSharedPointer<Feature>(),
                                                     1,
                                                     0);
    qreal weightedError = asdf.chooseBestFeature(simple, examples, examples.size() / 2, weights);
    qDebug() << "Resulting classifier has threshold" << simple->threshold() << "polarity" << simple->polarity() << "feature" << simple->feature()->toString();
}

//private slot
void TestTrainer::testTestValidation()
{
//    QList<IntegralImage> positives;
//    QList<IntegralImage> negatives;

//    positives.append(IntegralImage(":/testImages/07.png"));
//    positives.append(IntegralImage(":/testImages/07.png"));
//    positives.append(IntegralImage(":/testImages/07.png"));
//    positives.append(IntegralImage(":/testImages/07.png"));
//    positives.append(IntegralImage(":/testImages/07.png"));
//    positives.append(IntegralImage(":/testImages/07.png"));
//    positives.append(IntegralImage(":/testImages/07.png"));
//    positives.append(IntegralImage(":/testImages/07.png"));

//    negatives.append(IntegralImage(":/testImages/08.png"));
//    negatives.append(IntegralImage(":/testImages/08.png"));
//    negatives.append(IntegralImage(":/testImages/08.png"));
//    negatives.append(IntegralImage(":/testImages/08.png"));
//    negatives.append(IntegralImage(":/testImages/08.png"));
//    negatives.append(IntegralImage(":/testImages/08.png"));
//    negatives.append(IntegralImage(":/testImages/08.png"));
//    negatives.append(IntegralImage(":/testImages/08.png"));

//    Trainer asdf(0.4, 0.9, 0.01,
//                 positives, negatives);

//    QSharedPointer<ClassifierChain> chain(new ClassifierChain());
//    chain->appendClassifier(new SwitchingClassifier(true));

//    qreal dRate, fRate;
//    asdf.testValidation(chain, &dRate, &fRate);

//    qDebug() << dRate << fRate;
//    QVERIFY(dRate == 0.5);
//    QVERIFY(fRate == 0.5);
}
