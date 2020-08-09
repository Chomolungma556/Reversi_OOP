#include <string>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <iosfwd>


// ç¿ïW. 
class Point
{
public:
    Point()
    {
        Point(0, 0);
    }

    Point(int x, int y)
    {
        x_ = x;
        y_ = y;
    }

    int getX() const;
    int getY() const;

    void setX(int x);
    void setY(int y);

private:
    int x_;
    int y_;
};
