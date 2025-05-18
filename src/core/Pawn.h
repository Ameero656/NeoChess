#ifndef PAWN_H
#define PAWN_H

#include "core/Piece.h"

class Pawn : public Piece {
public:
    Pawn(Color c, Position pos);

    std::vector<Move> getPossibleMoves(const Board& board) const override;
    char getSymbol() const override;
    std::unique_ptr<Piece> clone() const override;

    // Pawn specific logic, e.g., for en passant eligibility
    // bool eligibleForEnPassant; // This might be better tracked in the Game or Board state
                               // based on the last move made.
    // Or a flag if this pawn just made a two-square move.
};

#endif // PAWN_H