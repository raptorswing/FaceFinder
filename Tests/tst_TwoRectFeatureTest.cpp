#include "tst_TwoRectFeatureTest.h"

#include <QtDebug>
#include <QtTest>

#include "IntegralImage.h"

TwoRectFeatureTest::TwoRectFeatureTest()
{
}

void TwoRectFeatureTest::testBasics()
{
    {
        IntegralImage img(":/testImages/03.png");

        TwoRectFeature horizFeat(QRect(0,0,23,23), TwoRectFeature::HorizontalOrientation);
        qint64 horizDiff = horizFeat.evaluate(img, QPoint(0,0), 10.0);

        TwoRectFeature verticalFeat(QRect(0,0,23,23), TwoRectFeature::VerticalOrientation);
        qint64 verticalDiff = verticalFeat.evaluate(img, QPoint(0,0), 10.0);
        //qDebug() << horizDiff << verticalDiff;
        QVERIFY(horizDiff == -1 * 239 * 119 * 255);
        QVERIFY(verticalDiff == 0);
    }

    {
        IntegralImage img(":/testImages/04.png");

        TwoRectFeature horizFeat(QRect(0,0,23,23), TwoRectFeature::HorizontalOrientation);
        qint64 horizDiff = horizFeat.evaluate(img, QPoint(0,0), 10.0);

        TwoRectFeature verticalFeat(QRect(0,0,23,23), TwoRectFeature::VerticalOrientation);
        qint64 verticalDiff = verticalFeat.evaluate(img, QPoint(0,0), 10.0);
        //qDebug() << horizDiff << verticalDiff;
        QVERIFY(horizDiff == 0);
        QVERIFY(verticalDiff == 239 * 119 * 255);
    }
}
