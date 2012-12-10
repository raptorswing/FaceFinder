#include "tst_ThreeRectFeatureTest.h"

#include <QtTest>

#include "IntegralImage.h"
#include "ThreeRectFeature.h"

ThreeRectFeatureTest::ThreeRectFeatureTest()
{
}

//private slot
void ThreeRectFeatureTest::testBasics()
{
    {
        IntegralImage img(":/testImages/05.png");

        ThreeRectFeature horizFeat(QRect(0,0,23,23), ThreeRectFeature::HorizontalOrientation);
        qint64 horizDiff = horizFeat.evaluate(img, QPoint(0,0), 10.0);

        ThreeRectFeature verticalFeat(QRect(0,0,23,23), ThreeRectFeature::VerticalOrientation);
        qint64 verticalDiff = verticalFeat.evaluate(img, QPoint(0,0), 10.0);
        //qDebug() << horizDiff << verticalDiff;
        QVERIFY(horizDiff == -1 * 80 * 79 * 255);
        QVERIFY(verticalDiff == 239 * 79 * 255);
    }

    {
        IntegralImage img(":/testImages/06.png");

        ThreeRectFeature horizFeat(QRect(0,0,23,23), ThreeRectFeature::HorizontalOrientation);
        qint64 horizDiff = horizFeat.evaluate(img, QPoint(0,0), 10.0);

        ThreeRectFeature verticalFeat(QRect(0,0,23,23), ThreeRectFeature::VerticalOrientation);
        qint64 verticalDiff = verticalFeat.evaluate(img, QPoint(0,0), 10.0);
        //qDebug() << horizDiff << verticalDiff;
        QVERIFY(verticalDiff == -1 * 80 * 79 * 255);
        QVERIFY(horizDiff == 239 * 79 * 255);
    }


    {
        IntegralImage img(":/testImages/07.png");

        ThreeRectFeature horizFeat(QRect(0,0,23,23), ThreeRectFeature::HorizontalOrientation);
        qint64 horizDiff = horizFeat.evaluate(img, QPoint(0,0), 1.0);

        ThreeRectFeature verticalFeat(QRect(0,0,23,23), ThreeRectFeature::VerticalOrientation);
        qint64 verticalDiff = verticalFeat.evaluate(img, QPoint(0,0), 1.0);
        //qDebug() << horizDiff << verticalDiff;
        QVERIFY(horizDiff == -1 * 8 * 7 * 255);
        QVERIFY(verticalDiff == 23 * 7 * 255);
    }

    {
        IntegralImage img(":/testImages/08.png");

        ThreeRectFeature horizFeat(QRect(0,0,23,23), ThreeRectFeature::HorizontalOrientation);
        qint64 horizDiff = horizFeat.evaluate(img, QPoint(0,0), 1.0);

        ThreeRectFeature verticalFeat(QRect(0,0,23,23), ThreeRectFeature::VerticalOrientation);
        qint64 verticalDiff = verticalFeat.evaluate(img, QPoint(0,0), 1.0);
        //qDebug() << horizDiff << verticalDiff;
        QVERIFY(verticalDiff == -1 * 8 * 7 * 255);
        QVERIFY(horizDiff == 23 * 7 * 255);
    }
}
