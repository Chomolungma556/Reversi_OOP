#include "Board.h"

Board::Board()
{
    init();
}

void Board::init()
{
    // ボード上の全マスを空きマスに設定する. 
    for (int y = 1; y <= BOARD_SIZE; ++y)
    {
        for (int x = 1; x <= BOARD_SIZE; ++x)
        {
            rawBoard_[x][y] = EMPTY;
        }
    }

    // 壁を設定する. 
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

    // 石の初期配置を設定する. 
    rawBoard_[4][4] = WHITE;
    rawBoard_[5][5] = WHITE;
    rawBoard_[4][5] = BLACK;
    rawBoard_[5][4] = BLACK;

    // 石の数を初期設定にする. 
    Discs_[BLACK] = 2;
    Discs_[WHITE] = 2;
    Discs_[EMPTY] = BOARD_SIZE*BOARD_SIZE - Discs_[BLACK] - Discs_[WHITE];

    // 手数は0から数える. 
    turns_ = 0;

    // 先手は黒. 
    currentPlayerColor_ = BLACK;

    // updateは全て削除する. 
    updateLog_.clear();

    // 石を置ける位置を初期化する. 
    initMovable();
}

bool Board::move(const Point& point)
{
    // ボード外に石は置けない. 
    if (point.getX() < 0 || point.getX() >= BOARD_SIZE)
    {
        return false;
    }

    if (point.getY() < 0 || point.getY() >= BOARD_SIZE)
    {
        return false;
    }

    // 石を置いた時、異なる色の石を挟むことができない場合、その位置に石を置くことはできない. 
    if (NONE == movableDir_[turns_][point.getX()][point.getY()])
    {
        return false;
    }

    // 自分の石で挟んだ石をひっくり返す. 
    flipDiscs(point);

    // 相手のターンに移る. 
    ++turns_;
    currentPlayerColor_ = -currentPlayerColor_;

    // データ更新処理. 
    initMovable();

    return true;
}

bool Board::pass()
{
    // 打ち手がある場合、パスはできない. 
    if (!movablePosition_[turns_].empty())
    {
        return false;
    }

    // ゲームが終了している場合、パスできない. 
    if (isGameOver())
    {
        return false;
    }

    // パスできる
    currentPlayerColor_ = -currentPlayerColor_;
    updateLog_.push_back(std::vector<Disc>());
    initMovable();

    return true;
}

bool Board::undo()
{
    // ゲーム開始時の場合、戻すことはできない. 
    if (0 == turns_)
    {
        return false;
    }

    // 一手前に戻す. 
    currentPlayerColor_ = -currentPlayerColor_;
    const std::vector<Disc>& update = updateLog_.back();

    // 前のターンにパスをした場合. 
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
    // 前のターンにパスをしなかった場合. 
    else
    {
        --turns_;

        // ボード上の石を前の状態に戻す. 
        rawBoard_[update.front().getX()][update.front().getY()] = EMPTY;
        for (int i = 1; i < update.size(); ++i)
        {
            rawBoard_[update.at(i).getX()][update.at(i).getY()] = -currentPlayerColor_;
        }

        // ボード上の石の数を更新する. 
        unsigned int discCount = static_cast<unsigned int>(update.size());
        Discs_[currentPlayerColor_] -= discCount;
        Discs_[-currentPlayerColor_] += discCount - 1;
        ++Discs_[EMPTY];
    }

    // 不要になったupdateを1つ削除する. 
    updateLog_.pop_back();

    return true;
}

bool Board::isGameOver() const
{
    // 最大の手数に達していたらゲーム終了. 
    if (MAX_TURNS == turns_)
    {
        return true;
    }

    // 打ち手がある場合はゲーム終了ではない. 
    if (!movablePosition_[turns_].empty())
    {
        return false;
    }

    // 現在のターンと逆の色に打ち手があるか判定する. 
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

    // pointで指定された位置に自分の石を置く. 
    rawBoard_[point.getX()][point.getY()] = currentPlayerColor_;
    update.push_back(operationDisc);

    // 指定された位置の上方向に、ひっくり返せる石が存在する. 
    if (UPPER & direction)
    {
        y = point.getY();
        operationDisc.setX(point.getX());

        // 自分の石の色で挟んだ石をひっくり返す. 
        while (currentPlayerColor_ != rawBoard_[point.getX()][--y])
        {
            rawBoard_[point.getX()][y] = currentPlayerColor_;
            operationDisc.setY(y);
            update.push_back(operationDisc);
        }
    }

    // 指定された位置の下方向に、ひっくり返せる石が存在する. 
    if (LOWER & direction)
    {
        y = point.getY();
        operationDisc.setX(point.getX());

        // 自分の石の色で挟んだ石をひっくり返す. 
        while (currentPlayerColor_ != rawBoard_[point.getX()][++y])
        {
            rawBoard_[point.getX()][y] = currentPlayerColor_;
            operationDisc.setY(y);
            update.push_back(operationDisc);
        }
    }

    // 指定された位置の右方向に、ひっくり返せる石が存在する. 
    if (RIGHT & direction)
    {
        x = point.getX();
        operationDisc.setY(point.getY());

        // 自分の石の色で挟んだ石をひっくり返す. 
        while (currentPlayerColor_ != rawBoard_[++x][point.getY()])
        {
            rawBoard_[x][point.getY()] = currentPlayerColor_;
            operationDisc.setX(x);
            update.push_back(operationDisc);
        }
    }

    // 指定された位置の左方向に、ひっくり返せる石が存在する. 
    if (LEFT & direction)
    {
        x = point.getX();
        operationDisc.setY(point.getY());

        // 自分の石の色で挟んだ石をひっくり返す. 
        while (currentPlayerColor_ != rawBoard_[--x][point.getY()])
        {
            rawBoard_[x][point.getY()] = currentPlayerColor_;
            operationDisc.setX(x);
            update.push_back(operationDisc);
        }
    }

    // 指定された位置の右上方向に、ひっくり返せる石が存在する. 
    if (UPPER_RIGHT & direction)
    {
        x = point.getX();
        y = point.getY();

        // 自分の石の色で挟んだ石をひっくり返す. 
        while (currentPlayerColor_ != rawBoard_[++x][--y])
        {
            rawBoard_[x][y] = currentPlayerColor_;
            operationDisc.setX(x);
            operationDisc.setY(y);
            update.push_back(operationDisc);
        }
    }

    // 指定された位置の右下方向に、ひっくり返せる石が存在する. 
    if (LOWER_RIGHT & direction)
    {
        x = point.getX();
        y = point.getY();

        // 自分の石の色で挟んだ石をひっくり返す. 
        while (currentPlayerColor_ != rawBoard_[++x][++y])
        {
            rawBoard_[x][y] = currentPlayerColor_;
            operationDisc.setX(x);
            operationDisc.setY(y);
            update.push_back(operationDisc);
        }
    }

    // 指定された位置の左上方向に、ひっくり返せる石が存在する. 
    if (UPPER_LEFT & direction)
    {
        x = point.getX();
        y = point.getY();

        // 自分の石の色で挟んだ石をひっくり返す. 
        while (currentPlayerColor_ != rawBoard_[--x][--y])
        {
            rawBoard_[x][y] = currentPlayerColor_;
            operationDisc.setX(x);
            operationDisc.setY(y);
            update.push_back(operationDisc);
        }
    }

    // 指定された位置の左下方向に、ひっくり返せる石が存在する. 
    if (LOWER_LEFT & direction)
    {
        x = point.getX();
        y = point.getY();

        // 自分の石の色で挟んだ石をひっくり返す. 
        while (currentPlayerColor_ != rawBoard_[--x][++y])
        {
            rawBoard_[x][y] = currentPlayerColor_;
            operationDisc.setX(x);
            operationDisc.setY(y);
            update.push_back(operationDisc);
        }
    }
    
    // ボード上の石の数を更新する. 
    unsigned int flippedDiscCount = static_cast<unsigned int>(update.size());

    Discs_[currentPlayerColor_] += flippedDiscCount;
    Discs_[-currentPlayerColor_] -= flippedDiscCount - 1;
    --Discs_[EMPTY];

    updateLog_.push_back(update);
}

unsigned int Board::checkMobility(const Disc& disc) const
{
    // 指定位置に既に石が置いてある場合、そこに石を置くことはできない
    if (EMPTY != rawBoard_[disc.getX()][disc.getY()])
    {
        return NONE;
    }

    // 指定位置に石を置けるか判定する.
    unsigned int direction = 0;
    int x = 0;
    int y = 0;

    // 指定位置の上方向に置く石と逆の色の石が存在するか判定する. 
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

    // 指定位置の下方向に置く石と逆の色の石が存在するか判定する. 
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

    // 指定位置の右方向に置く石と逆の色の石が存在するか判定する. 
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

    // 指定位置の左方向に置く石と逆の色の石が存在するか判定する. 
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

    // 指定位置の右上方向に置く石と逆の色の石が存在するか判定する. 
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

    // 指定位置の左上方向に置く石と逆の色の石が存在するか判定する. 
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

    // 指定位置の右下方向に置く石と逆の色の石が存在するか判定する. 
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

    // 指定位置の左下方向に置く石と逆の色の石が存在するか判定する. 
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

    // ボード上の1マス1マスに対して石が置けるか判定する. 
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
