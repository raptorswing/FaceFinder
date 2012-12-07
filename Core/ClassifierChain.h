#ifndef CLASSIFIERCHAIN_H
#define CLASSIFIERCHAIN_H

#include "Classifier.h"
#include "IntegralImage.h"

#include <QList>

/**
 * @brief The ClassifierChain class represents an ordered series of Classifier objects.
 * When asked to classify a sample of an image (face or not?) the ClassifierChain goes to its first
 * Classifier. If the first Classifier says no, the sample is classified as "not a face." If the first
 * Classifier says yes, the question goes to the second Classifier and so on. If any say no, the sample is
 * rejected. If all say yes, it is accepted ("is a face").
 */
class ClassifierChain
{
public:
    ClassifierChain();
    ~ClassifierChain();

    /**
     * @brief appendClassifier adds a Classifier at the end of the chain. ClassifierChain takes ownership of
     * the Classifier given. You should NOT delete it manually. Returns true on success, false on failure.
     * @param nClass
     * @return
     */
    bool appendClassifier(Classifier * nClass);

    /**
     * @brief insertClassifier inserts a Classifier at the given index. If the index is less than zero or greater
     * than the current size of the chain the operation fails and false is returned. On success, true is returned
     * and ClassifierChain takes ownership of the pointer and you should NOT manually delete the Classifier.
     * @param index
     * @param nClass
     * @return
     */
    bool insertClassifier(int index, Classifier * nClass);

    /**
     * @brief removeClassifier removes the Classifier at the given index. The removed Classifier will be deleted.
     * Returns true on success, false on failure.
     * @param index
     * @return
     */
    bool removeClassifier(int index);

    /**
     * @brief size returns the number of Classifier objects in this ClassifierChain
     * @return
     */
    int size() const;

    /**
     * @brief isEmpty returns true if there are no Classifier objects in this ClassifierChain. Returns false
     * otherwise.
     * @return
     */
    bool isEmpty() const;

    /**
     * @brief classifiers returns a const reference to the complete list of Classifier objects in this
     * ClassifierChain
     * @return
     */
    const QList<Classifier *>& classifiers() const;

    /**
     * @brief clear removes all Classifier objects from this ClassifierChain. They will all be deleted.
     */
    void clear();

    /**
     * @brief classify uses the series of Classifier objects in this chain to decide whether or not a
     * given sample of the IntegralImage is a face or not. Returns true if a face, false otherwise.
     * Takes an origin and a scale to determine what part of the IntegralImage to look at. E.g. if the origin were
     * (1,1) and the scale was 1.0 then we would be looking at the rectangular portion of the IntegralImage from
     * (1,1) to (25,25). The scale is multiplied by (24,24).
     * @param image the IntegralImage that might contain a face
     * @param origin the origin of the sub-window of the IntegralImage that we are concerned with.
     * @param scale the scale (x * (24x24)) of the sub-window of the IntegralImage that we are concerned with.
     * @return
     */
    bool classify(const IntegralImage& image,
                  const QPoint& origin,
                  qreal scale);

private:
    QList<Classifier *> _classifiers;
};

#endif // CLASSIFIERCHAIN_H
