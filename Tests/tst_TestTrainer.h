#ifndef TST_TESTTRAINER_H
#define TST_TESTTRAINER_H

#include <QObject>

class TestTrainer : public QObject
{
    Q_OBJECT
public:
    explicit TestTrainer();
    
private Q_SLOTS:
    void testChooseBestFeature();
    void testTestValidation();
    
};

#endif // TST_TESTTRAINER_H
