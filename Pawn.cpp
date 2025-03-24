#include "Pawn.h"
#include "Board.h"
#include <cmath>

Pawn::Pawn(int x, int y, bool white) : ChessPiece(x, y, white) {}

bool Pawn::canMove(int newX, int newY, const Board& board) const {
    int direction = white ? -1 : 1;
    int startRow = white ? 6 : 1;

    if (x == newX && board.isEmpty(newX, newY)) {
        if (y + direction == newY) return true;
        if (y == startRow && y + 2 * direction == newY && board.isEmpty(newX, y + direction)) return true;
    } else if (abs(x - newX) == 1 && y + direction == newY && board.isOpponent(newX, newY, white)) {
        return true;
    }

    return false;
}

std::string Pawn::getSymbol() const { 
    return white ? "♟" : "♙"; 
}