#include "Board.h"

Board::Board()
{
    init();
}

void Board::init()
{
    // �{�[�h��̑S�}�X���󂫃}�X�ɐݒ肷��. 
    for (int y = 1; y <= BOARD_SIZE; ++y)
    {
        for (int x = 1; x <= BOARD_SIZE; ++x)
        {
            rawBoard_[x][y] = EMPTY;
        }
    }

    // �ǂ�ݒ肷��. 
    for (int y = 0; y < BOARD_SIZE + 2; ++y)
    {
        rawBoard_[0][y] = WALL;
        rawBoard_[BOARD_SIZE + 1][y] = WALL;
    }

    for (int x = 0; x < BOARD_SIZE + 2; ++x)
    {
        rawBoard_[x][0] = WALL;
        rawBoard_[x][BOARD_SIZE + 1] = WALL;
    }

    // �΂̏����z�u��ݒ肷��. 
    rawBoard_[4][4] = WHITE;
    rawBoard_[5][5] = WHITE;
    rawBoard_[4][5] = BLACK;
    rawBoard_[5][4] = BLACK;

    // �΂̐��������ݒ�ɂ���. 
    Discs_[BLACK] = 2;
    Discs_[WHITE] = 2;
    Discs_[EMPTY] = BOARD_SIZE*BOARD_SIZE - Discs_[BLACK] - Discs_[WHITE];

    // �萔��0���琔����. 
    turns_ = 0;

    // ���͍�. 
    currentPlayerColor_ = BLACK;

    // update�͑S�č폜����. 
    updateLog_.clear();

    // �΂�u����ʒu������������. 
    initMovable();
}

bool Board::move(const Point& point)
{
    // �{�[�h�O�ɐ΂͒u���Ȃ�. 
    if (point.getX() < 0 || point.getX() >= BOARD_SIZE)
    {
        return false;
    }

    if (point.getY() < 0 || point.getY() >= BOARD_SIZE)
    {
        return false;
    }

    // �΂�u�������A�قȂ�F�̐΂����ނ��Ƃ��ł��Ȃ��ꍇ�A���̈ʒu�ɐ΂�u�����Ƃ͂ł��Ȃ�. 
    if (NONE == movableDir_[turns_][point.getX()][point.getY()])
    {
        return false;
    }

    // �����̐΂ŋ��񂾐΂��Ђ�����Ԃ�. 
    flipDiscs(point);

    // ����̃^�[���Ɉڂ�. 
    ++turns_;
    currentPlayerColor_ = -currentPlayerColor_;

    // �f�[�^�X�V����. 
    initMovable();

    return true;
}

bool Board::pass()
{
    // �ł��肪����ꍇ�A�p�X�͂ł��Ȃ�. 
    if (!movablePosition_[turns_].empty())
    {
        return false;
    }

    // �Q�[�����I�����Ă���ꍇ�A�p�X�ł��Ȃ�. 
    if (isGameOver())
    {
        return false;
    }

    // �p�X�ł���
    currentPlayerColor_ = -currentPlayerColor_;
    updateLog_.push_back(std::vector<Disc>());
    initMovable();

    return true;
}

bool Board::undo()
{
    // �Q�[���J�n���̏ꍇ�A�߂����Ƃ͂ł��Ȃ�. 
    if (0 == turns_)
    {
        return false;
    }

    // ���O�ɖ߂�. 
    currentPlayerColor_ = -currentPlayerColor_;
    const std::vector<Disc>& update = updateLog_.back();

    // �O�̃^�[���Ƀp�X�������ꍇ. 
    if (update.empty())
    {
        movablePosition_[turns_].clear();
        for (int x = 1; x <= BOARD_SIZE; ++x)
        {
            for (int y = 1; y <= BOARD_SIZE; ++y)
            {
                movableDir_[turns_][x][y] = NONE;
            }
        }
    }
    // �O�̃^�[���Ƀp�X�����Ȃ������ꍇ. 
    else
    {
        --turns_;

        // �{�[�h��̐΂�O�̏�Ԃɖ߂�. 
        rawBoard_[update.front().getX()][update.front().getY()] = EMPTY;
        for (int i = 1; i < update.size(); ++i)
        {
            rawBoard_[update.at(i).getX()][update.at(i).getY()] = -currentPlayerColor_;
        }

        // �{�[�h��̐΂̐����X�V����. 
        unsigned int discCount = static_cast<unsigned int>(update.size());
        Discs_[currentPlayerColor_] -= discCount;
        Discs_[-currentPlayerColor_] += discCount - 1;
        ++Discs_[EMPTY];
    }

    // �s�v�ɂȂ���update��1�폜����. 
    updateLog_.pop_back();

    return true;
}

bool Board::isGameOver() const
{
    // �ő�̎萔�ɒB���Ă�����Q�[���I��. 
    if (MAX_TURNS == turns_)
    {
        return true;
    }

    // �ł��肪����ꍇ�̓Q�[���I���ł͂Ȃ�. 
    if (!movablePosition_[turns_].empty())
    {
        return false;
    }

    // ���݂̃^�[���Ƌt�̐F�ɑł��肪���邩���肷��. 
    Disc disc;
    disc.setColor(-currentPlayerColor_);

    for (int x = 0; x < BOARD_SIZE; ++x)
    {
        disc.setX(x);
        for (int y = 0; y < BOARD_SIZE; ++y)
        {
            disc.setY(y);
            if (NONE != checkMobility(disc))
            {
                return false;
            }
        }
    }

    return true;
}

unsigned int Board::countDisc(Color color) const
{
    return Discs_[color];
}

Color Board::getColor(const Point& point) const
{
    return rawBoard_[point.getX()][point.getY()];
}

const std::vector<Point>& Board::getMovablePosition() const
{
    return movablePosition_[turns_];
}

std::vector<Disc> Board::getUpdate() const
{
    if (updateLog_.empty())
    {
        return std::vector<Disc>();
    }
    else
    {
        return updateLog_.back();
    }
}

Color Board::getCurrentColor() const
{
    return currentPlayerColor_;
}

unsigned int Board::getTurns() const
{
    return turns_;
}

void Board::flipDiscs(const Point& point)
{
    int x = 0;
    int y = 0;

    Disc operationDisc(point.getX(), point.getY(), currentPlayerColor_);

    int direction = movableDir_[turns_][point.getX()][point.getY()];

    std::vector<Disc> update;

    // point�Ŏw�肳�ꂽ�ʒu�Ɏ����̐΂�u��. 
    rawBoard_[point.getX()][point.getY()] = currentPlayerColor_;
    update.push_back(operationDisc);

    // �w�肳�ꂽ�ʒu�̏�����ɁA�Ђ�����Ԃ���΂����݂���. 
    if (UPPER & direction)
    {
        y = point.getY();
        operationDisc.setX(point.getX());

        // �����̐΂̐F�ŋ��񂾐΂��Ђ�����Ԃ�. 
        while (currentPlayerColor_ != rawBoard_[point.getX()][--y])
        {
            rawBoard_[point.getX()][y] = currentPlayerColor_;
            operationDisc.setY(y);
            update.push_back(operationDisc);
        }
    }

    // �w�肳�ꂽ�ʒu�̉������ɁA�Ђ�����Ԃ���΂����݂���. 
    if (LOWER & direction)
    {
        y = point.getY();
        operationDisc.setX(point.getX());

        // �����̐΂̐F�ŋ��񂾐΂��Ђ�����Ԃ�. 
        while (currentPlayerColor_ != rawBoard_[point.getX()][++y])
        {
            rawBoard_[point.getX()][y] = currentPlayerColor_;
            operationDisc.setY(y);
            update.push_back(operationDisc);
        }
    }

    // �w�肳�ꂽ�ʒu�̉E�����ɁA�Ђ�����Ԃ���΂����݂���. 
    if (RIGHT & direction)
    {
        x = point.getX();
        operationDisc.setY(point.getY());

        // �����̐΂̐F�ŋ��񂾐΂��Ђ�����Ԃ�. 
        while (currentPlayerColor_ != rawBoard_[++x][point.getY()])
        {
            rawBoard_[x][point.getY()] = currentPlayerColor_;
            operationDisc.setX(x);
            update.push_back(operationDisc);
        }
    }

    // �w�肳�ꂽ�ʒu�̍������ɁA�Ђ�����Ԃ���΂����݂���. 
    if (LEFT & direction)
    {
        x = point.getX();
        operationDisc.setY(point.getY());

        // �����̐΂̐F�ŋ��񂾐΂��Ђ�����Ԃ�. 
        while (currentPlayerColor_ != rawBoard_[--x][point.getY()])
        {
            rawBoard_[x][point.getY()] = currentPlayerColor_;
            operationDisc.setX(x);
            update.push_back(operationDisc);
        }
    }

    // �w�肳�ꂽ�ʒu�̉E������ɁA�Ђ�����Ԃ���΂����݂���. 
    if (UPPER_RIGHT & direction)
    {
        x = point.getX();
        y = point.getY();

        // �����̐΂̐F�ŋ��񂾐΂��Ђ�����Ԃ�. 
        while (currentPlayerColor_ != rawBoard_[++x][--y])
        {
            rawBoard_[x][y] = currentPlayerColor_;
            operationDisc.setX(x);
            operationDisc.setY(y);
            update.push_back(operationDisc);
        }
    }

    // �w�肳�ꂽ�ʒu�̉E�������ɁA�Ђ�����Ԃ���΂����݂���. 
    if (LOWER_RIGHT & direction)
    {
        x = point.getX();
        y = point.getY();

        // �����̐΂̐F�ŋ��񂾐΂��Ђ�����Ԃ�. 
        while (currentPlayerColor_ != rawBoard_[++x][++y])
        {
            rawBoard_[x][y] = currentPlayerColor_;
            operationDisc.setX(x);
            operationDisc.setY(y);
            update.push_back(operationDisc);
        }
    }

    // �w�肳�ꂽ�ʒu�̍�������ɁA�Ђ�����Ԃ���΂����݂���. 
    if (UPPER_LEFT & direction)
    {
        x = point.getX();
        y = point.getY();

        // �����̐΂̐F�ŋ��񂾐΂��Ђ�����Ԃ�. 
        while (currentPlayerColor_ != rawBoard_[--x][--y])
        {
            rawBoard_[x][y] = currentPlayerColor_;
            operationDisc.setX(x);
            operationDisc.setY(y);
            update.push_back(operationDisc);
        }
    }

    // �w�肳�ꂽ�ʒu�̍��������ɁA�Ђ�����Ԃ���΂����݂���. 
    if (LOWER_LEFT & direction)
    {
        x = point.getX();
        y = point.getY();

        // �����̐΂̐F�ŋ��񂾐΂��Ђ�����Ԃ�. 
        while (currentPlayerColor_ != rawBoard_[--x][++y])
        {
            rawBoard_[x][y] = currentPlayerColor_;
            operationDisc.setX(x);
            operationDisc.setY(y);
            update.push_back(operationDisc);
        }
    }
    
    // �{�[�h��̐΂̐����X�V����. 
    unsigned int flippedDiscCount = static_cast<unsigned int>(update.size());

    Discs_[currentPlayerColor_] += flippedDiscCount;
    Discs_[-currentPlayerColor_] -= flippedDiscCount - 1;
    --Discs_[EMPTY];

    updateLog_.push_back(update);
}

unsigned int Board::checkMobility(const Disc& disc) const
{
    // �w��ʒu�Ɋ��ɐ΂��u���Ă���ꍇ�A�����ɐ΂�u�����Ƃ͂ł��Ȃ�
    if (EMPTY != rawBoard_[disc.getX()][disc.getY()])
    {
        return NONE;
    }

    // �w��ʒu�ɐ΂�u���邩���肷��.
    unsigned int direction = 0;
    int x = 0;
    int y = 0;

    // �w��ʒu�̏�����ɒu���΂Ƌt�̐F�̐΂����݂��邩���肷��. 
    if (-disc.getColor() == rawBoard_[disc.getX()][disc.getY() - 1])
    {
        x = disc.getX();
        y = disc.getY() - 2;

        while (-disc.getColor() == rawBoard_[x][y])
        {
            --y;
        }
        if (disc.getColor() == rawBoard_[x][y])
        {
            direction |= UPPER;
        }
    }

    // �w��ʒu�̉������ɒu���΂Ƌt�̐F�̐΂����݂��邩���肷��. 
    if (-disc.getColor() == rawBoard_[disc.getX()][disc.getY() + 1])
    {
        x = disc.getX();
        y = disc.getY() + 2;

        while (-disc.getColor() == rawBoard_[x][y])
        {
            ++y;
        }
        if (disc.getColor() == rawBoard_[x][y])
        {
            direction |= LOWER;
        }
    }

    // �w��ʒu�̉E�����ɒu���΂Ƌt�̐F�̐΂����݂��邩���肷��. 
    if (-disc.getColor() == rawBoard_[disc.getX() + 1][disc.getY()])
    {
        x = disc.getX() + 2;
        y = disc.getY();

        while (-disc.getColor() == rawBoard_[x][y])
        {
            ++x;
        }
        if (disc.getColor() == rawBoard_[x][y])
        {
            direction |= RIGHT;
        }
    }

    // �w��ʒu�̍������ɒu���΂Ƌt�̐F�̐΂����݂��邩���肷��. 
    if (-disc.getColor() == rawBoard_[disc.getX() - 1][disc.getY()])
    {
        x = disc.getX() - 2;
        y = disc.getY();

        while (-disc.getColor() == rawBoard_[x][y])
        {
            --x;
        }
        if (disc.getColor() == rawBoard_[x][y])
        {
            direction |= LEFT;
        }
    }

    // �w��ʒu�̉E������ɒu���΂Ƌt�̐F�̐΂����݂��邩���肷��. 
    if (-disc.getColor() == rawBoard_[disc.getX() + 1][disc.getY() - 1])
    {
        x = disc.getX() + 2;
        y = disc.getY() - 2;

        while (-disc.getColor() == rawBoard_[x][y])
        {
            ++x;
            --y;
        }
        if (disc.getColor() == rawBoard_[x][y])
        {
            direction |= UPPER_RIGHT;
        }
    }

    // �w��ʒu�̍�������ɒu���΂Ƌt�̐F�̐΂����݂��邩���肷��. 
    if (-disc.getColor() == rawBoard_[disc.getX() - 1][disc.getY() - 1])
    {
        x = disc.getX() - 2;
        y = disc.getY() - 2;

        while (-disc.getColor() == rawBoard_[x][y])
        {
            --x;
            --y;
        }
        if (disc.getColor() == rawBoard_[x][y])
        {
            direction |= UPPER_LEFT;
        }
    }

    // �w��ʒu�̉E�������ɒu���΂Ƌt�̐F�̐΂����݂��邩���肷��. 
    if (-disc.getColor() == rawBoard_[disc.getX() + 1][disc.getY() + 1])
    {
        x = disc.getX() + 2;
        y = disc.getY() + 2;

        while (-disc.getColor() == rawBoard_[x][y])
        {
            ++x;
            ++y;
        }
        if (disc.getColor() == rawBoard_[x][y])
        {
            direction |= LOWER_RIGHT;
        }
    }

    // �w��ʒu�̍��������ɒu���΂Ƌt�̐F�̐΂����݂��邩���肷��. 
    if (-disc.getColor() == rawBoard_[disc.getX() - 1][disc.getY() + 1])
    {
        x = disc.getX() - 2;
        y = disc.getY() + 2;

        while (-disc.getColor() == rawBoard_[x][y])
        {
            --x;
            ++y;
        }
        if (disc.getColor() == rawBoard_[x][y])
        {
            direction |= LOWER_LEFT;
        }
    }

    return direction;
}

void Board::initMovable()
{
    Disc disc(0, 0, currentPlayerColor_);

    movablePosition_[turns_].clear();

    // �{�[�h���1�}�X1�}�X�ɑ΂��Đ΂��u���邩���肷��. 
    for (int x = 1; x <= BOARD_SIZE; ++x)
    {
        disc.setX(x);

        for (int y = 1; y <= BOARD_SIZE; ++y)
        {
            disc.setY(y);

            movableDir_[turns_][x][y] = checkMobility(disc);

            if (NONE != movableDir_[turns_][x][y])
            {
                movablePosition_[turns_].push_back(disc);
            }
        }
    }

    return;
}
