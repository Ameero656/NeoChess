#ifndef AI_PLAYER_H
#define AI_PLAYER_H

#include "player/Player.h"

// Forward declare EvaluationEngine as AIPlayer uses it.
class EvaluationEngine;
class Game; // Also need Game for context

class AIPlayer : public Player {
private:
    int searchDepth; // Configurable search depth for this AI instance

public:
    AIPlayer(Color color, std::string name = "AI", int depth = 3); // Default depth

    Move getMove(const Game& game, const EvaluationEngine* engine) const override;

    void setSearchDepth(int depth);
    int getSearchDepth() const;
};

#endif // AI_PLAYER_H