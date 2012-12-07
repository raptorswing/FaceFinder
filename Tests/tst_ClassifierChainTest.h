#ifndef TST_CLASSIFIERCHAINTEST_H
#define TST_CLASSIFIERCHAINTEST_H

#include <QObject>


class ClassifierChainTest : public QObject
{
    Q_OBJECT

public:
    ClassifierChainTest();

private Q_SLOTS:
    void testBasics();
    void testClassification();
    
};

#endif // TST_CLASSIFIERCHAINTEST_H
