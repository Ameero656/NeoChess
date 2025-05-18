#include "core/Queen.h"
#include "core/Board.h"
#include "core/Rook.h"   // For combined logic
#include "core/Bishop.h" // For combined logic

Queen::Queen(Color c, Position pos) : Piece(c, pos, PieceType::QUEEN, 9.0f) {}

char Queen::getSymbol() const {
    return 'Q';
}

std::unique_ptr<Piece> Queen::clone() const {
    return std::make_unique<Queen>(*this);
}

std::vector<Move> Queen::getPossibleMoves(const Board& board) const {
    std::vector<Move> moves;
    Position currentPos = getPosition();
    BoardDimensions dims = board.getDimensions();

    // Combine Rook and Bishop logic
    int DIRS[][2] = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0}, // Rook directions
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // Bishop directions
    };

    for (auto& dir : DIRS) {
        for (int i = 1; i < std::max(dims.rows, dims.cols); ++i) {
            Position nextPos(currentPos.row + dir[0] * i, currentPos.col + dir[1] * i);
            if (!nextPos.isValid(dims.rows, dims.cols)) {
                break; // Off board
            }
            const Piece* targetPiece = board.getPieceAt(nextPos);
            if (targetPiece == nullptr) {
                moves.emplace_back(currentPos, nextPos);
            } else {
                if (targetPiece->getColor() != color) {
                    moves.emplace_back(currentPos, nextPos); // Capture
                }
                break; // Blocked
            }
        }
    }
    return moves;
}