#include "core/Piece.h"
#include "core/Board.h" // Required for getPossibleMoves (though not used directly in base impl)

int Piece::nextId = 0;

Piece::Piece(Color c, Position pos, PieceType t, float val)
    : color(c), position(pos), type(t), value(val), hasMoved(false), pieceId(nextId++) {}

// Basic symbol logic, can be refined
char Piece::getSymbol() const {
    switch (type) {
        case PieceType::PAWN:   return 'P';
        case PieceType::ROOK:   return 'R';
        case PieceType::KNIGHT: return 'N';
        case PieceType::BISHOP: return 'B';
        case PieceType::QUEEN:  return 'Q';
        case PieceType::KING:   return 'K';
        default:                return ' '; // Should not happen for actual pieces
    }
}

Color Piece::getColor() const {
    return color;
}

Position Piece::getPosition() const {
    return position;
}

void Piece::setPosition(Position newPos) {
    position = newPos;
}

PieceType Piece::getType() const {
    return type;
}

float Piece::getValue() const {
    return value;
}

bool Piece::getHasMoved() const {
    return hasMoved;
}

void Piece::setHasMoved(bool moved) {
    hasMoved = moved;
}

int Piece::getId() const {
    return pieceId;
}