#include "core/Bishop.h"
#include "core/Board.h"

Bishop::Bishop(Color c, Position pos) : Piece(c, pos, PieceType::BISHOP, 3.2f) {} // Often valued slightly > Knight

char Bishop::getSymbol() const {
    return 'B';
}

std::unique_ptr<Piece> Bishop::clone() const {
    return std::make_unique<Bishop>(*this);
}

std::vector<Move> Bishop::getPossibleMoves(const Board& board) const {
    std::vector<Move> moves;
    Position currentPos = getPosition();
    BoardDimensions dims = board.getDimensions();

    int DIRS[][2] = {{1, 1}, {1, -1}, {-1, 1}, {-1, -1}}; // Diagonal directions

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