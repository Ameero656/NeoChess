#ifndef QUEEN_H
#define QUEEN_H

#include "core/Piece.h"

class Queen : public Piece {
public:
    Queen(Color c, Position pos);

    std::vector<Move> getPossibleMoves(const Board& board) const override;
    char getSymbol() const override;
    std::unique_ptr<Piece> clone() const override;
};

#endif // QUEEN_H