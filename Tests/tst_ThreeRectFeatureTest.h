#ifndef TST_THREERECTFEATURETEST_H
#define TST_THREERECTFEATURETEST_H

#include <QObject>

class ThreeRectFeatureTest : public QObject
{
    Q_OBJECT
public:
    explicit ThreeRectFeatureTest();
    
private Q_SLOTS:
    void testBasics();
    
};

#endif // TST_THREERECTFEATURETEST_H
