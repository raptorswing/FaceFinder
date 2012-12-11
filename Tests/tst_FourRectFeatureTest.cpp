#include "tst_FourRectFeatureTest.h"

#include <QtTest>
#include <QtDebug>

#include "FourRectFeature.h"

FourRectFeatureTest::FourRectFeatureTest()
{
}

//private slot
void FourRectFeatureTest::testBasics()
{
    {
        IntegralImage img(":/testImages/09.png");

        FourRectFeature horizFeat(QRect(0,0,23,23));
        qint64 diff = horizFeat.evaluate(img, QPoint(0,0), 1.0);
        //qDebug() << diff;
        QVERIFY(diff == -2 * 11 * 12 * 255);
    }
}
