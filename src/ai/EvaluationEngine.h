#ifndef EVALUATION_ENGINE_H
#define EVALUATION_ENGINE_H

#include "core/Move.h"
#include "core/ChessTypes.h" // For Color
#include <vector> // For storing lines of play, etc.

// Forward declarations
class Game; // Game state is needed for evaluation
class Board; // Board state is directly evaluated

// Structure to hold evaluation result
struct EvaluationResult {
    float score;         // The score of the position (+ for white, - for black)
    Move bestMove;       // The best move found from this position
    int nodesSearched;   // For performance tracking
    // std::vector<Move> principalVariation; // Optional: the expected line of play

    EvaluationResult() : score(0.0f), bestMove(Position(-1,-1), Position(-1,-1)), nodesSearched(0) {}
};


class EvaluationEngine {
public:
    EvaluationEngine();
    EvaluationEngine(float materialWeight, float mobilityWeight, float kingSafetyWeight, float pawnStructureWeight, float centerControlWeight);

    // Main method to find the best move for the current player in the given game state
    Move findBestMove(const Game& game, int depth) const;

    // Static evaluation of the board from a given player's perspective
    float staticEvaluate(const Board& board, Color perspective, const bool report = false) const;

private:
    // Recursive search function (e.g., Minimax with Alpha-Beta Pruning)
    // 'game' is const Game& as we operate on copies or don't modify original game state directly during search.
    // The 'game' parameter here would likely be a *copy* of the game state that the search algorithm
    // can then modify by making/unmaking moves.
    EvaluationResult search(Game game, int depth, float alpha, float beta, bool maximizingPlayer, Color originalPlayerColor) const;

    // Parameters for evaluation - can be made configurable
    float materialWeight;
    float mobilityWeight;
    float kingSafetyWeight;
    float pawnStructureWeight;
    float centerControlWeight;
    // Add more as needed

    // Helper: Order moves for more efficient alpha-beta pruning
    std::vector<Move> orderMoves(const std::vector<Move>& moves, const Board& board) const;
};

#endif // EVALUATION_ENGINE_H