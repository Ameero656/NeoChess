#ifndef BISHOP_H
#define BISHOP_H

#include "core/Piece.h"

class Bishop : public Piece {
public:
    Bishop(Color c, Position pos);

    std::vector<Move> getPossibleMoves(const Board& board) const override;
    char getSymbol() const override;
    std::unique_ptr<Piece> clone() const override;
};

#endif // BISHOP_H