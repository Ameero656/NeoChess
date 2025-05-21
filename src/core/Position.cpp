#include "core/Position.h"
#include <stdexcept> // For invalid_argument

Position::Position(int r, int c) : row(r), col(c) {}

bool Position::isValid(int maxRows, int maxCols) const {
    return row >= 0 && row < maxRows && col >= 0 && col < maxCols;
}

bool Position::operator==(const Position& other) const {
    return row == other.row && col == other.col;
}

bool Position::operator!=(const Position& other) const {
    return !(*this == other);
}

std::string Position::toAlgebraic() const {
    if (!isValid()) {
        return "??";
    }
    char file = 'a' + col;
    char rank = '1' + (7 - row);
    return std::string() + file + rank;
}

Position Position::fromAlgebraic(const std::string& algNot) {
    if (algNot.length() != 2) {
        throw std::invalid_argument("Algebraic notation must be 2 characters long.");
    }
    char file = algNot[0];
    char rank = algNot[1];

    if (file < 'a' || file > 'h' || rank < '1' || rank > '8') {
        throw std::invalid_argument("Invalid algebraic notation characters.");
    }

    int c = file - 'a';
    int r = 7 - (rank - '1'); 
    return Position(r, c);
}