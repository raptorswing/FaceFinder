#ifndef INTEGRALIMAGE_H
#define INTEGRALIMAGE_H

#include <QtCore>

class IntegralImage
{
public:
    /**
     * @brief IntegralImage Constructs a null (isNull() returns true), uninitialized IntegralImage.
     */
    IntegralImage();

    /**
     * @brief IntegralImage constructs an IntegralImage from a filename / file path. If it succeeds
     * the image will be non-null (isNull() will return false). If it fails the image will be null.
     * The image will be converted to grayscale if it is colored.
     * @param filename
     */
    IntegralImage(const QString& filename);

    //Copy constructor. All data is copied.
    IntegralImage(const IntegralImage& other);

    ~IntegralImage();

    //Assignment operator. All data is copied.
    IntegralImage & operator =(const IntegralImage& other);

    bool operator ==(const IntegralImage& other) const;
    bool operator !=(const IntegralImage& other) const;

    /**
     * @brief isNull Returns true if this image is uninitialized
     * @return
     */
    bool isNull() const;

    /**
     * @brief sumAtPixel Returns the sum of all pixels above and to the left of the given pixel
     * inclusive.
     * @param pos
     * @return
     */
    qint64 sumAtPixel(const QPoint& pos) const;
    qint64 sumAtPixel(int x, int y) const;

    /**
     * @brief sumInArea Returns the sum of all pixels inside the given rectangle. Exclusive of the
     * right column and bottom row!
     * @param rect
     * @return
     */
    qint64 sumInArea(const QRect& rect) const;
    qint64 sumInArea(int x, int y, int width, int height) const;

    int width() const;
    int height() const;

private:
    void loadFromFile(const QString& filename);
    void setSumAtPixel(int x, int y, qint64 sum);

    //Member variables
    bool _isNull;
    qint64 * _bins;
    int _width;
    int _height;
};

#endif // INTEGRALIMAGE_H
