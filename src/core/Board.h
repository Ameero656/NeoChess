#ifndef BOARD_H
#define BOARD_H

#include "core/ChessTypes.h"
#include "core/Position.h"
#include "core/Piece.h"
#include "core/Move.h" // For lastMove
#include <vector>
#include <memory> // For std::unique_ptr, std::shared_ptr

// Forward declare Piece derived classes to avoid circular dependencies
// if Board methods need to know about specific piece types for setup.
// However, direct knowledge is often better handled via PieceType enum.
class Pawn;
class Rook;
class Knight;
class Bishop;
class Queen;
class King;

class Board {
private:
    std::vector<std::vector<std::unique_ptr<Piece>>> grid;
    BoardDimensions dimensions;
    const Move* lastMove; // Pointer to the last move made on this board (non-owning, Game owns Move objects in history)
                         // Used for rules like en passant. Set by Game.

    // Castling rights - more robust than just checking piece->hasMoved
    // These flags are typically set to false if the King or respective Rook moves.
    bool whiteCanCastleKingside;
    bool whiteCanCastleQueenside;
    bool blackCanCastleKingside;
    bool blackCanCastleQueenside;

    // Position of the pawn that just made a two-square advance (for en passant)
    // If no such pawn, position can be invalid.
    Position enPassantTargetSquare;


public:
    Board(int rows = 8, int cols = 8); // Default to standard 8x8 board
    Board(const Board& other); // Copy constructor for cloning (e.g., for AI evaluation)
    Board& operator=(const Board& other); // Copy assignment operator
    Board(Board&& other) noexcept; // Move constructor
    Board& operator=(Board&& other) noexcept; // Move assignment operator

    void initializeEmptyBoard();
    void initializeDefaultSetup(); // Sets up the standard chess starting position
    // void initializeCustomSetup(const std::string& fen); // Future: FEN string setup

    const Piece* getPieceAt(Position pos) const;
    Piece* getPieceAt(Position pos); // Non-const version for internal modification
    void addPiece(std::unique_ptr<Piece> piece, Position pos);
    std::unique_ptr<Piece> removePiece(Position pos); // Returns the removed piece

    // Executes a move on the board. Does not validate legality beyond basic captures.
    // Returns the captured piece, if any.
    // Game logic is responsible for validating the move first.
    std::unique_ptr<Piece> performMove(const Move& move);
    std::unique_ptr<Piece> performUnmove(const Move& move);

    BoardDimensions getDimensions() const;

    // En Passant related
    Position getEnPassantTargetSquare() const;
    void setEnPassantTargetSquare(Position pos); // Game will call this after a 2-square pawn move
    void clearEnPassantTargetSquare();

    // Castling rights
    bool canCastleKingside(Color color) const;
    bool canCastleQueenside(Color color) const;
    void updateCastlingRights(const Move& moveMade, Piece* movedPiece); // Call after a king or rook moves
    void setCastlingRights(Color color, bool kingside, bool queenside); // For specific setups


    // Helper for Pawn's getPossibleMoves
    const Move* getLastMove() const;
    void setLastMove(const Move* move); // Game will set this after each move

    // Utility to find all pieces of a certain color/type
    std::vector<Piece*> findPieces(Color color, PieceType type = PieceType::EMPTY) const;
    Position findKing(Color color) const;

    // Check if a square is attacked by the opponent
    bool isSquareAttacked(Position square, Color attackerColor) const;

};

#endif // BOARD_H