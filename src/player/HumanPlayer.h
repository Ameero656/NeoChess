#ifndef HUMAN_PLAYER_H
#define HUMAN_PLAYER_H

#include "player/Player.h"
#include <iostream> // For std::cin, std::cout

class HumanPlayer : public Player {
public:
    HumanPlayer(Color color, std::string name = "Human");

    Move getMove(const Game& game, const EvaluationEngine* engine = nullptr) const override;
};

#endif // HUMAN_PLAYER_H