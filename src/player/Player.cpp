#include "player/Player.h"
// No Game.h needed here as Game is only forward-declared in Player.h for method signature

Player::Player(Color color, std::string playerName) : playerColor(color), name(std::move(playerName)) {}

Color Player::getColor() const {
    return playerColor;
}

std::string Player::getName() const {
    return name;
}