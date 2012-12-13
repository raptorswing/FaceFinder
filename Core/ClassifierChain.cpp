#include "ClassifierChain.h"

#include <QtDebug>

#include "TwoRectFeature.h"
#include "ThreeRectFeature.h"
#include "FourRectFeature.h"

ClassifierChain::ClassifierChain()
{
}

ClassifierChain::~ClassifierChain()
{
    this->clear();
}

bool ClassifierChain::appendClassifier(StrongClassifier *nClass)
{
    if (nClass == 0)
    {
        qWarning() << "ClassifierChain can't append null Classifier.";
        return false;
    }

    _classifiers.append(nClass);
    return true;
}

bool ClassifierChain::insertClassifier(int index, StrongClassifier *nClass)
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

    StrongClassifier * toDel = _classifiers[index];
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

const QList<StrongClassifier *> &ClassifierChain::classifiers() const
{
    return _classifiers;
}

void ClassifierChain::clear()
{
    //Delete all classifiers and clear the list!
    foreach(StrongClassifier * classifier, _classifiers)
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

    if (scale < 1.0)
    {
        qWarning() << "ClassifierChain can't classify with scale < 1.0";
        return false;
    }

    QPoint lowerRight(origin.x() + 24 * scale,
                      origin.y() + 24 * scale);
    if (lowerRight.x() > image.width() || lowerRight.y() > image.height())
    {
        qWarning() << "ClassifierChain can't classify with an origin/scale that puts lowerRight beyond image bounds";
        return false;
    }

    if (_classifiers.size() <= 0)
    {
        qWarning() << "ClassifierChain is emtpy. Returning false";
        return false;
    }

    //Try the image on the whole chain as long as it passes the predecessors
    int count = 0;
    foreach(StrongClassifier * classifier, _classifiers)
    {
        if (!classifier->classify(image, origin, scale))
        {
            return false;
        }
        count++;
    }

    return true;
}

QByteArray ClassifierChain::toBytes() const
{
    QByteArray toRet;

    foreach(StrongClassifier * classifier, _classifiers)
    {
        toRet.append("StrongClassifier\r\n");
        toRet.append("AlphaThresh " + QString::number(classifier->alphaThresh(),'g',12) + "\r\n");

        const QList<qreal>& alphaWeights = classifier->alphaWeights();
        const QList<SimpleClassifier *>& simples = classifier->classifiers();

        for (int i = 0; i < alphaWeights.size(); i++)
            toRet.append("Alpha " + QString::number(alphaWeights[i],'g',12) + "\r\n");

        for (int i = 0; i < simples.size(); i++)
        {
            SimpleClassifier * simple = simples[i];
            toRet.append("Simple " + QString::number(simple->polarity()) + " " + QString::number(simple->threshold()) + " " + simple->feature()->toString() + " " + "\r\n");
        }
    }


    return toRet;
}

ClassifierChain *ClassifierChain::fromBytes(const QByteArray &bytes)
{
    QList<QByteArray> lines = bytes.split('\n');

    ClassifierChain * work = new ClassifierChain();

    QList<qreal> tempAlphas;
    QList<SimpleClassifier *> tempSimples;

    StrongClassifier * current = 0;
    for (int l = 0; l < lines.size(); l++)
    {
        QByteArray line = lines[l];
        line.replace(QString("\r"), "");

        QList<QByteArray> parts = line.split(' ');
        if (parts.size() == 0)
            continue;

        if (parts[0] == "StrongClassifier")
        {
            if (current != 0)
            {
                for (int i = 0; i < tempAlphas.size(); i++)
                    current->appendClassifier(tempSimples[i], tempAlphas[i]);
                work->appendClassifier(current);
                tempAlphas.clear();
                tempSimples.clear();
            }
            current = new StrongClassifier();
        }
        else if (parts[0] == "AlphaThresh")
        {
            qreal alphaThresh = parts[1].toDouble();
            current->setAlphaThresh(alphaThresh);
        }
        else if (parts[0] == "Alpha")
        {
            qreal alpha = parts[1].toDouble();
            tempAlphas.append(alpha);
        }
        else if (parts[0] == "Simple")
        {
            SimpleClassifier * simple = new SimpleClassifier(QSharedPointer<Feature>(), 1, 1);

            int polarity = parts[1].toInt();
            qint64 threshold = parts[2].toLongLong();


            int x = parts[4].toInt();
            int y = parts[5].toInt();
            int w = parts[6].toInt();
            int h = parts[7].toInt();
            QRect rect(x,y,w,h);

            QSharedPointer<Feature> feat;
            if (parts[3] == "TwoRect")
            {
                TwoRectFeature::TwoRectOrientation orientation = TwoRectFeature::VerticalOrientation;
                if (parts[8] == "Horizontal")
                    orientation = TwoRectFeature::HorizontalOrientation;
                feat = QSharedPointer<Feature>(new TwoRectFeature(rect, orientation));
            }
            else if (parts[3] == "ThreeRect")
            {
                ThreeRectFeature::ThreeRectOrientation orientation = ThreeRectFeature::VerticalOrientation;
                if (parts[8] == "Horizontal")
                    orientation = ThreeRectFeature::VerticalOrientation;
                feat = QSharedPointer<Feature>(new ThreeRectFeature(rect, orientation));
            }
            else
                feat = QSharedPointer<Feature>(new FourRectFeature(rect));

            simple->setFeature(feat);
            simple->setPolarity(polarity);
            simple->setThreshold(threshold);

            tempSimples.append(simple);
        }
    }

    if (current != 0)
    {
        for (int i = 0; i < tempAlphas.size(); i++)
            current->appendClassifier(tempSimples[i], tempAlphas[i]);
        work->appendClassifier(current);
    }


    return work;
}

//private to prevent inadvertent use
ClassifierChain::ClassifierChain(const ClassifierChain &other)
{
    Q_UNUSED(other)
}

//private to prevent inadvertent use
ClassifierChain &ClassifierChain::operator =(const ClassifierChain &other)
{
    Q_UNUSED(other)
    return *this;
}
