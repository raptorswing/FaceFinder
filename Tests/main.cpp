#include <QtCore>
#include <QtTest>
#include <QtDebug>

#include "tst_IntegralImageTest.h"
#include "tst_ClassifierChainTest.h"
#include "tst_TwoRectFeatureTest.h"
#include "tst_ThreeRectFeatureTest.h"

int main(int argc, char** argv)
{
    Q_UNUSED(argc)
    Q_UNUSED(argv)

    {
        IntegralImageTest test;
        QTest::qExec(&test);
    }

    {
        ClassifierChainTest test;
        QTest::qExec(&test);
    }

    {
        TwoRectFeatureTest test;
        QTest::qExec(&test);
    }

    {
        ThreeRectFeatureTest test;
        QTest::qExec(&test);
    }


}
