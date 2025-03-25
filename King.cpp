#include "King.h"
#include "Board.h"
#include <cmath>

King::King(int x, int y, bool white) : ChessPiece(x, y, white) {}

bool King::canMove(int newX, int newY, const Board& board) const {
    int dx = abs(x - newX);
    int dy = abs(y - newY);
    return (dx <= 1 && dy <= 1) && (board.isEmpty(newX, newY) || board.isOpponent(newX, newY, white));
}

std::string King::getSymbol() const { 
    return white ? "♚" : "♔"; 
}

int King::getTypeIndex() const { return 0; }
