#include "player/AIPlayer.h"
#include "ai/EvaluationEngine.h" // Need full include for using the engine
#include "core/Game.h"           // For Game context
#include <iostream> // For messages if no engine provided

AIPlayer::AIPlayer(Color color, std::string name, int depth)
    : Player(color, name), searchDepth(depth) {}

Move AIPlayer::getMove(const Game& game, const EvaluationEngine* engine) const {
    if (!engine) {
        std::cerr << "Error: AIPlayer requires an EvaluationEngine to make a move." << std::endl;
        // Return a dummy/invalid move or throw an exception
        return Move(Position(-1, -1), Position(-1, -1));
    }

    std::cout << getName() << " (" << (playerColor == Color::WHITE ? "White" : "Black")
              << ") is thinking with depth " << searchDepth << "..." << std::endl;

    // The EvaluationEngine should operate on a const Game or a copy.
    // The Game class's requestAIMove can be a helper, or engine can be called directly.
    return engine->findBestMove(game, searchDepth);
    // Or: return game.requestAIMove(*engine, searchDepth); if Game has that method
}

void AIPlayer::setSearchDepth(int depth) {
    searchDepth = depth;
}

int AIPlayer::getSearchDepth() const {
    return searchDepth;
}