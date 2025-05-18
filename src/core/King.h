#ifndef KING_H
#define KING_H

#include "core/Piece.h"

class King : public Piece {
public:
    King(Color c, Position pos);

    std::vector<Move> getPossibleMoves(const Board& board) const override;
    char getSymbol() const override;
    std::unique_ptr<Piece> clone() const override;

    // Castling related flags might be part of Piece::hasMoved
    // or more explicitly here, or better yet, tracked by the Board/Game state.
    // For now, we rely on Piece::hasMoved and Board checks.
};

#endif // KING_H