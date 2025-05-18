#include "player/HumanPlayer.h"
#include "core/Game.h"   // For Game context
#include "core/Position.h" // For Position::fromAlgebraic
#include <string>
#include <vector>
#include <limits> // Required for std::numeric_limits
#include <algorithm> // For std::find

HumanPlayer::HumanPlayer(Color color, std::string name) : Player(color, name) {}

Move HumanPlayer::getMove(const Game& game, const EvaluationEngine* engine) const {
    std::string fromAlg, toAlg;
    Move chosenMove(Position(-1,-1), Position(-1,-1)); // Invalid default
    bool validMoveChosen = false;

    const auto& legalMoves = game.getLegalMoves(); // Get all legal moves for the current player

    if (legalMoves.empty()) {
        // This should ideally be handled by the Game state (checkmate/stalemate)
        // but as a fallback, if getMove is called, return an invalid move.
        std::cout << "No legal moves available for " << getName() << "." << std::endl;
        return chosenMove; // Return an invalid move
    }

    while (!validMoveChosen) {
        std::cout << getName() << " (" << (playerColor == Color::WHITE ? "White" : "Black")
                  << "), enter your move (e.g., e2 e4): ";
        std::cin >> fromAlg >> toAlg;

        // Clear error flags and discard rest of the line for bad input
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cout << "Invalid input format. Please use algebraic notation (e.g., a1 b2)." << std::endl;
            continue;
        }


        try {
            Position fromPos = Position::fromAlgebraic(fromAlg);
            Position toPos = Position::fromAlgebraic(toAlg);

            // Check if this move (fromPos, toPos) is in the list of legal moves.
            // This needs to handle promotion choices if any.
            // For simplicity, we'll prompt for promotion if a pawn reaches the back rank.

            // Construct a potential move to check against legal moves
            // This simplified version doesn't immediately handle promotion input.
            // A better approach would check if multiple legal moves share from/to but differ in promotion.
            Move potentialMove(fromPos, toPos);
            
            std::vector<Move> matchingMoves;
            for(const auto& legal : legalMoves) {
                if (legal.from == fromPos && legal.to == toPos) {
                    matchingMoves.push_back(legal);
                }
            }

            if (matchingMoves.empty()) {
                std::cout << "That is not a legal move. Try again." << std::endl;
            } else if (matchingMoves.size() == 1) {
                chosenMove = matchingMoves[0];
                validMoveChosen = true;
            } else {
                // Multiple moves match (must be promotions)
                const Piece* piece = game.getBoard().getPieceAt(fromPos);
                if (piece && piece->getType() == PieceType::PAWN &&
                    (toPos.row == 0 || toPos.row == game.getBoard().getDimensions().rows - 1)) {
                    
                    std::cout << "Promote pawn to (Q, R, B, N): ";
                    char promoChar;
                    std::cin >> promoChar;
                    promoChar = std::toupper(promoChar);
                    PieceType promoType = PieceType::EMPTY;

                    switch (promoChar) {
                        case 'Q': promoType = PieceType::QUEEN; break;
                        case 'R': promoType = PieceType::ROOK; break;
                        case 'B': promoType = PieceType::BISHOP; break;
                        case 'N': promoType = PieceType::KNIGHT; break;
                        default: std::cout << "Invalid promotion choice. Defaulting to Queen." << std::endl; promoType = PieceType::QUEEN; break;
                    }
                    
                    bool promotionMatchFound = false;
                    for(const auto& legal : matchingMoves) {
                        if (legal.promotionPiece == promoType) {
                            chosenMove = legal;
                            validMoveChosen = true;
                            promotionMatchFound = true;
                            break;
                        }
                    }
                    if (!promotionMatchFound) {
                         std::cout << "Selected promotion is not valid for this move. Try again." << std::endl;
                    }

                } else {
                    // Should not happen if not promotion
                    std::cout << "Ambiguous move, but not a promotion. This is unexpected. Try again." << std::endl;
                }
            }

        } catch (const std::invalid_argument& e) {
            std::cout << "Invalid algebraic notation: " << e.what() << " Try again." << std::endl;
        }
    }
    return chosenMove;
}