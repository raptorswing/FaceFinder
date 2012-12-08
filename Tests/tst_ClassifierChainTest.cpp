#include "tst_ClassifierChainTest.h"

#include "ClassifierChain.h"

#include <QtTest>
#include <QtDebug>

//for use in tests
class TestClassifier : public Classifier
{
public:
    TestClassifier(int * destructionCounter) : _destructionCounter(destructionCounter)
    {}

    ~TestClassifier()
    {
        (*_destructionCounter)++;
    }

    virtual bool classify(const IntegralImage &image, const QPoint &origin, qreal scale)
    {
        Q_UNUSED(image)
        Q_UNUSED(origin)
        Q_UNUSED(scale)

        return true;
    }

private:
    int * _destructionCounter;
};

ClassifierChainTest::ClassifierChainTest()
{
}

//private slot
void ClassifierChainTest::testBasics()
{
    ClassifierChain chain;

    //Should be empty initially
    QVERIFY(chain.size() == 0);
    QVERIFY(chain.isEmpty());

    int destructionCounter = 0;

    //Size should be five after appending five classifiers
    for (int i = 0; i < 5; i++)
    {
        Classifier * testClassifier = new TestClassifier(&destructionCounter);
        QVERIFY(chain.appendClassifier(testClassifier));
    }
    QVERIFY(chain.size() == 5);
    QVERIFY(!chain.isEmpty());
    QVERIFY(destructionCounter == 0);

    //Should not be able to add null classifier
    QVERIFY(!chain.appendClassifier(0));

    //Shouldn't be able to remove classifiers from bad indices
    QVERIFY(!chain.removeClassifier(-1));
    QVERIFY(!chain.removeClassifier(50));

    //Size should be four after removing one from the middle
    QVERIFY(chain.removeClassifier(2));
    QVERIFY(chain.size() == 4);
    QVERIFY(destructionCounter == 1);

    //Size should be 0 after clearing
    chain.clear();
    QVERIFY(chain.size() == 0);
    QVERIFY(chain.isEmpty());
    QVERIFY(destructionCounter == 5);
}

//private slot
void ClassifierChainTest::testClassification()
{

    IntegralImage goodImage(":/testImages/averageMaleFace24x24.png");
    IntegralImage tooSmallImage(":/testImages/01.png");

    int destructionCounter = 0;

    ClassifierChain chain;

    //Add some of the "stock" Classifiers which always return false
    for (int i = 0; i < 5; i++)
        chain.appendClassifier(new Classifier());

    //Should return false even on a good image.
    QVERIFY(!chain.classify(goodImage, QPoint(0,0), 1.0));

    //remove all Classifiers
    chain.clear();

    //Add some of the TestClassifiers which always return true
    for (int i = 0; i < 5; i++)
        chain.appendClassifier(new TestClassifier(&destructionCounter));

    //Should return true on good image
    QVERIFY(chain.classify(goodImage, QPoint(0,0), 1.0));

    //Should return false if we add a "false-returning" classifier at the end of the chain
    chain.appendClassifier(new Classifier());
    QVERIFY(!chain.classify(goodImage, QPoint(0,0), 1.0));
    chain.removeClassifier(5);

    //Should return true on good image again since we removed "false-returner"
    QVERIFY(chain.classify(goodImage, QPoint(0,0), 1.0));

    //Should return false on image that is too small
    QVERIFY(!chain.classify(tooSmallImage, QPoint(0,0), 1.0));

    //Should return false on good image with bad bounds / scale
    QVERIFY(!chain.classify(goodImage, QPoint(-1,0), 1.0));
    QVERIFY(!chain.classify(goodImage, QPoint(0,-1), 1.0));
    QVERIFY(!chain.classify(goodImage, QPoint(0,0), 1.5));
    QVERIFY(!chain.classify(goodImage, QPoint(0,0), 0.5));
}
