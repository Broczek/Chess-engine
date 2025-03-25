#include "Rook.h"
#include "Board.h"
#include <cmath>
#include <string>

Rook::Rook(int x, int y, bool white) : ChessPiece(x, y, white) {}

bool Rook::canMove(int newX, int newY, const Board& board) const {
    if (!(x == newX || y == newY)) return false;

    int dx = (newX - x) ? ((newX - x) / abs(newX - x)) : 0;
    int dy = (newY - y) ? ((newY - y) / abs(newY - y)) : 0;

    for (int ix = x + dx, iy = y + dy; ix != newX || iy != newY; ix += dx, iy += dy)
        if (!board.isEmpty(ix, iy)) return false;

    return board.isEmpty(newX, newY) || board.isOpponent(newX, newY, white);
}

std::string Rook::getSymbol() const { 
    return white ? "♜" : "♖"; 
}

int Rook::getTypeIndex() const { return 2; }