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

    // ����. 
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

    // �{�[�h��̐�. 
    Color rawBoard_[BOARD_SIZE + 2][BOARD_SIZE + 2];

    // �萔. 
    unsigned int turns_;
    
    // ���݂̃v���C���[. 
    Color currentPlayerColor_;

    // �ł���̗���. 
    std::vector<std::vector<Disc>> updateLog_;

    // �΂�u����ʒu. 
    std::vector<Point> movablePosition_[MAX_TURNS + 1];

    // �΂��Ђ�����Ԃ������. 
    unsigned int movableDir_[MAX_TURNS + 1][BOARD_SIZE + 2][BOARD_SIZE + 2];

    // �e�F�̐΂̐�. 
    ColorStorage<unsigned int> Discs_;
};
