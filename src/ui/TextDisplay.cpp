#include "ui/TextDisplay.h"
#include "core/Game.h"   // For full Game state access
#include "core/Board.h"
#include "core/Piece.h"
#include "core/Move.h"
#include "core/ChessTypes.h" // For Color, GameState
#include <iostream>
#include <vector>
#include <iomanip> // For std::setw

// ANSI escape codes for colors (optional, might not work on all terminals e.g. Windows cmd by default)

const std::string RESET_COLOR = "\033[0m";
const std::string WHITE_PIECE_COLOR = "\033[1;37m"; // Bold White
const std::string BLACK_PIECE_COLOR = "\033[1;31m"; // Bold Red (often used for black for visibility on dark bg) or \033[1;30m for Bold Black
const std::string HIGHLIGHT_COLOR_BG = "\033[43m";   // Yellow background



TextDisplay::TextDisplay() {}

void TextDisplay::clearScreen() const {
#ifdef _WIN32
    system("cls");
#else
    // Standard ANSI escape sequence for clearing screen
    std::cout << "\033[2J\033[1;1H";
#endif
    std::cout.flush();
}

char TextDisplay::getPieceChar(const Piece* piece) const {
    if (!piece) {
        return '.'; // Empty square
    }
    return piece->getSymbol(); // Uses Piece's own symbol logic
}

void TextDisplay::displayBoard(const Board& board, const Move* lastMove) const {
    BoardDimensions dims = board.getDimensions();

    std::cout << std::endl;
    std::cout << "    "; // Initial padding for column labels
    for (int c = 0; c < dims.cols; ++c) {
        std::cout << (char)('a' + c) << " ";
    }
    std::cout << std::endl;
    std::cout << "  +";
    for (int c = 0; c < dims.cols; ++c) {
        std::cout << "--";
    }
    std::cout << "-+" << std::endl;

    for (int r = 0; r < dims.rows; ++r) {
        std::cout << std::setw(2) << (dims.rows - r) << "| "; // Row labels (8 down to 1)
        for (int c = 0; c < dims.cols; ++c) {
            const Piece* piece = board.getPieceAt(Position(r, c));
            char pieceChar = getPieceChar(piece);
            std::string pieceColorStr = "";
            std::string bgColorStr = "";

            if (piece) {
                pieceColorStr = (piece->getColor() == Color::WHITE) ? WHITE_PIECE_COLOR : BLACK_PIECE_COLOR;
            }
            if (lastMove && (lastMove->from == Position(r,c) || lastMove->to == Position(r,c)) ) {
                bgColorStr = HIGHLIGHT_COLOR_BG;
            }


            std::cout << bgColorStr << pieceColorStr << pieceChar << RESET_COLOR << " ";
        }
        std::cout << "|" << std::setw(2) << (dims.rows - r) << std::endl;
    }

    std::cout << "  +";
    for (int c = 0; c < dims.cols; ++c) {
        std::cout << "--";
    }
    std::cout << "-+" << std::endl;
    std::cout << "    ";
    for (int c = 0; c < dims.cols; ++c) {
        std::cout << (char)('a' + c) << " ";
    }
    std::cout << std::endl << std::endl;
}

void TextDisplay::displayGameStatus(const Game& game, const bool static_eval, const EvaluationEngine evalEngine) const {
    std::cout << "---------------------------------" << std::endl;
    std::cout << "Turn: " << game.getFullMoveCounter();
    std::cout << " | Player to move: "
              << (game.getCurrentPlayerColor() == Color::WHITE ? "White" : "Black");
    std::cout << " | Halfmove Clock: " << game.getHalfMoveClock() << std::endl;
    if (static_eval) {
        std::cout << " | Static Evaluation: " << std::endl;
        evalEngine.staticEvaluate(game.getBoard(), Color::WHITE, true);
    }

    std::cout << "Game State Hash:" << game.getGameStateHash() << "->" << game.getGameStateCount() << std::endl;
        


    GameState state = game.getGameState();
    std::string statusMsg = "Status: ";
    switch (state) {
        case GameState::PLAYING: statusMsg += "Playing"; break;
        case GameState::CHECK: statusMsg += (game.getCurrentPlayerColor() == Color::WHITE ? "White" : "Black"); statusMsg += " is in Check!"; break;
        case GameState::CHECKMATE_WHITE_WINS: statusMsg += "Checkmate! White wins."; break;
        case GameState::CHECKMATE_BLACK_WINS: statusMsg += "Checkmate! Black wins."; break;
        case GameState::STALEMATE: statusMsg += "Stalemate! Game is a draw."; break;
        case GameState::DRAW_HALF_MOVE_RULE: statusMsg += "Draw by 50-move rule."; break;
        default: statusMsg += "Game Over or Unknown State."; break;
    }
    std::cout << statusMsg << std::endl;

    const auto& history = game.getMoveHistory();
    if (!history.empty()) {
        std::cout << "Last move: " << history.back().toString() << std::endl;
    }
    std::cout << "---------------------------------" << std::endl;
}

void TextDisplay::displayEvaluation(float score, const Move& bestMove) const {
    std::cout << "AI Evaluation: Score = " << std::fixed << std::setprecision(2) << score;
    if (bestMove.from.isValid() && bestMove.to.isValid()) {
        std::cout << " | Suggested Move: " << bestMove.toString();
    }
    std::cout << std::endl;
}