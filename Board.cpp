#include "Board.h"
#include "Rook.h"
#include "Knight.h"
#include "Bishop.h"
#include "Queen.h"
#include "King.h"
#include "Pawn.h"
#include "operations.h"
#include <iostream>

Board::Board() {
    setupBoard();
}

Board::~Board() {
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            delete board[y][x];
}

void Board::setupBoard() {
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++)
            board[y][x] = nullptr;

    board[7][0] = new Rook(0, 7, true);
    board[7][1] = new Knight(1, 7, true);
    board[7][2] = new Bishop(2, 7, true);
    board[7][3] = new Queen(3, 7, true);
    board[7][4] = new King(4, 7, true);
    board[7][5] = new Bishop(5, 7, true);
    board[7][6] = new Knight(6, 7, true);
    board[7][7] = new Rook(7, 7, true);
    for (int i = 0; i < 8; i++)
        board[6][i] = new Pawn(i, 6, true);

    board[0][0] = new Rook(0, 0, false);
    board[0][1] = new Knight(1, 0, false);
    board[0][2] = new Bishop(2, 0, false);
    board[0][3] = new Queen(3, 0, false);
    board[0][4] = new King(4, 0, false);
    board[0][5] = new Bishop(5, 0, false);
    board[0][6] = new Knight(6, 0, false);
    board[0][7] = new Rook(7, 0, false);
    for (int i = 0; i < 8; i++)
        board[1][i] = new Pawn(i, 1, false);
}

void Board::drawBoard() const {
    Operations::clearScreen();
    std::string letters = "ABCDEFGH";

    std::cout << "    ";
    for (int i = 0; i < 8; i++) std::cout << letters[i] << "  ";
    std::cout << "\n";

    for (int y = 0; y < 8; y++) {
        std::cout << " " << 8 - y << " ";
        for (int x = 0; x < 8; x++) {
            std::cout << "[";
            if (board[y][x] != nullptr)
                std::cout << board[y][x]->getSymbol();
            else
                std::cout << " ";
            std::cout << "]";
        }
        std::cout << "\n";
    }
}

ChessPiece* Board::getChessPiece(int x, int y) const {
    return board[y][x];
}

void Board::moveChessPiece(int x_from, int y_from, int x_to, int y_to) {
    board[y_to][x_to] = board[y_from][x_from];
    board[y_from][x_from] = nullptr;
    board[y_to][x_to]->setPosition(x_to, y_to);
}

bool Board::isEmpty(int x, int y) const {
    return board[y][x] == nullptr;
}

bool Board::isOpponent(int x, int y, bool isWhite) const {
    return board[y][x] != nullptr && board[y][x]->isWhite() != isWhite;
}

std::pair<int, int> Board::findKing(bool white) const {
    for (int y = 0; y < 8; y++)
        for (int x = 0; x < 8; x++) {
            ChessPiece* ChessPiece = board[y][x];
            if (ChessPiece && ChessPiece->isWhite() == white && dynamic_cast<King*>(ChessPiece))
                return { x, y };
        }
    return { -1, -1 };
}

bool Board::isAttacked(int x, int y, bool byWhite) const {
    for (int iy = 0; iy < 8; iy++)
        for (int ix = 0; ix < 8; ix++) {
            ChessPiece* ChessPiece = board[iy][ix];
            if (ChessPiece && ChessPiece->isWhite() == byWhite && ChessPiece->canMove(x, y, *this))
                return true;
        }
    return false;
}
