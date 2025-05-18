#ifndef TEXT_DISPLAY_H
#define TEXT_DISPLAY_H

#include <string>
#include "ai/EvaluationEngine.h"


const std::string COLOR_TO_STRING;

// Forward declare Game as TextDisplay will need to read its state
class Game;
class Board; // Specifically the board
class Piece; // And pieces
struct Move; // For highlighting last move

class TextDisplay {
public:
    TextDisplay();

    // Displays the current state of the game board
    void displayBoard(const Board& board, const Move* lastMove = nullptr) const;

    // Displays game status messages (e.g., Check, Checkmate, Player to move)
    void displayGameStatus(const Game& game, const bool static_eval = false, const EvaluationEngine evalEngine = {}) const;

    // Displays evaluation information
    void displayEvaluation(float score, const Move& bestMove) const;

    // Helper to clear the console (platform-dependent, basic version here)
    void clearScreen() const;

private:
    // Helper to get piece character (could be more advanced, e.g. unicode)
    char getPieceChar(const Piece* piece) const;
};

#endif // TEXT_DISPLAY_H