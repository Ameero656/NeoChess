#ifndef KNIGHT_H
#define KNIGHT_H

#include "core/Piece.h"

class Knight : public Piece {
public:
    Knight(Color c, Position pos);

    std::vector<Move> getPossibleMoves(const Board& board) const override;
    char getSymbol() const override;
    std::unique_ptr<Piece> clone() const override;
};

#endif // KNIGHT_H