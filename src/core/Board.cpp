// src/core/Board.cpp

#include "core/Board.h"
#include "core/Pawn.h"
#include "core/Rook.h"
#include "core/Knight.h"
#include "core/Bishop.h"
#include "core/Queen.h"
#include "core/King.h"
#include <stdexcept> // For out_of_range
#include <vector>    // Ensure vector is included for swap

// Constructor
Board::Board(int rows, int cols) : dimensions({rows, cols}), lastMove(nullptr),
                                   whiteCanCastleKingside(true), whiteCanCastleQueenside(true),
                                   blackCanCastleKingside(true), blackCanCastleQueenside(true),
                                   enPassantTargetSquare(-1, -1) {
    initializeEmptyBoard();
}

// Deep copy constructor
Board::Board(const Board& other)
    : dimensions(other.dimensions),
      lastMove(nullptr), 
      whiteCanCastleKingside(other.whiteCanCastleKingside),
      whiteCanCastleQueenside(other.whiteCanCastleQueenside),
      blackCanCastleKingside(other.blackCanCastleKingside),
      blackCanCastleQueenside(other.blackCanCastleQueenside),
      enPassantTargetSquare(other.enPassantTargetSquare) {
    
    grid.resize(dimensions.rows); 
    for (int r = 0; r < dimensions.rows; ++r) {
        grid[r].resize(dimensions.cols); // CORRECT: Default-constructs unique_ptrs to nullptr
        for (int c = 0; c < dimensions.cols; ++c) {
            if (other.grid[r][c]) {
                grid[r][c] = other.grid[r][c]->clone(); 
            } else {
                grid[r][c] = nullptr; 
            }
        }
    }
}

// Deep copy assignment operator (using build-new-and-swap)
Board& Board::operator=(const Board& other) {
    if (this == &other) {
        return *this; 
    }

    dimensions = other.dimensions;
    lastMove = nullptr; 
    whiteCanCastleKingside = other.whiteCanCastleKingside;
    whiteCanCastleQueenside = other.whiteCanCastleQueenside;
    blackCanCastleKingside = other.blackCanCastleKingside;
    blackCanCastleQueenside = other.blackCanCastleQueenside;
    enPassantTargetSquare = other.enPassantTargetSquare;

    std::vector<std::vector<std::unique_ptr<Piece>>> new_grid(other.dimensions.rows);
    for (int r = 0; r < other.dimensions.rows; ++r) {
        new_grid[r].resize(other.dimensions.cols); // CORRECT: Default-constructs unique_ptrs to nullptr
        for (int c = 0; c < other.dimensions.cols; ++c) {
            if (other.grid[r][c]) {
                new_grid[r][c] = other.grid[r][c]->clone(); 
            } else {
                new_grid[r][c] = nullptr; 
            }
        }
    }
    grid.swap(new_grid);
    return *this;
}

// Move constructor
Board::Board(Board&& other) noexcept
    : grid(std::move(other.grid)), 
      dimensions(other.dimensions),
      lastMove(other.lastMove), 
      whiteCanCastleKingside(other.whiteCanCastleKingside),
      whiteCanCastleQueenside(other.whiteCanCastleQueenside),
      blackCanCastleKingside(other.blackCanCastleKingside),
      blackCanCastleQueenside(other.blackCanCastleQueenside),
      enPassantTargetSquare(other.enPassantTargetSquare) {
    other.lastMove = nullptr;
    other.enPassantTargetSquare = Position(-1,-1);
}

// Move assignment operator
Board& Board::operator=(Board&& other) noexcept {
    if (this == &other) {
        return *this; 
    }
    grid = std::move(other.grid);
    dimensions = other.dimensions;
    lastMove = other.lastMove; 
    whiteCanCastleKingside = other.whiteCanCastleKingside;
    whiteCanCastleQueenside = other.whiteCanCastleQueenside;
    blackCanCastleKingside = other.blackCanCastleKingside;
    blackCanCastleQueenside = other.blackCanCastleQueenside;
    enPassantTargetSquare = other.enPassantTargetSquare;

    other.lastMove = nullptr;
    other.enPassantTargetSquare = Position(-1,-1);
    return *this;
}


void Board::initializeEmptyBoard() {
    for (auto& row_vec : grid) { 
        row_vec.clear();
    }
    grid.clear();

    grid.resize(dimensions.rows);
    for (int r = 0; r < dimensions.rows; ++r) {
        // Use resize that default-constructs unique_ptrs (to nullptr)
        grid[r].resize(dimensions.cols); // CORRECTED: Was grid[r].resize(dimensions.cols, nullptr);
    }

    lastMove = nullptr;
    whiteCanCastleKingside = true;
    whiteCanCastleQueenside = true;
    blackCanCastleKingside = true;
    blackCanCastleQueenside = true;
    enPassantTargetSquare = Position(-1, -1);
}

// ... (rest of Board.cpp remains the same as the previous full version) ...
// Ensure initializeDefaultSetup, getPieceAt, addPiece, removePiece, performMove,
// getDimensions, en passant methods, castling methods, getLastMove, setLastMove,
// findPieces, findKing, isSquareAttacked are all present and correct from the
// previous full Board.cpp update. The change here is specifically in initializeEmptyBoard
// and ensuring the copy constructor/assignment also use the correct resize form.

void Board::initializeDefaultSetup() {
    initializeEmptyBoard(); 

    if (dimensions.rows != 8 || dimensions.cols != 8) {
        throw std::runtime_error("Default setup is designed for 8x8 board.");
    }

    // Black pieces (standard chess: row 0 is Black's back rank)
    grid[0][0] = std::make_unique<Rook>(Color::BLACK, Position(0, 0));
    grid[0][1] = std::make_unique<Knight>(Color::BLACK, Position(0, 1));
    grid[0][2] = std::make_unique<Bishop>(Color::BLACK, Position(0, 2));
    grid[0][3] = std::make_unique<Queen>(Color::BLACK, Position(0, 3));
    grid[0][4] = std::make_unique<King>(Color::BLACK, Position(0, 4));
    grid[0][5] = std::make_unique<Bishop>(Color::BLACK, Position(0, 5));
    grid[0][6] = std::make_unique<Knight>(Color::BLACK, Position(0, 6));
    grid[0][7] = std::make_unique<Rook>(Color::BLACK, Position(0, 7));
    for (int c = 0; c < 8; ++c) {
        grid[1][c] = std::make_unique<Pawn>(Color::BLACK, Position(1, c));
    }

    // White pieces (standard chess: row 7 is White's back rank)
    for (int c = 0; c < 8; ++c) {
        grid[6][c] = std::make_unique<Pawn>(Color::WHITE, Position(6, c));
    }
    grid[7][0] = std::make_unique<Rook>(Color::WHITE, Position(7, 0));
    grid[7][1] = std::make_unique<Knight>(Color::WHITE, Position(7, 1));
    grid[7][2] = std::make_unique<Bishop>(Color::WHITE, Position(7, 2));
    grid[7][3] = std::make_unique<Queen>(Color::WHITE, Position(7, 3));
    grid[7][4] = std::make_unique<King>(Color::WHITE, Position(7, 4));
    grid[7][5] = std::make_unique<Bishop>(Color::WHITE, Position(7, 5));
    grid[7][6] = std::make_unique<Knight>(Color::WHITE, Position(7, 6));
    grid[7][7] = std::make_unique<Rook>(Color::WHITE, Position(7, 7));

    whiteCanCastleKingside = true;
    whiteCanCastleQueenside = true;
    blackCanCastleKingside = true;
    blackCanCastleQueenside = true;
    enPassantTargetSquare = Position(-1,-1);
    lastMove = nullptr;
}


const Piece* Board::getPieceAt(Position pos) const {
    if (!pos.isValid(dimensions.rows, dimensions.cols)) {
        return nullptr; 
    }
    return grid[pos.row][pos.col].get();
}

Piece* Board::getPieceAt(Position pos) {
    if (!pos.isValid(dimensions.rows, dimensions.cols)) {
        return nullptr; 
    }
    return grid[pos.row][pos.col].get();
}

void Board::addPiece(std::unique_ptr<Piece> piece, Position pos) {
    if (!pos.isValid(dimensions.rows, dimensions.cols)) {
        throw std::out_of_range("Position out of board bounds in addPiece.");
    }
    if (piece) {
        piece->setPosition(pos); 
        grid[pos.row][pos.col] = std::move(piece);
    } else {
        grid[pos.row][pos.col] = nullptr; 
    }
}

std::unique_ptr<Piece> Board::removePiece(Position pos) {
    if (!pos.isValid(dimensions.rows, dimensions.cols) || !grid[pos.row][pos.col]) {
        return nullptr; 
    }
    return std::move(grid[pos.row][pos.col]); 
}

std::unique_ptr<Piece> Board::performMove(const Move& move) {
    Piece* movingPieceOriginalPtr = getPieceAt(move.from); 
    if (!movingPieceOriginalPtr) {
         throw std::runtime_error("performMove: No piece at source square " + move.from.toAlgebraic());
    }

    std::unique_ptr<Piece> pieceFromSource = removePiece(move.from); 
    std::unique_ptr<Piece> capturedPiece = nullptr;

    if (move.isEnPassantCapture && movingPieceOriginalPtr->getType() == PieceType::PAWN) {
        Position capturedPawnActualPos(move.from.row, move.to.col); 
        if (capturedPawnActualPos.isValid(dimensions.rows, dimensions.cols)) {
            capturedPiece = removePiece(capturedPawnActualPos);
        }
    } else {
        if (getPieceAt(move.to)) { 
            capturedPiece = removePiece(move.to);
        }
    }

    addPiece(std::move(pieceFromSource), move.to); 
    movingPieceOriginalPtr->setHasMoved(true); 

    if (move.promotionPiece != PieceType::EMPTY && movingPieceOriginalPtr->getType() == PieceType::PAWN) {
        bool atPromotionRank = (movingPieceOriginalPtr->getColor() == Color::WHITE && move.to.row == 0) ||
                               (movingPieceOriginalPtr->getColor() == Color::BLACK && move.to.row == dimensions.rows - 1);
        if (atPromotionRank) {
            std::unique_ptr<Piece> promotedPiece;
            Color color = movingPieceOriginalPtr->getColor();
            switch (move.promotionPiece) {
                case PieceType::QUEEN:  promotedPiece = std::make_unique<Queen>(color, move.to); break;
                case PieceType::ROOK:   promotedPiece = std::make_unique<Rook>(color, move.to);   break;
                case PieceType::BISHOP: promotedPiece = std::make_unique<Bishop>(color, move.to); break;
                case PieceType::KNIGHT: promotedPiece = std::make_unique<Knight>(color, move.to); break;
                default: break; 
            }
            if (promotedPiece) {
                promotedPiece->setHasMoved(true); 
                addPiece(std::move(promotedPiece), move.to); 
            }
        }
    }

    if (move.isCastling && movingPieceOriginalPtr->getType() == PieceType::KING) {
        Position rookFromPos, rookToPos;
        if (move.to.col > move.from.col) { 
            rookFromPos = Position(move.from.row, dimensions.cols - 1); 
            rookToPos   = Position(move.from.row, move.to.col - 1);   
        } else { 
            rookFromPos = Position(move.from.row, 0);                 
            rookToPos   = Position(move.from.row, move.to.col + 1);   
        }
        
        Piece* rookOriginalPtr = getPieceAt(rookFromPos);
        if (rookOriginalPtr) { 
            std::unique_ptr<Piece> rook = removePiece(rookFromPos);
            if (rook) { 
                addPiece(std::move(rook), rookToPos);
                rookOriginalPtr->setHasMoved(true);
            }
        }
    }
    
    updateCastlingRights(move, movingPieceOriginalPtr); 
    
    return capturedPiece;
}


BoardDimensions Board::getDimensions() const {
    return dimensions;
}

Position Board::getEnPassantTargetSquare() const {
    return enPassantTargetSquare;
}

void Board::setEnPassantTargetSquare(Position pos) {
    enPassantTargetSquare = pos;
}

void Board::clearEnPassantTargetSquare() {
    enPassantTargetSquare = Position(-1,-1); 
}


bool Board::canCastleKingside(Color color) const {
    return (color == Color::WHITE) ? whiteCanCastleKingside : blackCanCastleKingside;
}

bool Board::canCastleQueenside(Color color) const {
    return (color == Color::WHITE) ? whiteCanCastleQueenside : blackCanCastleQueenside;
}

void Board::updateCastlingRights(const Move& moveMade, Piece* movedPieceDetails) {
    if (!movedPieceDetails) return;

    Position originalPos = moveMade.from; 
    Color pieceColor = movedPieceDetails->getColor();
    PieceType pieceType = movedPieceDetails->getType();

    const int whiteBackRank = (dimensions.rows == 8) ? 7 : -1; 
    const int blackBackRank = (dimensions.rows == 8) ? 0 : -1; 
    const int queenSideCol = 0;
    const int kingSideCol = (dimensions.cols == 8) ? 7 : -1;

    if (pieceType == PieceType::KING) {
        if (pieceColor == Color::WHITE) {
            whiteCanCastleKingside = false;
            whiteCanCastleQueenside = false;
        } else { 
            blackCanCastleKingside = false;
            blackCanCastleQueenside = false;
        }
    } 
    else if (pieceType == PieceType::ROOK) {
        if (pieceColor == Color::WHITE && originalPos.row == whiteBackRank) {
            if (originalPos.col == queenSideCol) whiteCanCastleQueenside = false; 
            else if (originalPos.col == kingSideCol) whiteCanCastleKingside = false; 
        } else if (pieceColor == Color::BLACK && originalPos.row == blackBackRank) {
            if (originalPos.col == queenSideCol) blackCanCastleQueenside = false; 
            else if (originalPos.col == kingSideCol) blackCanCastleKingside = false; 
        }
    }
}

void Board::setCastlingRights(Color color, bool kingside, bool queenside) {
    if (color == Color::WHITE) {
        whiteCanCastleKingside = kingside;
        whiteCanCastleQueenside = queenside;
    } else { 
        blackCanCastleKingside = kingside;
        blackCanCastleQueenside = queenside;
    }
}

const Move* Board::getLastMove() const {
    return lastMove;
}

void Board::setLastMove(const Move* move) {
    lastMove = move;
}

std::vector<Piece*> Board::findPieces(Color color, PieceType type) const {
    std::vector<Piece*> foundPieces;
    for (int r = 0; r < dimensions.rows; ++r) {
        for (int c = 0; c < dimensions.cols; ++c) {
            const Piece* p = grid[r][c].get(); 
            if (p && p->getColor() == color) {
                if (type == PieceType::EMPTY || p->getType() == type) { 
                    foundPieces.push_back(const_cast<Piece*>(p)); 
                }
            }
        }
    }
    return foundPieces;
}

Position Board::findKing(Color color) const {
    for (int r = 0; r < dimensions.rows; ++r) {
        for (int c = 0; c < dimensions.cols; ++c) {
            const Piece* p = grid[r][c].get();
            if (p && p->getColor() == color && p->getType() == PieceType::KING) {
                return p->getPosition(); 
            }
        }
    }
    return Position(-1, -1); 
}

bool Board::isSquareAttacked(Position square, Color attackerColor) const {
    if (!square.isValid(dimensions.rows, dimensions.cols)) {
        return false; 
    }

    for (int r = 0; r < dimensions.rows; ++r) {
        for (int c = 0; c < dimensions.cols; ++c) {
            const Piece* p = grid[r][c].get();
            if (p && p->getColor() == attackerColor) {
                std::vector<Move> moves = p->getPossibleMoves(*this); 
                for (const auto& move : moves) {
                    if (p->getType() == PieceType::PAWN) {
                        int direction = (p->getColor() == Color::WHITE) ? -1 : 1; 
                        Position currentPawnPos = p->getPosition();
                        if (square.row == currentPawnPos.row + direction) {
                            if (square.col == currentPawnPos.col + 1 || square.col == currentPawnPos.col - 1) {
                                return true; 
                            }
                        }
                    } 
                    else if (move.to == square) {
                        return true; 
                    }
                }
            }
        }
    }
    return false; 
}
