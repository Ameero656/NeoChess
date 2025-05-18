#ifndef MOVE_H
#define MOVE_H

#include "core/Position.h"
#include "core/ChessTypes.h" // For PieceType (promotion)

struct Move {
    Position from;
    Position to;
    PieceType promotionPiece; // Type to promote to, if applicable (e.g., QUEEN)
    bool isCastling;
    bool isEnPassantCapture;
    // Add other special move flags if needed (e.g. double pawn push for en passant tracking)

    Move(Position f, Position t, PieceType promo = PieceType::EMPTY, bool castling = false, bool enPassant = false);

    bool operator==(const Move& other) const;
    std::string toString() const; // For logging or display
};

#endif // MOVE_H