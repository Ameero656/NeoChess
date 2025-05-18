#include "core/Game.h"
#include "player/HumanPlayer.h"
#include "player/AIPlayer.h"
#include "ai/EvaluationEngine.h"
#include "ui/TextDisplay.h"
#include <iostream>
#include <string>
#include <limits> // For numeric_limits

// Helper function to get player type from user
PlayerType getPlayerTypeChoice(const std::string& playerNumber) {
    std::string choice;
    while (true) {
        std::cout << "Select Player " << playerNumber << " type (human/ai): ";
        std::cin >> choice;
        if (choice == "human") return PlayerType::HUMAN;
        if (choice == "ai") return PlayerType::AI;
        std::cout << "Invalid choice. Please type 'human' or 'ai'." << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

int getAIDepthChoice(const std::string& playerNumber) {
    int depth;
    while (true) {
        std::cout << "Enter AI depth for Player " << playerNumber << " (e.g., 1-5): ";
        std::cin >> depth;
        if (std::cin.good() && depth > 0 && depth < 10) { // Basic validation
            return depth;
        }
        std::cout << "Invalid depth. Please enter a small positive integer." << std::endl;
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}


int main() {
    std::cout << "Welcome to C++ Chess!" << std::endl;

    PlayerType p1Type = getPlayerTypeChoice("1 (White)");
    PlayerType p2Type = getPlayerTypeChoice("2 (Black)");

    int p1Depth = 3, p2Depth = 3; // Default depths
    if (p1Type == PlayerType::AI) p1Depth = getAIDepthChoice("1 (White)");
    if (p2Type == PlayerType::AI) p2Depth = getAIDepthChoice("2 (Black)");


    Game chessGame(p1Type, p2Type);
    // If AI players were chosen, their depths might need to be set
    // This requires access to the player objects within Game, or Game constructor takes depths
    // For now, we can modify AIPlayer's constructor to take default depth or set it after.
    // Let's refine AIPlayer setup if specific depths were chosen:

    // This is a bit of a hack to set depth post-construction.
    // A better design might involve Game taking PlayerConfig objects.
    if (p1Type == PlayerType::AI) {
        AIPlayer* aiP1 = dynamic_cast<AIPlayer*>(const_cast<Player*>(chessGame.getPlayer(Color::WHITE)));
        if (aiP1) aiP1->setSearchDepth(p1Depth);
    }
    if (p2Type == PlayerType::AI) {
        AIPlayer* aiP2 = dynamic_cast<AIPlayer*>(const_cast<Player*>(chessGame.getPlayer(Color::BLACK)));
        if (aiP2) aiP2->setSearchDepth(p2Depth);
    }


    EvaluationEngine engine;
    TextDisplay display;

    chessGame.start(); // Initialize game state and board

    while (chessGame.getGameState() == GameState::PLAYING || chessGame.getGameState() == GameState::CHECK) {
        display.clearScreen();
        const Move* lastMovePtr = nullptr;
        if (!chessGame.getMoveHistory().empty()){
            lastMovePtr = &chessGame.getMoveHistory().back();
        }
        display.displayBoard(chessGame.getBoard(), lastMovePtr);
        display.displayGameStatus(chessGame);

        const Player* currentPlayer = chessGame.getCurrentPlayer();
        if (!currentPlayer) {
            std::cerr << "Error: No current player!" << std::endl;
            break;
        }

        std::cout << currentPlayer->getName() << "'s turn." << std::endl;

        Move move = currentPlayer->getMove(chessGame, &engine); // Pass engine for AI

        if (!move.from.isValid()) { // Indicates an issue or no move (e.g. AI couldn't find one)
            std::cout << "Player could not make a move. Game might be stuck or ended." << std::endl;
            break;
        }

        if (!chessGame.makeMove(move)) {
            std::cout << "Move (" << move.toString() << ") was invalid. This shouldn't happen if getMove is correct." << std::endl;
            // Potentially loop for current player to try again if human, or break if AI bug
        }
    }

    // Game over, display final state
    display.clearScreen();
    const Move* lastMovePtr = nullptr;
    if (!chessGame.getMoveHistory().empty()){
        lastMovePtr = &chessGame.getMoveHistory().back();
    }
    display.displayBoard(chessGame.getBoard(), lastMovePtr);
    display.displayGameStatus(chessGame);
    std::cout << "Game Over!" << std::endl;

    return 0;
}