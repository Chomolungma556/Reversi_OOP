#include "ConsoleBoard.h"

void ConsoleBoard::display()
{
    std::cout << "  a b c d e f g h " << std::endl;

    for (int y = 1; y <= BOARD_SIZE; ++y)
    {
        std::cout << " " << y;

        for (int x = 1; x <= BOARD_SIZE; ++x)
        {
            switch (getColor(Point(x, y)))
            {
            case BLACK:
                std::cout << "œ";
                break;
            case WHITE:
                std::cout << "Z";
                break;
            default:
                std::cout << "  ";
                break;
            }
        }
        std::cout << std::endl;
    }
}
