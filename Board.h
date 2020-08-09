#include "Disc.h"
#include <vector>

static const int BOARD_SIZE = 8;
static const int MAX_TURNS = 60;

class Board
{
public:
    Board();

    void init();
    bool move(const Point& point);
    bool pass();
    bool undo();
    bool isGameOver() const;

    unsigned int countDisc(Color color) const;
    Color getColor(const Point& point) const;
    const std::vector<Point>& getMovablePosition() const;
    std::vector<Disc> getUpdate() const;
    Color getCurrentColor() const;
    unsigned int getTurns() const;

private:
    void flipDiscs(const Point& point);
    unsigned int checkMobility(const Disc& disc) const;
    void initMovable();

    // 方向. 
    enum Direction
    {
        NONE        = 0,
        UPPER       = 1,
        UPPER_LEFT  = 2,
        LEFT        = 4,
        LOWER_LEFT  = 8,
        LOWER       = 16,
        LOWER_RIGHT = 32,
        RIGHT       = 64,
        UPPER_RIGHT = 128
    };

    // ボード上の石. 
    Color rawBoard_[BOARD_SIZE + 2][BOARD_SIZE + 2];

    // 手数. 
    unsigned int turns_;
    
    // 現在のプレイヤー. 
    Color currentPlayerColor_;

    // 打ち手の履歴. 
    std::vector<std::vector<Disc>> updateLog_;

    // 石を置ける位置. 
    std::vector<Point> movablePosition_[MAX_TURNS + 1];

    // 石をひっくり返せる方向. 
    unsigned int movableDir_[MAX_TURNS + 1][BOARD_SIZE + 2][BOARD_SIZE + 2];

    // 各色の石の数. 
    ColorStorage<unsigned int> Discs_;
};
