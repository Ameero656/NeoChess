#include "core/Rook.h"
#include "core/Board.h"

Rook::Rook(Color c, Position pos) : Piece(c, pos, PieceType::ROOK, 5.0f) {}

char Rook::getSymbol() const {
    return 'R';
}

std::unique_ptr<Piece> Rook::clone() const {
    return std::make_unique<Rook>(*this);
}

std::vector<Move> Rook::getPossibleMoves(const Board& board) const {
    std::vector<Move> moves;
    Position currentPos = getPosition();
    BoardDimensions dims = board.getDimensions();

    int DIRS[][2] = {{0, 1}, {0, -1}, {1, 0}, {-1, 0}}; // Right, Left, Down, Up

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