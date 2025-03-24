#include "ChessPiece.h"

ChessPiece::ChessPiece(int startX, int startY, bool isWhite) : x(startX), y(startY), white(isWhite), moved(false) {}

bool ChessPiece::rusz(int newX, int newY, const Board& board) {
    if (canMove(newX, newY, board)) {
        x = newX;
        y = newY;
        moved = true;
        return true;
    }
    return false;
}

bool ChessPiece::isWhite() const { return white; }
void ChessPiece::setPosition(int x, int y) { this->x = x; this->y = y; }
bool ChessPiece::hasMoved() const { return moved; }
void ChessPiece::setMoved(bool moved) { this->moved = moved; }
int ChessPiece::getX() const { return x; }
int ChessPiece::getY() const { return y; }