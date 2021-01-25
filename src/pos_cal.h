#ifndef POS_CAL_H
#define POS_CAL_H

#include "def.h"
#include "position.h"



template <BoardLen BOARD_LEN>
class PosCalculator
{
public:
    typedef enum {
        IN_CENTRAL = 0,
        IN_EDGE = 1,
        IN_CORNER = 2,
    } PosStatus;
    static PosCalculator &Ins();

    inline const Position &GetPos(PointIndex index) const {
        FOO_ASSERT(this->IsInBoard(index));
        return position_[index];
    }
    inline PointIndex GetIndex(const Position &pos) const {
        FOO_ASSERT(this->IsInBoard(pos));
        return indexes_[pos.y_][pos.x_];
    }

    inline bool IsInBoard(const Position &pos) const {
        return pos.x_ >= 0 && pos.x_ < BOARD_LEN
            && pos.y_ >= 0 && pos.y_ < BOARD_LEN;
    }
    inline bool IsInBoard(PointIndex indx) const {
        return indx >= 0 && indx < BLSq<BOARD_LEN>();
    }
    PosStatus GetPosStatus(const Position &pos);

#ifdef FOO_TEST
    static void TEST();
#endif

private:
    Position position_[BLSq<BOARD_LEN>()];
    PointIndex indexes_[BOARD_LEN][BOARD_LEN];

    PosCalculator();
    ~PosCalculator() = default;
    DISALLOW_COPY_AND_ASSIGN(PosCalculator);
    void Init();
    PosStatus CalPosStatus(BoardLen x, BoardLen y) const;
};



#include "pos_cal-TLT.h"

#ifdef FOO_TEST
#include "pos_cal_TEST.h"
#endif

#endif
