#include "Point.h"

typedef int Color;
static const Color EMPTY = 0;
static const Color WHITE = -1;
static const Color BLACK = 1;
static const Color WALL = 2;

// �F�ʂ̏����i�[���邽�߂̃N���X. 
template<typename T> class ColorStorage
{
public:
    T& operator[](Color color)
    {
        return data[color + 1];
    }

    const T& operator[](Color color) const
    {
        return data[color + 1];
    }

private:
    T data[3];
};

// �{�[�h�ɒu������. 
class Disc : public Point
{
public:
    Disc() : Point(0, 0)
    {
        color_ = EMPTY;
    }

    Disc(int x, int y, Color color) : Point(x, y)
    {
        color_ = color;
    }

    Color getColor() const;
    void setColor(const Color& color);

private:
    // �΂̐F. 
    Color color_;
};
