#include "core/Knight.h"
#include "core/Board.h"

Knight::Knight(Color c, Position pos) : Piece(c, pos, PieceType::KNIGHT, 3.0f) {}

char Knight::getSymbol() const {
    return 'N';
}

std::unique_ptr<Piece> Knight::clone() const {
    return std::make_unique<Knight>(*this);
}

std::vector<Move> Knight::getPossibleMoves(const Board& board) const {
    std::vector<Move> moves;
    Position currentPos = getPosition();
    BoardDimensions dims = board.getDimensions();

    int DIRS[][2] = {
        {2, 1}, {2, -1}, {-2, 1}, {-2, -1},
        {1, 2}, {1, -2}, {-1, 2}, {-1, -2}
    };

    for (auto& dir : DIRS) {
        Position nextPos(currentPos.row + dir[0], currentPos.col + dir[1]);
        if (nextPos.isValid(dims.rows, dims.cols)) {
            const Piece* targetPiece = board.getPieceAt(nextPos);
            if (targetPiece == nullptr || targetPiece->getColor() != color) {
                moves.emplace_back(currentPos, nextPos);
            }
        }
    }
    return moves;
}