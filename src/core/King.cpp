#include "core/King.h"
#include "core/Board.h"

King::King(Color c, Position pos) : Piece(c, pos, PieceType::KING, 1000.0f) {} // Effectively infinite value

char King::getSymbol() const {
    return 'K';
}

std::unique_ptr<Piece> King::clone() const {
    return std::make_unique<King>(*this);
}

std::vector<Move> King::getPossibleMoves(const Board& board) const {
    std::vector<Move> moves;
    Position currentPos = getPosition();
    BoardDimensions dims = board.getDimensions();

    int DIRS[][2] = {
        {0, 1}, {0, -1}, {1, 0}, {-1, 0}, // Orthogonal
        {1, 1}, {1, -1}, {-1, 1}, {-1, -1} // Diagonal
    };

    for (auto& dir : DIRS) {
        Position nextPos(currentPos.row + dir[0], currentPos.col + dir[1]);
        if (nextPos.isValid(dims.rows, dims.cols)) {
            const Piece* targetPiece = board.getPieceAt(nextPos);
            // Basic move: to empty square or capture opponent
            // IMPORTANT: This does NOT check if the king moves into check.
            // That check is typically done by the Game/Board logic after a pseudo-legal move is generated.
            if (targetPiece == nullptr || targetPiece->getColor() != color) {
                moves.emplace_back(currentPos, nextPos);
            }
        }
    }

    // Castling (Simplified: relies on Board to verify path clear and not in check)
    // Full castling logic is complex and usually sits in the Game or Board class
    // to check intermediate squares and check status.
    // Here, we generate the *intent* to castle.
    if (!getHasMoved() && dims.rows == 8 && dims.cols == 8) { // Standard board for castling
        // Kingside (O-O)
        // Check if rook exists, hasn't moved, and path is clear.
        // Board::canCastleKingside(color) would be a helper.
        // For now, just generate the move; Game logic will validate it fully.
        Position kingsideRookPos(currentPos.row, dims.cols - 1);
        const Piece* kRook = board.getPieceAt(kingsideRookPos);
        if (kRook && kRook->getType() == PieceType::ROOK && kRook->getColor() == color && !kRook->getHasMoved()) {
            bool pathClear = true;
            for (int c = currentPos.col + 1; c < kingsideRookPos.col; ++c) {
                if (board.getPieceAt({currentPos.row, c}) != nullptr) {
                    pathClear = false;
                    break;
                }
            }
            if (pathClear) {
                 // The move is to the king's new square
                moves.emplace_back(currentPos, Position(currentPos.row, currentPos.col + 2), PieceType::EMPTY, true);
            }
        }

        // Queenside (O-O-O)
        Position queensideRookPos(currentPos.row, 0);
        const Piece* qRook = board.getPieceAt(queensideRookPos);
        if (qRook && qRook->getType() == PieceType::ROOK && qRook->getColor() == color && !qRook->getHasMoved()) {
            bool pathClear = true;
            for (int c = currentPos.col - 1; c > queensideRookPos.col; --c) {
                if (board.getPieceAt({currentPos.row, c}) != nullptr) {
                    pathClear = false;
                    break;
                }
            }
            if (pathClear) {
                moves.emplace_back(currentPos, Position(currentPos.row, currentPos.col - 2), PieceType::EMPTY, true);
            }
        }
    }
    return moves;
}