#ifndef CHESS_TYPES_H
#define CHESS_TYPES_H

#include <vector>
#include <string>

// Represents the color of a piece or player
enum class Color {
    WHITE,
    BLACK,
    NONE // For empty squares or neutral state
    // Potentially more colors if you extend to >2 player chess
};

// Represents the type of a chess piece
enum class PieceType {
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
    EMPTY
};

// Represents the current state of the game
enum class GameState {
    PLAYING,
    CHECK,
    CHECKMATE_WHITE_WINS,
    CHECKMATE_BLACK_WINS,
    STALEMATE,
    DRAW_HALF_MOVE_RULE,
    DRAW_THREEFOLD_REPETITION, // Future consideration
    DRAW_INSUFFICIENT_MATERIAL, // Future consideration
    DRAW_AGREEMENT // Future consideration
};

// For user vs user, user vs computer, computer vs computer
enum class PlayerType {
    HUMAN,
    AI
};

// Simple struct for board dimensions - useful for customizable boards
struct BoardDimensions {
    int rows = 8;
    int cols = 8;
};

#endif // CHESS_TYPES_H