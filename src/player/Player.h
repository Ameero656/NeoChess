#ifndef PLAYER_H
#define PLAYER_H

#include "core/ChessTypes.h"
#include "core/Move.h"
#include <string>

// Forward declare Game to avoid circular dependency if Game.h includes Player.h
class Game;
class EvaluationEngine; // For AIPlayer

class Player {
protected:
    Color playerColor;
    std::string name;

public:
    Player(Color color, std::string playerName = "Player");
    virtual ~Player() = default;

    Color getColor() const;
    std::string getName() const;

    // Pure virtual function for getting a move
    // The Game& context provides all necessary information.
    // For AI, EvaluationEngine might be passed or accessed via Game.
    virtual Move getMove(const Game& game, const EvaluationEngine* engine = nullptr) const = 0; // ADDED const

    // Optional: A way for players to be notified of events, like game over or opponent's move.
    // virtual void notify(const std::string& message);
};

#endif // PLAYER_H