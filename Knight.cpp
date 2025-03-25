#include "Knight.h"
#include "Board.h"
#include <cmath>

Knight::Knight(int x, int y, bool white) : ChessPiece(x, y, white) {}

bool Knight::canMove(int newX, int newY, const Board& board) const {
    int dx = abs(x - newX);
    int dy = abs(y - newY);
    return ((dx == 2 && dy == 1) || (dx == 1 && dy == 2)) && (board.isEmpty(newX, newY) || board.isOpponent(newX, newY, white));
}

std::string Knight::getSymbol() const { 
    return white ? "♞" : "♘"; 
}

int Knight::getTypeIndex() const { return 4; }
