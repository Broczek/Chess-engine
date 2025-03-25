#pragma once
#include <utility>

class ChessPiece;

class Board {
private:
    ChessPiece* board[8][8];

public:
    Board();
    ~Board();

    ChessPiece* getChessPiece(int x, int y) const;

    void setupBoard();
    void drawBoard() const;
    void moveChessPiece(int x_from, int y_from, int x_to, int y_to);

    bool isEmpty(int x, int y) const;
    bool isOpponent(int x, int y, bool isWhite) const;
    bool isAttacked(int x, int y, bool byWhite) const;

    std::pair<int, int> findKing(bool white) const;

    friend class Game;
};
