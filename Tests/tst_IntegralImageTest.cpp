#include "tst_IntegralImageTest.h"

#include <QString>
#include <QtTest>
#include <QtDebug>

#include "IntegralImage.h"

IntegralImageTest::IntegralImageTest()
{
}

//private slot
void IntegralImageTest::testSumAtPixel1()
{
    IntegralImage img(":/testImages/images/01.png");

    QVERIFY(img.sumAtPixel(4,4) == 9 * 255);
    QVERIFY(img.sumAtPixel(4,0) == 2 * 255);
    QVERIFY(img.sumAtPixel(0,4) == 2 * 255);
    QVERIFY(img.sumAtPixel(0,0) == 1 * 255);
    QVERIFY(img.sumAtPixel(2,2) == 3 * 255);
}

//private slot
void IntegralImageTest::testSumAtPixel2()
{
    IntegralImage img(":/testImages/images/02.png");

    QVERIFY(img.sumAtPixel(23,23) == 24 * 24 * 255);
    QVERIFY(img.sumAtPixel(23,0) == 24 * 255);
    QVERIFY(img.sumAtPixel(0,23) == 24 * 255);
    QVERIFY(img.sumAtPixel(0,0) == 1 * 255);
    QVERIFY(img.sumAtPixel(11,11) == 12 * 12 * 255);
}

//private slot
void IntegralImageTest::testSumInArea1()
{
    IntegralImage img(":/testImages/images/01.png");

    QVERIFY(img.sumInArea(0,0,0,0) == 0 * 255);
    QVERIFY(img.sumInArea(0,0,4,0) == 0 * 255);
    QVERIFY(img.sumInArea(0,0,0,4) == 0 * 255);
    QVERIFY(img.sumInArea(0,0,1,1) == 1 * 255);
    QVERIFY(img.sumInArea(0,0,4,4) == 6 * 255);
    QVERIFY(img.sumInArea(2,2,2,2) == 2 * 255);
    QVERIFY(img.sumInArea(0,2,4,2) == 3 * 255);
    QVERIFY(img.sumInArea(2,0,2,4) == 3 * 255);
}

//private slot
void IntegralImageTest::testSumInArea2()
{
    IntegralImage img(":/testImages/images/02.png");

    QVERIFY(img.sumInArea(0,0,0,0) == 0 * 255);
    QVERIFY(img.sumInArea(0,0,23,0) == 0 * 255);
    QVERIFY(img.sumInArea(0,0,0,23) == 0 * 255);
    QVERIFY(img.sumInArea(0,0,1,1) == 1 * 255);
    QVERIFY(img.sumInArea(0,0,23,23) == 23 * 23 * 255);
    QVERIFY(img.sumInArea(11,11,12,12) == 12 * 12 * 255);
}

//private slot
void IntegralImageTest::testCopyConstructor()
{
    IntegralImage * test = new IntegralImage(":/testImages/images/01.png");
    IntegralImage test2(*test);

    QVERIFY(!test->isNull());
    QVERIFY(!test2.isNull());

    QVERIFY(test->width() == test2.width());
    QVERIFY(test->height() == test2.height());

    bool good = true;
    for (int y = 0; y < test2.height() && good; y++)
    {
        for (int x = 0; x < test2.width() && good; x++)
        {
            if (test->sumAtPixel(x,y) != test2.sumAtPixel(x,y))
            {
                good = false;
                qDebug() << "Mismatch at" << x << y;
                qDebug() << test->sumAtPixel(x,y) << "vs." << test2.sumAtPixel(x,y);
                break;
            }
        }
    }
    QVERIFY(good);

    delete test;
}

//private slot
void IntegralImageTest::testAssignmentOperator()
{
    IntegralImage * test = new IntegralImage(":/testImages/images/01.png");
    IntegralImage test2;

    QVERIFY(test2.isNull());

    test2 = *test;

    QVERIFY(!test->isNull());
    QVERIFY(!test2.isNull());

    QVERIFY(test->width() == test2.width());
    QVERIFY(test->height() == test2.height());

    //Try a sneaky self-assignment
    test2 = test2;

    bool good = true;
    for (int y = 0; y < test2.height() && good; y++)
    {
        for (int x = 0; x < test2.width() && good; x++)
        {
            if (test->sumAtPixel(x,y) != test2.sumAtPixel(x,y))
            {
                good = false;
                qDebug() << "Mismatch at" << x << y;
                qDebug() << test->sumAtPixel(x,y) << "vs." << test2.sumAtPixel(x,y);
                break;
            }
        }
    }
    QVERIFY(good);
    delete test;
}
