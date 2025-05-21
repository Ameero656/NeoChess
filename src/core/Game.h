#ifndef GAME_H
#define GAME_H

#include "core/Board.h"
#include "player/Player.h" // Forward declaration is fine for unique_ptr members
#include "core/ChessTypes.h"
#include "core/Move.h"
#include <vector>
#include <memory> // For std::unique_ptr
#include <string> // For FEN, move history in string format
#include <unordered_map>


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
    std::unordered_map<uint64_t, int> gameStateRecord;
    uint64_t gameStateHash;



    void switchPlayer();
    void updateGameState(); // Checks for check, checkmate, stalemate, draw conditions
    bool hasLegalMoves(Color playerColor); // Checks if the player has any legal moves

    // Generates all pseudo-legal moves for a player (moves that are valid on the board
    std::vector<Move> generatePseudoLegalMoves(Color playerColor) const;

public:
    Game(PlayerType p1Type, PlayerType p2Type, int boardRows = 8, int boardCols = 8);
    ~Game();

    void start(); // Main game loop will be initiated from here or externally
    bool makeMove(const Move& move); // Attempts to make a move, returns true if successful
    bool unmakeMove(const Move& move);

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
    Game clone() const;

    // To allow AI player to suggest a move
    Move requestAIMove(const EvaluationEngine& engine, int depth);
    bool isKingInCheck(Color kingColor) const;
    
    // Explicitly defined Move Constructor
    Game(Game&& other) noexcept;

    // It's also good practice to define a move assignment operator if you have a move constructor
    Game& operator=(Game&& other) noexcept;

    uint64_t getGameStateHash() const;
    int Game::getGameStateCount() const;
    void recordGameState();
    void hashGameState();
};

#endif // GAME_H