#pragma once
#include <utility>

class ChessPiece;

class Board {
private:
    ChessPiece* board[8][8];

public:
    Board();
    ~Board();

    void setupBoard();
    void drawBoard() const;

    ChessPiece* getChessPiece(int x, int y) const;
    void moveChessPiece(int x_from, int y_from, int x_to, int y_to);

    bool isEmpty(int x, int y) const;
    bool isOpponent(int x, int y, bool isWhite) const;

    std::pair<int, int> findKing(bool white) const;
    bool isAttacked(int x, int y, bool byWhite) const;

    friend class Game;
};
