#include "ClassifierChain.h"

#include <QtDebug>

ClassifierChain::ClassifierChain()
{
}

ClassifierChain::~ClassifierChain()
{
    this->clear();
}

bool ClassifierChain::appendClassifier(Classifier *nClass)
{
    if (nClass == 0)
    {
        qWarning() << "ClassifierChain can't append null Classifier.";
        return false;
    }

    _classifiers.append(nClass);
    return true;
}

bool ClassifierChain::insertClassifier(int index, Classifier *nClass)
{
    if (nClass == 0)
    {
        qWarning() << "ClassifierChain can't insert null Classifier.";
        return false;
    }

    if (index < 0 || index > _classifiers.size())
    {
        qWarning() << "ClassifierChain can't insert to invalid index" << index;
        return false;
    }

    _classifiers.insert(index, nClass);
    return true;
}

bool ClassifierChain::removeClassifier(int index)
{
    if (index < 0 || index >= _classifiers.size())
    {
        qWarning() << "ClassifierChain can't delete invalid index" << index;
        return false;
    }

    Classifier * toDel = _classifiers[index];
    delete toDel;

    _classifiers.removeAt(index);

    return true;
}

int ClassifierChain::size() const
{
    return _classifiers.size();
}

bool ClassifierChain::isEmpty() const
{
    return _classifiers.isEmpty();
}

const QList<Classifier *> &ClassifierChain::classifiers() const
{
    return _classifiers;
}

void ClassifierChain::clear()
{
    //Delete all classifiers and clear the list!
    foreach(Classifier * classifier, _classifiers)
        delete classifier;
    _classifiers.clear();
}

bool ClassifierChain::classify(const IntegralImage &image,
                               const QPoint &origin,
                               qreal scale)
{
    if (image.isNull())
    {
        qWarning() << "ClassifierChain cannot classify null image";
        return false;
    }

    if (origin.x() < 0 || origin.y() < 0)
    {
        qWarning() << "ClassifierChain can't classify with origin < (0,0)";
        return false;
    }

    QPoint lowerRight(origin.x() + 24 * scale,
                      origin.y() + 24 * scale);
    if (lowerRight.x() >= image.width() || lowerRight.y() >= image.height())
    {
        qWarning() << "ClassifierChain can't classify with an origin/scale that puts lowerRight beyond image bounds";
        return false;
    }

    //Try the image on the whole chain as long as it passes the predecessors
    foreach(Classifier * classifier, _classifiers)
    {
        if (!classifier->classify(image, origin, scale))
            return false;
    }

    return true;
}
