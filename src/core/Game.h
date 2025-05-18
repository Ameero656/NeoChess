#ifndef GAME_H
#define GAME_H

#include "core/Board.h"
#include "player/Player.h" // Forward declaration is fine for unique_ptr members
#include "core/ChessTypes.h"
#include "core/Move.h"
#include <vector>
#include <memory> // For std::unique_ptr
#include <string> // For FEN, move history in string format

// Forward declare Player to avoid full include here if Player.h includes Game.h (circular)
class Player;
class EvaluationEngine; // For AI to access evaluation (or Game provides an eval interface)

class Game {
private:
    Board board;
    std::unique_ptr<Player> player1; // White
    std::unique_ptr<Player> player2; // Black
    Color currentPlayerColor;
    GameState gameState;
    std::vector<Move> moveHistory; // Stores all moves made
    int halfMoveClock; // For 50-move rule
    int fullMoveCounter; // Increments after Black moves

    // For threefold repetition detection (more complex, placeholder for now)
    // std::vector<std::string> positionHistoryFEN; // Store FEN strings of positions

    void switchPlayer();
    void updateGameState(); // Checks for check, checkmate, stalemate, draw conditions
    bool hasLegalMoves(Color playerColor); // Checks if the player has any legal moves

    // Generates all pseudo-legal moves for a player (moves that are valid on the board
    // but don't yet account for leaving the king in check)
    std::vector<Move> generatePseudoLegalMoves(Color playerColor) const;

public:
    Game(PlayerType p1Type, PlayerType p2Type, int boardRows = 8, int boardCols = 8);
    ~Game();

    void start(); // Main game loop will be initiated from here or externally
    bool makeMove(const Move& move); // Attempts to make a move, returns true if successful

    // Getters
    const Board& getBoard() const;
    Board& getBoard(); // Non-const version for internal use or AI needing to modify a copy
    Color getCurrentPlayerColor() const;
    GameState getGameState() const;
    const std::vector<Move>& getMoveHistory() const;
    int getHalfMoveClock() const;
    int getFullMoveCounter() const;
    const Player* getCurrentPlayer() const; // Returns a const pointer to the current player
    const Player* getPlayer(Color color) const;

    // Generates all fully legal moves for the current player
    std::vector<Move> getLegalMoves() const;
    std::vector<Move> getLegalMovesForColor(Color color) const;


    // For AI and deep copying/simulation
    Game clone() const; // Creates a deep copy of the game state
                       // This is complex due to Player ownership if they hold state.
                       // A simpler clone might only copy board and essential game vars,
                       // and be used by an AI that doesn't need full Player objects.

    // For setting up custom positions (e.g., from FEN) - Future
    // void loadFEN(const std::string& fenString);
    // std::string toFEN() const;

    // To allow AI player to suggest a move
    Move requestAIMove(const EvaluationEngine& engine, int depth);
    bool isKingInCheck(Color kingColor) const;
    
    // Explicitly defined Move Constructor
    Game(Game&& other) noexcept;

    // It's also good practice to define a move assignment operator if you have a move constructor
    Game& operator=(Game&& other) noexcept;
};

#endif // GAME_H