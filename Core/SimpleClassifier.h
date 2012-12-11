#ifndef SIMPLECLASSIFIER_H
#define SIMPLECLASSIFIER_H

#include <QSharedPointer>

#include "Classifier.h"
#include "Feature.h"

/**
 * @brief The SimpleClassifier class represents a simple one-feature classifier. StrongClassifier objects are composed
 * of these. ClassifierChain object should be, in turn, composed of layers of StrongClassifier objects.
 */
class SimpleClassifier : public Classifier
{
public:
    /**
     * @brief SimpleClassifier constructs from a pointer to a feature, the polarity (-1 or 1), and
     * the threshold. The feature will evaluate instances based on whether or not
     * polarity*feature < polarity*threshold
     * @param feat
     * @param polarity must always be either zero (0) or one (1).
     * @param threshold
     */
    SimpleClassifier(const QSharedPointer<Feature>& feat, int polarity, qint64 threshold);
    virtual ~SimpleClassifier();

    //Copy Constructor
    SimpleClassifier(const SimpleClassifier& other);

    //Assignment Operator
    SimpleClassifier & operator =(const SimpleClassifier& other);

    //virtual from Classifier
    virtual bool classify(const IntegralImage& image,
                              const QPoint& origin,
                              qreal scale);

    const QSharedPointer<Feature>& feature() const;
    void setFeature(const QSharedPointer<Feature>& nFeat);

    int polarity() const;
    void setPolarity(int nPol);

    qint64 threshold() const;
    void setThreshold(qint64 nThresh);


private:
    QSharedPointer<Feature> _feature;
    int _polarity;
    qint64 _threshold;
};

#endif // SIMPLECLASSIFIER_H
