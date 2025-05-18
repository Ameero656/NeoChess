#ifndef ROOK_H
#define ROOK_H

#include "core/Piece.h"

class Rook : public Piece {
public:
    Rook(Color c, Position pos);

    std::vector<Move> getPossibleMoves(const Board& board) const override;
    char getSymbol() const override;
    std::unique_ptr<Piece> clone() const override;
};

#endif // ROOK_H