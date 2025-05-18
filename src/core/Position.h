#ifndef POSITION_H
#define POSITION_H

#include <string> // For algebraic notation conversion

struct Position {
    int row;
    int col;

    Position(int r = -1, int c = -1);

    bool isValid(int maxRows = 8, int maxCols = 8) const;
    bool operator==(const Position& other) const;
    bool operator!=(const Position& other) const;

    // Helper to convert to algebraic notation (e.g., "e4")
    std::string toAlgebraic() const;
    // Helper to create Position from algebraic notation
    static Position fromAlgebraic(const std::string& algNot);
};

// For using Position as a key in std::map or std::unordered_map if needed later
namespace std {
    template <>
    struct hash<Position> {
        size_t operator()(const Position& p) const {
            return hash<int>()(p.row) ^ (hash<int>()(p.col) << 1);
        }
    };
}

#endif // POSITION_H