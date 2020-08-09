#include "ConsoleBoard.h"
#include <string>

int main()
{
    ConsoleBoard board;
    
    while (true)
    {
        board.display();

        std::cout << "Black Disc:" << board.countDisc(BLACK) << std::endl;
        std::cout << "White Disc:" << board.countDisc(WHITE) << std::endl;
        std::cout << "Empty Disc:" << board.countDisc(EMPTY) << std::endl;

        std::cout << std::endl;

        std::cout << "input..." << std::endl;

        Point p;
        std::string in;
        std::cin >> in;

        if ("p" == in)
        {
            if (!board.pass())
            {
                std::cerr << "can't pass!!" << std::endl;
            }
            continue;
        }

        if ("u" == in)
        {
            board.undo();
            continue;
        }

        if (in.length() >= 2)
        {
            p.setX(in[0] - 'a' + 1);
            p.setY(in[1] - '1' + 1);
        }
        else
        {
            std::cerr << "invalid input!!" << std::endl;
            continue;
        }

        if (false == board.move(p))
        {
            std::cerr << "can't put the disc there!!" << std::endl;
            continue;
        }

        if (board.isGameOver())
        {
            std::cout << "Game Over..." << std::endl;
            return 0;
        }
    }

    return 0;
}
