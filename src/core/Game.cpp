#include "core/Game.h"
#include "player/Player.h" // Now include full definition
#include "player/HumanPlayer.h"
#include "player/AIPlayer.h"
#include "ai/EvaluationEngine.h" // For requestAIMove
#include <iostream> // For simple error messages
#include <algorithm> // For std::any_of, std::all_of

Game::Game(PlayerType p1Type, PlayerType p2Type, int boardRows, int boardCols)
    : board(boardRows, boardCols), currentPlayerColor(Color::WHITE), gameState(GameState::PLAYING),
      halfMoveClock(0), fullMoveCounter(1) {

    // Board should be initialized here if not in its own constructor
    board.initializeDefaultSetup(); // Or some other setup

    if (p1Type == PlayerType::HUMAN) {
        player1 = std::make_unique<HumanPlayer>(Color::WHITE);
    } else {
        player1 = std::make_unique<AIPlayer>(Color::WHITE);
    }

    if (p2Type == PlayerType::HUMAN) {
        player2 = std::make_unique<HumanPlayer>(Color::BLACK);
    } else {
        player2 = std::make_unique<AIPlayer>(Color::BLACK);
    }

    recordGameState();
}

Game::~Game() {
    // Smart pointers handle their own memory
}

void Game::switchPlayer() {
    currentPlayerColor = (currentPlayerColor == Color::WHITE) ? Color::BLACK : Color::WHITE;
}

bool Game::isKingInCheck(Color kingColor) const {
    Position kingPos = board.findKing(kingColor);
    if (!kingPos.isValid(board.getDimensions().rows, board.getDimensions().cols)) {
        // This should not happen in a normal game
        return false; // Or throw an error
    }
    Color attackerColor = (kingColor == Color::WHITE) ? Color::BLACK : Color::WHITE;
    return board.isSquareAttacked(kingPos, attackerColor);
}

std::vector<Move> Game::generatePseudoLegalMoves(Color playerColor) const {
    std::vector<Move> allMoves;
    for (int r = 0; r < board.getDimensions().rows; ++r) {
        for (int c = 0; c < board.getDimensions().cols; ++c) {
            const Piece* piece = board.getPieceAt(Position(r, c));
            if (piece && piece->getColor() == playerColor) {
                std::vector<Move> pieceMoves = piece->getPossibleMoves(board);
                allMoves.insert(allMoves.end(), pieceMoves.begin(), pieceMoves.end());
            }
        }
    }
    return allMoves;
}

std::vector<Move> Game::getLegalMovesForColor(Color color) const {
    std::vector<Move> legalMoves;
    std::vector<Move> pseudoLegalMoves = generatePseudoLegalMoves(color);

    for (const auto& move : pseudoLegalMoves) {
        // Simulate the move on a temporary board
        Board tempBoard = board; // Uses Board's copy constructor
        Piece* movingPiece = tempBoard.getPieceAt(move.from); // Get piece from temp board
        if (!movingPiece) continue; // Should not happen if generatePseudoLegalMoves is correct

        tempBoard.performMove(move); // Perform the move

        // Check if the current player's king is in check AFTER the move
        if (!tempBoard.isSquareAttacked(tempBoard.findKing(color), (color == Color::WHITE) ? Color::BLACK : Color::WHITE)) {
             // Special castling rules: King cannot castle through or into check
            if (move.isCastling) {
                bool canCastle = true;
                Position kingFrom = move.from;
                Position kingTo = move.to;
                Color opponentColor = (color == Color::WHITE) ? Color::BLACK : Color::WHITE;

                // Cannot castle out of check
                if (board.isSquareAttacked(kingFrom, opponentColor)) {
                    canCastle = false;
                }
                // Cannot castle through check
                Position intermediateSquare = (kingTo.col > kingFrom.col) ?
                                             Position(kingFrom.row, kingFrom.col + 1) :
                                             Position(kingFrom.row, kingFrom.col - 1);
                if (board.isSquareAttacked(intermediateSquare, opponentColor)) {
                    canCastle = false;
                }
                // Cannot castle into check (already covered by the general check after tempBoard.performMove)

                if(canCastle) legalMoves.push_back(move);

            } else {
                 legalMoves.push_back(move);
            }
        }
    }
    return legalMoves;
}


std::vector<Move> Game::getLegalMoves() const {
    return getLegalMovesForColor(currentPlayerColor);
}


bool Game::hasLegalMoves(Color playerColor) {
    return !getLegalMovesForColor(playerColor).empty();
}

void Game::updateGameState() {
    bool inCheck = isKingInCheck(currentPlayerColor);
    bool canMove = hasLegalMoves(currentPlayerColor);

    if (inCheck && !canMove) {
        gameState = (currentPlayerColor == Color::WHITE) ? GameState::CHECKMATE_BLACK_WINS : GameState::CHECKMATE_WHITE_WINS;
    } else if (!inCheck && !canMove) {
        gameState = GameState::STALEMATE;
    } else if (halfMoveClock >= 100) { // 50 moves by each player = 100 half-moves
        gameState = GameState::DRAW_HALF_MOVE_RULE;
    }
    // TODO: Add other draw conditions (threefold repetition, insufficient material)
    else if (inCheck) {
        gameState = GameState::CHECK; // Set to CHECK if playing and in check
    } else {
        gameState = GameState::PLAYING; // Default if no other condition met
    }
    board.setLastMove(moveHistory.empty() ? nullptr : &moveHistory.back());
}


void Game::start() {
    // This is a conceptual start. The actual game loop might be in main.cpp
    // or a dedicated game controller class.
    // For now, this just ensures the game is in a playable state.
    if (gameState != GameState::PLAYING && gameState != GameState::CHECK) {
        board.initializeDefaultSetup();
        currentPlayerColor = Color::WHITE;
        gameState = GameState::PLAYING;
        moveHistory.clear();
        halfMoveClock = 0;
        fullMoveCounter = 1;
        // positionHistoryFEN.clear();
        // addCurrentPositionToHistory(); // For threefold repetition
    }
    updateGameState(); // Initial state check
}


bool Game::makeMove(const Move& proposedMove) {
    if (gameState != GameState::PLAYING && gameState != GameState::CHECK) {
        // std::cout << "Game is over or not started properly." << std::endl;
        return false;
    }

    const Piece* pieceToMove = board.getPieceAt(proposedMove.from);
    if (!pieceToMove || pieceToMove->getColor() != currentPlayerColor) {
        // std::cout << "Invalid piece selection or not current player's piece." << std::endl;
        return false;
    }

    std::vector<Move> legalMoves = getLegalMoves();
    bool isProposedMoveLegal = false;
    for (const auto& legalMove : legalMoves) {
        if (legalMove == proposedMove) {
            isProposedMoveLegal = true;
            break;
        }
    }

    if (!isProposedMoveLegal) {
        // std::cout << "Move is not legal." << std::endl;
        return false;
    }

    // --- Update clocks and en passant before making the move on board ---
    // Half-move clock reset
    if (pieceToMove->getType() == PieceType::PAWN || board.getPieceAt(proposedMove.to) != nullptr) {
        halfMoveClock = 0;
    } else {
        halfMoveClock++;
    }

    // Set en passant target square if it's a two-square pawn move
    board.clearEnPassantTargetSquare(); // Clear previous one
    if (pieceToMove->getType() == PieceType::PAWN && std::abs(proposedMove.to.row - proposedMove.from.row) == 2) {
        int direction = (currentPlayerColor == Color::WHITE) ? -1 : 1;
        board.setEnPassantTargetSquare(Position(proposedMove.from.row + direction, proposedMove.from.col));
    }

    // --- Actually perform the move ---
    board.performMove(proposedMove);
    moveHistory.push_back(proposedMove);
    board.setLastMove(&moveHistory.back());


    // Full move counter
    if (currentPlayerColor == Color::BLACK) {
        fullMoveCounter++;
    }

    switchPlayer();
    updateGameState(); // Check for check/checkmate/stalemate/draw after the move
    recordGameState();
    if (getGameStateCount() >= 3) gameState = GameState::DRAW_THREEFOLD_REPETITION;

 
    return true;
}

bool Game::unmakeMove(const Move& proposedMove) {
    Position source = proposedMove.to;
    Position destination = proposedMove.from;
    const Piece* pieceToMove = board.getPieceAt(source);
    board.performUnmove(proposedMove);
    //TODO: Impliment

    return false;
    
}

// Getters
const Board& Game::getBoard() const {
    return board;
}
Board& Game::getBoard() {
    return board;
}


Color Game::getCurrentPlayerColor() const {
    return currentPlayerColor;
}

GameState Game::getGameState() const {
    return gameState;
}

const std::vector<Move>& Game::getMoveHistory() const {
    return moveHistory;
}

int Game::getHalfMoveClock() const {
    return halfMoveClock;
}

int Game::getFullMoveCounter() const {
    return fullMoveCounter;
}

const Player* Game::getCurrentPlayer() const {
    return (currentPlayerColor == Color::WHITE) ? player1.get() : player2.get();
}
const Player* Game::getPlayer(Color color) const {
    return (color == Color::WHITE) ? player1.get() : player2.get();
}


Game Game::clone() const {
    // This is a simplified clone. Proper cloning of Players might be needed
    // if they store significant state. For AI that just needs a board and game rules,
    // creating new default players for the clone might be sufficient.
    Game clonedGame(PlayerType::AI, PlayerType::AI); // Use dummy player types initially

    clonedGame.board = this->board; // Uses Board's copy constructor
    clonedGame.currentPlayerColor = this->currentPlayerColor;
    clonedGame.gameState = this->gameState;
    clonedGame.moveHistory = this->moveHistory; // Copies the vector of Moves
    clonedGame.halfMoveClock = this->halfMoveClock;
    clonedGame.fullMoveCounter = this->fullMoveCounter;

    // Re-point the board's lastMove to the cloned history if not empty
    if (!clonedGame.moveHistory.empty()) {
        clonedGame.board.setLastMove(&clonedGame.moveHistory.back());
    } else {
        clonedGame.board.setLastMove(nullptr);
    }
    
    // Players are tricky to clone deeply if they have complex state.
    // For evaluation, we might not need to clone players fully.
    // The clone will get new default AI players. This might be fine for many AI algos.
    // If AIPlayer stores learned data, then a proper clone or copy mechanism for AIPlayer is needed.
    // For now, it gets new default AI players based on the constructor.
    // A more robust clone might take player types as params or have a way to clone players.
    // Let's assume for now the cloned game gets generic AI players.
    // If you need to preserve player *types* (Human/AI) in the clone:
    PlayerType p1t = (dynamic_cast<HumanPlayer*>(player1.get())) ? PlayerType::HUMAN : PlayerType::AI;
    PlayerType p2t = (dynamic_cast<HumanPlayer*>(player2.get())) ? PlayerType::HUMAN : PlayerType::AI;
    
    // Re-create players for the clone
    if (p1t == PlayerType::HUMAN) clonedGame.player1 = std::make_unique<HumanPlayer>(Color::WHITE);
    else clonedGame.player1 = std::make_unique<AIPlayer>(Color::WHITE);

    if (p2t == PlayerType::HUMAN) clonedGame.player2 = std::make_unique<HumanPlayer>(Color::BLACK);
    else clonedGame.player2 = std::make_unique<AIPlayer>(Color::BLACK);

    return clonedGame;
}

Move Game::requestAIMove(const EvaluationEngine& engine, int depth) {
    // This method would typically be called by an AIPlayer.
    // For now, it's a utility in Game.
    // The EvaluationEngine should ideally operate on a const Game& or a copy.
    return engine.findBestMove(*this, depth);
}

// Move Constructor
Game::Game(Game&& other) noexcept
    : board(std::move(other.board)), // Use Board's move constructor
      player1(std::move(other.player1)),
      player2(std::move(other.player2)),
      currentPlayerColor(other.currentPlayerColor),
      gameState(other.gameState),
      moveHistory(std::move(other.moveHistory)),
      halfMoveClock(other.halfMoveClock),
      fullMoveCounter(other.fullMoveCounter) {
    // After moving, 'other' should be in a valid but unspecified state.
    // For example, its unique_ptrs are now null.
    // Ensure its board's lastMove pointer is also sensible if it was pointing into its own history
    if (!this->moveHistory.empty()) {
        this->board.setLastMove(&this->moveHistory.back());
    } else {
        this->board.setLastMove(nullptr);
    }
    other.board.setLastMove(nullptr); // Or handle other.board state appropriately
}

// Move Assignment Operator
Game& Game::operator=(Game&& other) noexcept {
    if (this != &other) { // Prevent self-assignment
        board = std::move(other.board);
        player1 = std::move(other.player1);
        player2 = std::move(other.player2);
        currentPlayerColor = other.currentPlayerColor;
        gameState = other.gameState;
        moveHistory = std::move(other.moveHistory);
        halfMoveClock = other.halfMoveClock;
        fullMoveCounter = other.fullMoveCounter;

        // Similar to move constructor, handle board's lastMove
        if (!this->moveHistory.empty()) {
            this->board.setLastMove(&this->moveHistory.back());
        } else {
            this->board.setLastMove(nullptr);
        }
        other.board.setLastMove(nullptr);
    }
    return *this;
}

uint64_t Game::getGameStateHash() const {
    return gameStateHash;
}

int Game::getGameStateCount() const {
    auto it = gameStateRecord.find(gameStateHash);
    if (it != gameStateRecord.end()) {
        return it->second;
    }
    return 0;
}

void Game::recordGameState() {
    hashGameState();
    
    gameStateRecord[gameStateHash]++; 
}

void Game::hashGameState() {

    gameStateHash = 0;

    for (int r=0; r < board.getDimensions().rows; ++r) {
        for (int c=0; c < board.getDimensions().cols; ++c) {
            const Piece* piece = board.getPieceAt(Position(r, c));
            if (!piece) continue;
            int val = (static_cast<int>(piece->getType()) + static_cast<int>(piece->getColor()) + r + c);

            gameStateHash ^= val * (currentPlayerColor == Color::WHITE ? 0x9e3779b97f4a7c15 : 0xfedcba9876543210);
        }
    }
}