#ifndef PIECES_STR_SET_INL_H
#define PIECES_STR_SET_INL_H

#include "chain_set.h"

#include <cstring>

#include <algorithm>

#ifdef FOO_TEST
#define IS_POINT_NOT_EMPTY(piece_i) \
    (this->GetListHead(piece_i) != ChainSet<BOARD_LEN>::NONE_LIST)
#endif

using namespace std;


template <BoardLen BOARD_LEN>
ChainSet<BOARD_LEN>::ChainSet() {}


template <BoardLen BOARD_LEN>
void ChainSet<BOARD_LEN>::Copy(const ChainSet<BOARD_LEN> &c)
{
    memcpy(nodes_, c.nodes_, sizeof(nodes_));
    for (int i=0; i<BoardLenSquare<BOARD_LEN>(); ++i) {
        lists_[i].tail_ = c.lists_[i].tail_;
        lists_[i].len_ = c.lists_[i].len_;
        lists_[i].air_set_ = c.lists_[i].air_set_;
    }
}


template <BoardLen BOARD_LEN>
AirCount ChainSet<BOARD_LEN>::GetAirCountByPiece(PointIndex piece_i) const
{
    FOO_ASSERT(IS_POINT_NOT_EMPTY(piece_i));
    return this->GetAirCountOfAChain(this->GetListHead(piece_i));
}


template <BoardLen BOARD_LEN>
typename ChainSet<BOARD_LEN>::PntIndxArray
ChainSet<BOARD_LEN>::GetPieces(PointIndex piece_i) const
{
    FOO_ASSERT(IS_POINT_NOT_EMPTY(piece_i));
    return this->GetPiecesOfAChain(this->GetListHead(piece_i));
}


template <BoardLen BOARD_LEN>
void ChainSet<BOARD_LEN>::AddPiece(PointIndex indx,
                                   const ChainSet<BOARD_LEN>::AirSet &air_set)
{
//    FOO_PRINT_LINE("\nAddPiece called.");
    this->LetAdjcntChainsSetAir(indx, false);
    const PosCalculator<BOARD_LEN> &ins = this->GetPosClcltr();
    this->CreateList(indx, air_set);
    PointIndex list_i = indx;
    const Position &pos = ins.GetPos(indx);

    for (int i=0; i<4; ++i) {
//        FOO_PRINT_LINE(" ");
//        FOO_PRINT_LINE("in for loop, i = %d", i);
        Position adj_pos = pos.AdjcntPos(i);
//        FOO_PRINT_LINE("adjacent pos = %d, %d", adj_pos.x_, adj_pos.y_);
        if (ins.IsInBoard(adj_pos)) {
//            FOO_PRINT_LINE("adjacent pos is in board.");
            PointIndex adj_i = ins.GetIndex(adj_pos);
            PointIndex adj_list = this->GetListHead(adj_i);
//            FOO_PRINT_LINE("adj_list = %d", adj_list);
            if (adj_list == ChainSet<BOARD_LEN>::NONE_LIST) {
//                FOO_PRINT_LINE("is empty.");
                continue;
            }
            if (adj_list == list_i) continue;

            list_i = this->MergeLists(list_i, adj_list);
        }
    }
}


template <BoardLen BOARD_LEN>
void ChainSet<BOARD_LEN>::LetAdjcntChainsSetAir(PointIndex indx, bool v)
{
    AirSet air_set;
    if (!v) air_set.flip();
    air_set[indx] = v;
    auto &ins = this->GetPosClcltr();
    const Position &pos = ins.GetPos(indx);

    for (int i=0; i<4; ++i) {
        Position adj_pos = pos.AdjcntPos(i);
        if (!ins.IsInBoard(adj_pos)) continue;
        PointIndex adj_i = ins.GetIndex(adj_pos);
        PointIndex head = this->GetListHead(adj_i);
        if (head == ChainSet<BOARD_LEN>::NONE_LIST) continue;

        List *pl = lists_ + head;
        const AirSet &r_as = pl->air_set_;
        pl->air_set_ = v ? (r_as | air_set) : (r_as & air_set);
    }
}


template <BoardLen BOARD_LEN>
void ChainSet<BOARD_LEN>::RemoveListByPiece(PointIndex piece_i)
{
    FOO_ASSERT(IS_POINT_NOT_EMPTY(piece_i));
    PointIndex list_i = this->GetListHead(piece_i);
    this->RemoveList(list_i);
}


template <BoardLen BOARD_LEN>
void
ChainSet<BOARD_LEN>::CreateList(PointIndex node_i,
                                const ChainSet<BOARD_LEN>::AirSet &air_set)
{
    nodes_[node_i].list_head_ = node_i;
    lists_[node_i].tail_ = node_i;
    lists_[node_i].len_ = 1;
    lists_[node_i].air_set_ = air_set;
}


template <BoardLen BOARD_LEN>
PointIndex ChainSet<BOARD_LEN>::MergeLists(PointIndex list_a, PointIndex list_b)
{
    FOO_PRINT_LINE("MergeLists called.");
    FOO_ASSERT(list_a != list_b);
    if (lists_[list_a].len_ > lists_[list_b].len_) swap(list_a, list_b);

    for (int i=list_a; ; i=nodes_[i].next_) {
        nodes_[i].list_head_ = list_b;
        if (i == lists_[list_a].tail_) break;
    }

    nodes_[lists_[list_b].tail_].next_ = list_a;
    lists_[list_b].tail_ = lists_[list_a].tail_;
    lists_[list_b].len_ += lists_[list_a].len_;
    lists_[list_b].air_set_ |= lists_[list_a].air_set_;
    return list_b;
}


template <BoardLen BOARD_LEN>
void ChainSet<BOARD_LEN>::RemoveList(PointIndex head)
{
    for (int i=head; ; i=nodes_[i].next_) {
        nodes_[i].list_head_ = ChainSet<BOARD_LEN>::NONE_LIST;
        if (i == lists_[head].tail_) break;
    }
}


template <BoardLen BOARD_LEN>
AirCount ChainSet<BOARD_LEN>::GetAirCountOfAChain(PointIndex list_i) const
{
    return lists_[list_i].air_set_.count();
}


template <BoardLen BOARD_LEN>
typename ChainSet<BOARD_LEN>::PntIndxArray
ChainSet<BOARD_LEN>::GetPiecesOfAChain(PointIndex list_i) const
{
    auto pl = lists_ + list_i;
    ChainSet<BOARD_LEN>::PntIndxArray v(pl->len_);
    int vi = 0;

    for (int i=list_i; ; i=nodes_[i].next_) {
        v[vi++] = i;
        if (i == pl->tail_) break;
    }

    return move(v);
}


#endif
