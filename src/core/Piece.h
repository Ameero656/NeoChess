#ifndef PIECE_H
#define PIECE_H

#include "core/ChessTypes.h"
#include "core/Position.h"
#include "core/Move.h"
#include <vector>
#include <string>
#include <memory> // For std::unique_ptr in derived classes if needed for specific data

// Forward declaration
class Board;

class Piece {
protected:
    Color color;
    Position position;
    PieceType type;
    float value; // Relative value of the piece
    bool hasMoved;
    int pieceId; // Unique ID for the piece instance, can be useful

    static int nextId; // For generating unique piece IDs

public:
    Piece(Color c, Position pos, PieceType t, float val);
    virtual ~Piece() = default; // Important for base class with virtual methods

    // Pure virtual method for getting possible moves
    virtual std::vector<Move> getPossibleMoves(const Board& board) const = 0;

    // Virtual method for getting symbol (can be overridden for pawn color, etc.)
    virtual char getSymbol() const;
    
    Color getColor() const;
    Position getPosition() const;
    void setPosition(Position newPos); // Used when a piece is moved
    PieceType getType() const;
    float getValue() const;
    bool getHasMoved() const;
    void setHasMoved(bool moved);
    int getId() const;

    // Optional: a method to create a clone of the piece (for board copying)
    virtual std::unique_ptr<Piece> clone() const = 0;
};

#endif // PIECE_H