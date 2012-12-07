#include "IntegralImage.h"

#include <QImage>
#include <QtDebug>

//for memcpy
#include <cstring>

IntegralImage::IntegralImage() : _isNull(true), _bins(0), _width(0), _height(0)
{
}

IntegralImage::IntegralImage(const QString &filename) : _isNull(true), _bins(0), _width(0), _height(0)
{
    this->loadFromFile(filename);
}

IntegralImage::IntegralImage(const IntegralImage &other) : _isNull(true), _bins(0), _width(0), _height(0)
{
    if (!other.isNull())
    {
        _isNull = false;
        _width = other.width();
        _height = other.height();
        _bins = new quint64[_width * _height];
        memcpy(_bins, other._bins,_width * _height * sizeof(quint64));
    }
}

IntegralImage::~IntegralImage()
{
    if (!this->isNull())
    {
        delete _bins;
        _bins = 0;
    }
}

IntegralImage &IntegralImage::operator =(const IntegralImage &other)
{
    //Always check for self-assignment!
    if (&other == this)
        return *this;

    //Deallocate memory if we've got it
    if (!this->isNull())
        delete _bins;

    //Copy values from other object
    _isNull = other.isNull();
    _width = other.width();
    _height = other.height();

    if (!this->isNull())
    {
        _bins = new quint64[_width * _height];
        memcpy(_bins, other._bins, _width * _height * sizeof(quint64));
    }

    return *this;
}

bool IntegralImage::operator ==(const IntegralImage &other) const
{
    //Do the cheap checks first
    if (other.isNull() != this->isNull())
        return false;
    else if (other.width() != this->width() || other.height() != this->height())
        return false;
    else if (this->isNull())
        return true;

    //If we get to this point both images must be non-null and have the same dimensions.
    for (int y = 0; y < this->height(); y++)
    {
        for (int x = 0; x < this->width(); x++)
        {
            if (this->sumAtPixel(x,y) != other.sumAtPixel(x,y))
                return false;
        }
    }
    return true;
}

bool IntegralImage::operator !=(const IntegralImage &other) const
{
    return !(*this == other);
}

bool IntegralImage::isNull() const
{
    return _isNull;
}

quint64 IntegralImage::sumAtPixel(const QPoint &pos) const
{
    return this->sumAtPixel(pos.x(), pos.y());
}

quint64 IntegralImage::sumAtPixel(int x, int y) const
{
    if (x < 0 || y < 0 || x >= _width || y >= _height)
    {
        qWarning() << "IntegralImage sumAtPixel out of bounds:" << x << y;
        return 0;
    }
    return _bins[x + y * _width];
}

quint64 IntegralImage::sumInArea(const QRect &rect) const
{
    return this->sumInArea(rect.left(),
                           rect.top(),
                           rect.width(),
                           rect.height());
}

quint64 IntegralImage::sumInArea(int x, int y, int width, int height) const
{
    const quint64 bottomRight = this->sumAtPixel(x + width, y + height);
    const quint64 topLeft = this->sumAtPixel(x, y);
    const quint64 bottomLeft = this->sumAtPixel(x, y + height);
    const quint64 topRight = this->sumAtPixel(x + width, y);

    return bottomRight + topLeft - (bottomLeft + topRight);
}

int IntegralImage::width() const
{
    return _width;
}

int IntegralImage::height() const
{
    return _height;
}

//private
void IntegralImage::loadFromFile(const QString &filename)
{
    QImage qimage(filename);
    if (qimage.isNull())
    {
        qWarning() << "IntegralImage given bad filename:" << filename;
        return;
    }

    _width = qimage.width();
    _height = qimage.height();
    _isNull = false;

    if (!qimage.isGrayscale())
        qDebug() << "IntegralImage" << filename << "is not grayscale - will be converted";

    //Allocate memory for storing the sums
    _bins = new quint64[qimage.width() * qimage.height()];

    //Loop through the image and build the sums
    for (int y = 0; y < qimage.height(); y++)
    {
        quint64 rowSoFar = 0;

        for (int x = 0; x < qimage.width(); x++)
        {
            quint64 myVal = qGray(qimage.pixel(x,y));

            quint64 cellAboveCumulative = 0;
            if (y > 0)
                cellAboveCumulative = this->sumAtPixel(x, y-1);

            quint64 myCumulative = myVal + rowSoFar + cellAboveCumulative;
            this->setSumAtPixel(x,y,myCumulative);

            rowSoFar += myVal;
        }
    }
}

//private
void IntegralImage::setSumAtPixel(int x, int y, quint64 sum)
{
    _bins[x + y*_width] = sum;
}
