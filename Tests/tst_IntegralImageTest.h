#ifndef TST_INTEGRALIMAGETEST_H
#define TST_INTEGRALIMAGETEST_H

#include <QObject>

class IntegralImageTest : public QObject
{
    Q_OBJECT

public:
    IntegralImageTest();

private Q_SLOTS:
    void testSumAtPixel1();
    void testSumAtPixel2();

    void testSumInArea1();
    void testSumInArea2();

    void testCopyConstructor();
    void testAssignmentOperator();
};

#endif // TST_INTEGRALIMAGETEST_H
