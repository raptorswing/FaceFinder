#ifndef TST_FOURRECTFEATURETEST_H
#define TST_FOURRECTFEATURETEST_H

#include <QObject>

class FourRectFeatureTest : public QObject
{
    Q_OBJECT
public:
    explicit FourRectFeatureTest();
    
private Q_SLOTS:
    void testBasics();
    
};

#endif // TST_FOURRECTFEATURETEST_H
