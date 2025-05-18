#include "core/Move.h"

Move::Move(Position f, Position t, PieceType promo, bool castling, bool enPassant)
    : from(f), to(t), promotionPiece(promo), isCastling(castling), isEnPassantCapture(enPassant) {}

bool Move::operator==(const Move& other) const {
    return from == other.from &&
           to == other.to &&
           promotionPiece == other.promotionPiece &&
           isCastling == other.isCastling &&
           isEnPassantCapture == other.isEnPassantCapture;
}

std::string Move::toString() const {
    std::string str = from.toAlgebraic() + to.toAlgebraic();
    if (promotionPiece != PieceType::EMPTY) {
        // Simple representation for promotion, can be enhanced
        switch (promotionPiece) {
            case PieceType::QUEEN: str += 'q'; break;
            case PieceType::ROOK: str += 'r'; break;
            case PieceType::BISHOP: str += 'b'; break;
            case PieceType::KNIGHT: str += 'n'; break;
            default: break;
        }
    }
    return str;
}