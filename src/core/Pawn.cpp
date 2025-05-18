#include "core/Pawn.h"
#include "core/Board.h" // Required for Board context in getPossibleMoves
#include "core/ChessTypes.h"

Pawn::Pawn(Color c, Position pos) : Piece(c, pos, PieceType::PAWN, 1.0f) {}

char Pawn::getSymbol() const {
    return 'P';
}

std::unique_ptr<Piece> Pawn::clone() const {
    return std::make_unique<Pawn>(*this);
}

std::vector<Move> Pawn::getPossibleMoves(const Board& board) const {
    std::vector<Move> moves;
    int direction = (color == Color::WHITE) ? -1 : 1; // White moves from higher row to lower, Black vice-versa
                                                      // Assuming row 0 is top of board (e.g. Black's back rank for 8x8)
                                                      // If row 0 is White's back rank, this needs to be flipped.
                                                      // Let's assume standard chess setup: White starts rows 6,7. Black 0,1.
                                                      // So white moves from row 6 to 0. Black from row 1 to 7.
    if (color == Color::WHITE) direction = -1; // Moves "up" the board (decreasing row index)
    else direction = 1; // Moves "down" the board (increasing row index)


    Position currentPos = getPosition();
    BoardDimensions dims = board.getDimensions();

    // 1. Forward one square
    Position oneStep(currentPos.row + direction, currentPos.col);
    if (oneStep.isValid(dims.rows, dims.cols) && board.getPieceAt(oneStep) == nullptr) {
        // Check for promotion
        if (oneStep.row == 0 || oneStep.row == dims.rows - 1) {
            moves.emplace_back(currentPos, oneStep, PieceType::QUEEN); // Default to Queen
            moves.emplace_back(currentPos, oneStep, PieceType::ROOK);
            moves.emplace_back(currentPos, oneStep, PieceType::BISHOP);
            moves.emplace_back(currentPos, oneStep, PieceType::KNIGHT);
        } else {
            moves.emplace_back(currentPos, oneStep);
        }

        // 2. Forward two squares (if first move)
        if (!getHasMoved()) {
            Position twoSteps(currentPos.row + 2 * direction, currentPos.col);
            if (twoSteps.isValid(dims.rows, dims.cols) && board.getPieceAt(twoSteps) == nullptr) {
                // Note: This move could make this pawn eligible for en passant by opponent
                moves.emplace_back(currentPos, twoSteps);
            }
        }
    }

    // 3. Captures
    int captureOffsets[] = {-1, 1};
    for (int offset : captureOffsets) {
        Position capturePos(currentPos.row + direction, currentPos.col + offset);
        if (capturePos.isValid(dims.rows, dims.cols)) {
            const Piece* targetPiece = board.getPieceAt(capturePos);
            if (targetPiece != nullptr && targetPiece->getColor() != color) {
                 // Check for promotion on capture
                if (capturePos.row == 0 || capturePos.row == dims.rows - 1) {
                    moves.emplace_back(currentPos, capturePos, PieceType::QUEEN);
                    moves.emplace_back(currentPos, capturePos, PieceType::ROOK);
                    moves.emplace_back(currentPos, capturePos, PieceType::BISHOP);
                    moves.emplace_back(currentPos, capturePos, PieceType::KNIGHT);
                } else {
                    moves.emplace_back(currentPos, capturePos);
                }
            }
            // 4. En Passant (More complex, requires knowledge of the last move made in the game)
            // This requires the Board or Game to provide info on the last move
            // For now, we'll placeholder it. A full implementation would check:
            // - If the opponent's last move was a two-square pawn advance
            // - If that pawn landed adjacent to this pawn
            // - If this pawn is on its 5th rank (for white) or 4th rank (for black)

            Position enPassantTargetSquare(currentPos.row + direction, currentPos.col + offset);
            Position opponentPawnSquare(currentPos.row, currentPos.col + offset);

            const Move* lastMove = board.getLastMove(); // Board needs to store this
            if (lastMove && lastMove->to == opponentPawnSquare) {
                 const Piece* lastMovedPiece = board.getPieceAt(opponentPawnSquare);
                 if (lastMovedPiece && lastMovedPiece->getType() == PieceType::PAWN &&
                     lastMovedPiece->getColor() != color &&
                     std::abs(lastMove->to.row - lastMove->from.row) == 2) { // Opponent pawn moved two squares
                     // Check if current pawn is in the correct rank for en passant
                     bool canEnPassant = false;
                     if (color == Color::WHITE && currentPos.row == 3 && dims.rows == 8) { // White pawn on 5th rank (row 3 if 0-indexed from black side)
                         canEnPassant = true;
                     } else if (color == Color::BLACK && currentPos.row == 4 && dims.rows == 8) { // Black pawn on 4th rank (row 4)
                         canEnPassant = true;
                     }
                     // Add more general rank checks if board size is variable.
                     // For a standard 8x8 board where white starts on rank 2 (row 6) and black on rank 7 (row 1):
                     // White pawn on rank 5 (row 3 for 0-indexed, 0 is black's back rank)
                     // Black pawn on rank 4 (row 4 for 0-indexed)
                     if ( (color == Color::WHITE && currentPos.row == (dims.rows / 2) - 1 ) ||
                          (color == Color::BLACK && currentPos.row == (dims.rows / 2) ) ) {
                        // This is a simplified check. Actual rank depends on board setup.
                        // For standard 8x8 (0-7 rows): White on row 3, Black on row 4.
                        if ( (color == Color::WHITE && currentPos.row == 3 && dims.rows == 8) ||
                             (color == Color::BLACK && currentPos.row == 4 && dims.rows == 8) ) {
                             moves.emplace_back(currentPos, enPassantTargetSquare, PieceType::EMPTY, false, true);
                        }
                     }
                 }
            }
        }
    }
    return moves;
}