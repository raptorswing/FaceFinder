#ifndef TST_TWORECTFEATURETEST_H
#define TST_TWORECTFEATURETEST_H

#include <QObject>

#include "TwoRectFeature.h"

class TwoRectFeatureTest : public QObject
{
    Q_OBJECT

public:
    explicit TwoRectFeatureTest();

private Q_SLOTS:
    void testBasics();

    
};

#endif // TST_TWORECTFEATURETEST_H
