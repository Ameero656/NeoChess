#include "ai/EvaluationEngine.h"
#include "core/Game.h"   // For Game and Board context
#include "core/Board.h"
#include "core/Piece.h"
#include <limits>     // For std::numeric_limits
#include <algorithm>  // For std::sort, std::max, std::min
#include <iostream>   // For debugging output
#include <cmath>

const float INFINITY_SCORE = std::numeric_limits<float>::infinity();

EvaluationEngine::EvaluationEngine()
    : materialWeight(1.0f), mobilityWeight(0.02f), kingSafetyWeight(0.05f),
      pawnStructureWeight(1.0f), centerControlWeight(0.5f) {
}

EvaluationEngine::EvaluationEngine(float materialWeight, float mobilityWeight, float kingSafetyWeight, float pawnStructureWeight, float centerControlWeight)
    : materialWeight(materialWeight), mobilityWeight(mobilityWeight), kingSafetyWeight(kingSafetyWeight),
      pawnStructureWeight(pawnStructureWeight), centerControlWeight(centerControlWeight) {
}

// Basic move ordering: captures first, then checks, then others.
// A more sophisticated version would use MVV-LVA (Most Valuable Victim - Least Valuable Aggressor)
// or history heuristics.
std::vector<Move> EvaluationEngine::orderMoves(const std::vector<Move>& moves, const Board& board) const {
    std::vector<Move> ordered = moves;
    std::sort(ordered.begin(), ordered.end(), [&](const Move& a, const Move& b) {
        bool aIsCapture = (board.getPieceAt(a.to) != nullptr);
        bool bIsCapture = (board.getPieceAt(b.to) != nullptr);
        // Placeholder: A real check detection is needed here
        // bool aIsCheck = false; // game.isCheckAfterMove(a);
        // bool bIsCheck = false; // game.isCheckAfterMove(b);

        if (aIsCapture && !bIsCapture) return true;
        if (!aIsCapture && bIsCapture) return false;
        
        // Further ordering (e.g., checks, promotions) can be added here.
        return false; // Default: no change in order
    });
    return ordered;
}


float EvaluationEngine::staticEvaluate(const Board& board, Color perspective, const bool report) const {
    float allyMaterial = 0.0f;
    float enemyMaterial = 0.0f;

    float allyPawnStructureScore = 0.0f;
    float enemyPawnStructureScore = 0.0f;

    float allyCenterControlScore = 0.0f;
    float enemyCenterControlScore = 0.0f;

    float allyKingSafetyScore = 0.0f;
    float enemyKingSafetyScore = 0.0f;

    float allyMobilityScore = 0.0f;
    float enemyMobilityScore = 0.0f;

    BoardDimensions dimensions = board.getDimensions();

    std::vector<std::vector<float>> centerControlMap = {
        {0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f},
        {0.8f, 1.0f, 1.2f, 1.4f, 1.4f, 1.2f, 1.0f, 0.8f},
        {0.8f, 1.2f, 1.4f, 1.6f, 1.6f, 1.4f, 1.2f, 0.8f},
        {0.8f, 1.2f, 1.4f, 1.8f, 1.8f, 1.4f, 1.2f, 0.8f},
        {0.8f, 1.2f, 1.4f, 1.8f, 1.8f, 1.4f, 1.2f, 0.8f},
        {0.8f, 1.2f, 1.4f, 1.6f, 1.6f, 1.4f, 1.2f, 0.8f},
        {0.8f, 1.0f, 1.2f, 1.4f, 1.4f, 1.2f, 1.0f, 0.8f},
        {0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f, 0.8f}
    };


    for (int r = 0; r < dimensions.rows; ++r) {
        for (int c = 0; c < dimensions.cols; ++c) {
            const Piece* piece = board.getPieceAt(Position(r, c));
            if (!piece) continue;
            int moveCount = static_cast<int>(piece->getPossibleMoves(board).size());

            float centerScore = centerControlMap[r][c];
            if (piece->getColor() == perspective) {
                allyMaterial += piece->getValue();
                allyMobilityScore += moveCount;
                allyCenterControlScore += centerScore; 
                
            } else {
                enemyMaterial += piece->getValue();
                enemyMobilityScore += moveCount;
                enemyCenterControlScore += centerScore;
            }
            
        }
    }

    // score double pawns for pawn structure

    for (int c = 0; c < dimensions.cols; ++c) {
        int allyPawnCount = 0;
        int enemyPawnCount = 0;

        for (int r = 0; r < dimensions.rows; ++r) {
            const Piece* piece = board.getPieceAt(Position(r, c));
            if (!piece || piece->getType() != PieceType::PAWN) continue;
            
            Position bottomLeft(-1, c - 1);
            Position bottomRight(-1, c + 1);

            if (perspective == Color::WHITE) {
                bottomRight.row = r + 1;
                bottomLeft.row = r + 1;
            }
            else  {
                bottomRight.row = r - 1;
                bottomLeft.row = r - 1;
            }

            const Piece* bottomLeftPiece = board.getPieceAt(bottomRight);
            const Piece* bottomRightPiece = board.getPieceAt(bottomLeft);

            if (piece->getColor() == perspective) {
                allyPawnCount++;
                bottomLeftPiece && bottomLeftPiece->getType() == PieceType::PAWN ? allyPawnStructureScore += 0.05f : allyPawnStructureScore -=0.025f;
                bottomRightPiece && bottomRightPiece->getType() == PieceType::PAWN ? allyPawnStructureScore += 0.05f : allyPawnStructureScore -=0.025f;
            }
            else {
                enemyPawnCount++;
                bottomLeftPiece && bottomLeftPiece->getType() == PieceType::PAWN ? enemyPawnStructureScore += 0.05f : enemyPawnStructureScore -= 0.025f;
                bottomRightPiece && bottomRightPiece->getType() == PieceType::PAWN ? enemyPawnStructureScore += 0.05f : enemyPawnStructureScore -= 0.025f;
            }            
        }

        if (allyPawnCount == 2) allyPawnStructureScore -=0.1f;
        else if (allyPawnCount == 3) allyPawnStructureScore -= 0.25f;

        if (enemyPawnCount == 2) enemyPawnStructureScore -=0.1f;
        else if (enemyPawnCount == 3) enemyPawnStructureScore -= 0.25f;
    }

    // score isolated pawns



    Position allyKingPos = board.findKing(perspective);
    Position enemyKingPos = board.findKing(perspective == Color::WHITE ? Color::BLACK : Color::WHITE);

    if (perspective == Color::WHITE) {
        for (int r = allyKingPos.row; r >= 0; --r) {
            const Piece* allyFilePiece = board.getPieceAt(Position(r, allyKingPos.col));
            if (allyFilePiece) allyKingSafetyScore += (allyFilePiece->getColor() == perspective ? 1 : 0);
        }
        for (int r = enemyKingPos.row; r < dimensions.rows; ++r) {
            const Piece* enemyFilePiece = board.getPieceAt(Position(r, enemyKingPos.col));
            if (enemyFilePiece) enemyKingSafetyScore += (enemyFilePiece->getColor() != perspective ? 1 : 0);
        }
    } else {
        for (int r = enemyKingPos.row; r >= 0; --r) {
            const Piece* enemyFilePiece = board.getPieceAt(Position(r, allyKingPos.col));
            if (enemyFilePiece) allyKingSafetyScore += (enemyFilePiece->getColor() != perspective ? 1 : 0);
        }
        for (int r = allyKingPos.row; r < dimensions.rows; ++r) {
            const Piece* allyFilePiece = board.getPieceAt(Position(r, allyKingPos.col));
            if (allyFilePiece) enemyKingSafetyScore += (allyFilePiece->getColor() == perspective ? 1 : 0);
        }
    }

    
    
    float materialScore = materialWeight * (allyMaterial - enemyMaterial);
    float pawnStructureScore = pawnStructureWeight * (allyPawnStructureScore - enemyPawnStructureScore);
    float centerControlScore = centerControlWeight * (allyCenterControlScore - enemyCenterControlScore); 
    float kingSafetyScore = kingSafetyWeight * (allyKingSafetyScore - enemyKingSafetyScore);
    float mobilityScore = mobilityWeight * (allyMobilityScore - enemyMobilityScore);

    float score = materialScore + pawnStructureScore + centerControlScore + kingSafetyScore + mobilityScore;

    if (report) {
        auto round3 = [](float val) { return std::round(val * 1000.0f) / 1000.0f; };
        std::cout << "Static evaluation score: " << round3(score) << std::endl;
        std::cout << "  Material: " << round3(materialScore) << " | (WGT=" << materialWeight << ")" << std::endl;
        std::cout << "  Pawn structure: " << round3(pawnStructureScore) << " | (WGT=" << pawnStructureWeight << ")" << std::endl;
        std::cout << "  Center control: " << round3(centerControlScore) << " | (WGT=" << centerControlWeight << ")" << std::endl;
        std::cout << "  King safety: " << round3(kingSafetyScore) << " | (WGT=" << kingSafetyWeight << ")" << std::endl;
        std::cout << "  Mobility: " << round3(mobilityScore) << " | (WGT=" << mobilityWeight << ")" << std::endl;
    }
    
    // Adjust score based on perspective
    return (perspective == Color::WHITE) ? score : -score;
;
}


EvaluationResult EvaluationEngine::search(Game game, int depth, float alpha, float beta, bool isMaximizingTurn, Color originalPlayerColor) const {
    EvaluationResult currentEval;
    currentEval.nodesSearched = 1;


    std::vector<Move> legalMoves = game.getLegalMoves(); // Get moves for current player in 'game'

    if (game.getHalfMoveClock() == 100 || game.getGameStateCount() >= 3) {
        currentEval.score = 0;
        return currentEval;
    }

    // Base cases for recursion
    if (depth == 0) {
        currentEval.score = staticEvaluate(game.getBoard(), originalPlayerColor);
        // No bestMove at leaf node of this type
        return currentEval;
    }

    
    if (legalMoves.empty()) {
        if (game.isKingInCheck(game.getCurrentPlayerColor())) { // Checkmate
            currentEval.score = isMaximizingTurn ? -INFINITY_SCORE : INFINITY_SCORE; // Current player (whose turn it is) is checkmated
        } else { // Stalemate
            currentEval.score = 0.0f;
        }
        return currentEval;
    }

    
    
    // Order moves for better alpha-beta pruning
    legalMoves = orderMoves(legalMoves, game.getBoard());

    if (isMaximizingTurn) { // Corresponds to originalPlayerColor's turn
        float maxEval = -INFINITY_SCORE;
        Move bestMoveSoFar = legalMoves.empty() ? Move(Position(-1,-1), Position(-1,-1)) : legalMoves[0];

        for (const auto& move : legalMoves) {
            Game nextGameState = game.clone(); // Create a copy to simulate the move
            nextGameState.makeMove(move);   // makeMove switches player and updates game state

            EvaluationResult result = search(std::move(nextGameState), depth - 1, alpha, beta, false, originalPlayerColor);
            currentEval.nodesSearched += result.nodesSearched;

            if (result.score > maxEval) {
                maxEval = result.score;
                bestMoveSoFar = move;
            }
            alpha = std::max(alpha, result.score);
            if (beta <= alpha) {
                break; // Beta cut-off
            }
        }
        currentEval.score = maxEval;
        currentEval.bestMove = bestMoveSoFar;
    } else { // Minimizing player's turn (opponent of originalPlayerColor)
        float minEval = INFINITY_SCORE;
        Move bestMoveSoFar = legalMoves.empty() ? Move(Position(-2,-2), Position(-1,-1)) : legalMoves[0];


        for (const auto& move : legalMoves) {
            Game nextGameState = game.clone(); // Create a copy
            nextGameState.makeMove(move);

            EvaluationResult result = search(std::move(nextGameState), depth - 1, alpha, beta, true, originalPlayerColor);
            currentEval.nodesSearched += result.nodesSearched;

            if (result.score < minEval) {
                minEval = result.score;
                bestMoveSoFar = move; // This move is from opponent's perspective, not usually returned for original player
            }
            
            beta = std::min(beta, result.score);
            if (beta <= alpha) {
                break; // Alpha cut-off
            }
        }
        currentEval.score = minEval;
        currentEval.bestMove = bestMoveSoFar;
    }
    return currentEval;
}


Move EvaluationEngine::findBestMove(const Game& game, int depth) const {
    if (depth <= 0) depth = 1; // Ensure at least depth 1

    // The 'game' state here is the current actual game state.
    // The 'search' function will work on copies.
    // Determine if the current player in 'game' is the one we are maximizing for.
    // The score in staticEvaluate is from White's perspective.
    // So, if current player is White, it's a maximizing turn. If Black, it's a minimizing turn
    // in terms of that raw score.
    // Let's define maximizingPlayer based on whose turn it is in the *initial* call.
    // The 'search' function's 'maximizingPlayer' bool will alternate.
    // 'originalPlayerColor' in search helps interpret the score from a consistent view.

    Color playerToMove = game.getCurrentPlayerColor();
    bool isWhiteToMove = (playerToMove == Color::WHITE);

    std::cout << "Engine searching for best move for " << (isWhiteToMove ? "White" : "Black") << " at depth " << depth << std::endl;
    
    // Initial call to search:
    // - 'game' is the current game state.
    // - 'depth' is the requested search depth.
    // - alpha, beta initialized.
    // - 'isMaximizingTurn' is true if playerToMove == originalPlayerColor (which is playerToMove here)
    // - 'originalPlayerColor' is playerToMove.
    // The search function needs to know whose perspective "maximizing" refers to.
    // Let's make the score relative to the player whose turn it is initially.
    // So, staticEvaluate should return score relative to current player,
    // or search needs to flip score if originalPlayerColor is BLACK.
    // For simplicity, let's assume staticEvaluate returns score relative to WHITE.
    // The 'search' function will then know if it's maximizing this (if White is originalPlayerColor)
    // or minimizing this (if Black is originalPlayerColor).

    EvaluationResult result = search(game.clone(), depth, -INFINITY_SCORE, INFINITY_SCORE, isWhiteToMove, playerToMove);

    std::cout << "Nodes searched: " << result.nodesSearched << std::endl;
    std::cout << "Best move found: " << result.bestMove.toString() << " with score: " << result.score << std::endl;
    
    // If no moves are possible (checkmate/stalemate), result.bestMove might be invalid.
    // Game loop should handle this (e.g., by game state).
    if (game.getLegalMoves().empty()) {
        std::cout << "No legal moves available, returning invalid move from engine." << std::endl;
        return Move(Position(-1,-1), Position(-1,-1));
    }
    
    return result.bestMove;
}