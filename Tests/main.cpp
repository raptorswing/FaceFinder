#include <QtCore>
#include <QtTest>
#include <QtDebug>

#include "tst_IntegralImageTest.h"

int main(int argc, char** argv)
{
    Q_UNUSED(argc)
    Q_UNUSED(argv)

    {
        IntegralImageTest test;
        QTest::qExec(&test);
    }


}
