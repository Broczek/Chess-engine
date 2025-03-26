#include "Game.h"
#include "operations.h"
#include "King.h"
#include "Queen.h"
#include "Rook.h"
#include "Bishop.h"
#include "Knight.h"
#include "Pawn.h"
#include "Zobrist.h"
#include <conio.h>
#include <iostream>
#include <vector>
#include <algorithm>

constexpr int pieceValues[] = {
    20000, // King
    900,   // Queen
    500,   // Rook
    330,   // Bishop
    320,   // Knight
    100    // Pawn
};

Game::Game() : cursorX(0), cursorY(0), whiteTurn(true) { Zobrist::init(); }

Game::~Game() {}

void Game::initialize() {
    Operations::clearScreen();
    Operations::hideCursor();

    whiteTurn = true;
    halfmoveClock = 0;
    lastDoublePawnX = -1;
    lastDoublePawnY = -1;
    positionHistory.clear();
    board.setupBoard();
    transpositionTable.clear();
}


void Game::draw() {
    board.drawBoard();
    Operations::gotoxy(0, 9);
    std::cout << (whiteTurn ? "White's turn" : "Black's turn");
}

ChessPiece* Game::selectFigure() {
    cursorX = 0;
    cursorY = 0;

    while (true) {
        draw();
        Operations::gotoxy(cursorX * 3 + 4, cursorY + 1);
        Operations::setTextColor(14);
        std::cout << "X";

        int key = _getch();

        if (key == 224) {
            int arrow = _getch();
            if (arrow == 72 && cursorY > 0) cursorY--;
            else if (arrow == 80 && cursorY < 7) cursorY++;
            else if (arrow == 75 && cursorX > 0) cursorX--;
            else if (arrow == 77 && cursorX < 7) cursorX++;
        }
        else if (key == 13) {
            ChessPiece* ChessPiece = board.getChessPiece(cursorX, cursorY);
            if (ChessPiece != nullptr && ChessPiece->isWhite() == whiteTurn)
                return ChessPiece;
        }
    }
}

void Game::moveFigure(ChessPiece* chessPiece) {
    int originalX = chessPiece->getX();
    int originalY = chessPiece->getY();
    int targetX = originalX, targetY = originalY;

    while (true) {
        draw();
        Operations::gotoxy(targetX * 3 + 4, targetY + 1);
        Operations::setTextColor(10);
        std::cout << "*";

        int key = _getch();

        if (key == 'q' && targetX > 0 && targetY > 0) { targetX--; targetY--; }
        else if (key == 'w' && targetY > 0) { targetY--; }
        else if (key == 'e' && targetX < 7 && targetY > 0) { targetX++; targetY--; }
        else if (key == 'a' && targetX > 0) { targetX--; }
        else if (key == 'd' && targetX < 7) { targetX++; }
        else if (key == 'z' && targetX > 0 && targetY < 7) { targetX--; targetY++; }
        else if (key == 'x' && targetY < 7) { targetY++; }
        else if (key == 'c' && targetX < 7 && targetY < 7) { targetX++; targetY++; }
        else if (key == 's') return;
        else if (key == 13) {
            int diffX = abs(targetX - chessPiece->getX());

            if (dynamic_cast<King*>(chessPiece) && diffX == 2 && targetY == chessPiece->getY()) {
                bool kingside = targetX > chessPiece->getX();
                ChessPiece* rook = board.getChessPiece(kingside ? 7 : 0, chessPiece->getY());

                if (canCastle(chessPiece, rook, kingside)) {
                    performCastle(chessPiece, kingside);
                    whiteTurn = !whiteTurn;
                    lastDoublePawnX = -1;
                    lastDoublePawnY = -1;
                    halfmoveClock++;
                    return;
                }
                else {
                    draw();
                    Operations::gotoxy(0, 10);
                    Operations::setTextColor(12);
                    std::cout << "Castling not possible!";
                    _getch();
                    return;
                }
            }

            bool isEnPassant = false;
            Pawn* movingPawn = dynamic_cast<Pawn*>(chessPiece);

            if (movingPawn) {
                int dir = chessPiece->isWhite() ? -1 : 1;

                if (abs(targetX - originalX) == 1 && targetY - originalY == dir && board.isEmpty(targetX, targetY)) {
                    if (targetX == lastDoublePawnX && targetY == lastDoublePawnY + dir) {
                        isEnPassant = true;
                    }
                }
            }

            if (chessPiece->canMove(targetX, targetY, board) || isEnPassant) {
                ChessPiece* captured = isEnPassant
                    ? board.getChessPiece(targetX, targetY + (chessPiece->isWhite() ? 1 : -1))
                    : board.getChessPiece(targetX, targetY);

                if (isEnPassant) {
                    delete board.getChessPiece(targetX, targetY + (chessPiece->isWhite() ? 1 : -1));
                    board.board[targetY + (chessPiece->isWhite() ? 1 : -1)][targetX] = nullptr;
                }

                board.moveChessPiece(originalX, originalY, targetX, targetY);

                if (isCheck(whiteTurn)) {
                    board.moveChessPiece(targetX, targetY, originalX, originalY);
                    board.board[targetY][targetX] = captured;
                    if (captured) captured->setPosition(targetX, targetY);
                    if (isEnPassant) {
                        board.board[targetY + (chessPiece->isWhite() ? 1 : -1)][targetX] = captured;
                    }

                    draw();
                    Operations::gotoxy(0, 10);
                    Operations::setTextColor(12);
                    std::cout << "Illegal move! Your king is still in check!";
                    _getch();
                }
                else {
                    chessPiece->setMoved(true);

                    if (movingPawn && abs(targetY - originalY) == 2) {
                        lastDoublePawnX = targetX;
                        lastDoublePawnY = targetY;
                    } else {
                        lastDoublePawnX = -1;
                        lastDoublePawnY = -1;
                    }

                    if (movingPawn || captured) {
                        halfmoveClock = 0;
                    } else {
                        halfmoveClock++;
                    }

                    if (movingPawn) {
                        if ((chessPiece->isWhite() && targetY == 0) || (!chessPiece->isWhite() && targetY == 7)) {
                            handlePawnPromotion(targetX, targetY, chessPiece->isWhite());
                        }
                    }

                    bool isCapture = (captured != nullptr || isEnPassant);
                    std::string notation = getAlgebraicNotation(chessPiece, targetX, targetY, isCapture);
                    logMove(notation);

                    whiteTurn = !whiteTurn;
                    return;
                }
            }
        }
    }
}

bool Game::isCheck(bool white) const {
    auto [kingX, kingY] = board.findKing(white);
    return board.isAttacked(kingX, kingY, !white);
}

bool Game::isCheckmate(bool white) {
    if (!isCheck(white))
        return false;

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            ChessPiece* chessPiece = board.getChessPiece(x, y);
            if (chessPiece && chessPiece->isWhite() == white) {
                for (int ty = 0; ty < 8; ty++) {
                    for (int tx = 0; tx < 8; tx++) {
                        if (chessPiece->canMove(tx, ty, board)) {
                            ChessPiece* target = board.getChessPiece(tx, ty);
                            board.moveChessPiece(x, y, tx, ty);
                            bool inCheck = isCheck(white);
                            board.moveChessPiece(tx, ty, x, y);
                            board.board[ty][tx] = target;
                            if (target) target->setPosition(tx, ty);

                            if (!inCheck)
                                return false;
                        }
                    }
                }
            }
        }
    }

    return true;
}

void Game::run() {
    while (true) {
        chooseSide();
        initialize();
        updatePositionHistory();
        moveHistory.clear();
        startLogging();

        while (true) {
            if (isCheckmate(whiteTurn)) {
                draw();
                Operations::gotoxy(0, 10);
                Operations::setTextColor(12);
                std::cout << "Checkmate! " << (!whiteTurn ? "White" : "Black") << " wins!";
                saveGameResult(whiteTurn ? "0-1" : "1-0");
                break;
            }

            if (isStalemate(whiteTurn)) {
                draw();
                Operations::gotoxy(0, 10);
                Operations::setTextColor(14);
                std::cout << "Stalemate! It's a draw.";
                saveGameResult("1/2-1/2");
                break;
            }

            if (isThreefoldRepetition()) {
                draw();
                Operations::gotoxy(0, 10);
                Operations::setTextColor(14);
                std::cout << "Threefold repetition! It's a draw.";
                saveGameResult("1/2-1/2");
                break;
            }

            if (isInsufficientMaterial()) {
                draw();
                Operations::gotoxy(0, 10);
                Operations::setTextColor(14);
                std::cout << "Draw! Insufficient mating material.";
                saveGameResult("1/2-1/2");
                break;
            }

            if (halfmoveClock >= 100) {
                draw();
                Operations::gotoxy(0, 10);
                Operations::setTextColor(14);
                std::cout << "Draw! 50-move rule: no pawn moves or captures.";
                saveGameResult("1/2-1/2");
                break;
            }

            if ((whiteTurn && playerIsWhite) || (!whiteTurn && !playerIsWhite)) {
                ChessPiece* selected = selectFigure();
                moveFigure(selected);
                draw();
                updatePositionHistory();
            } else {
                draw();
                makeComputerMove();
                updatePositionHistory();
            }            
            
            if (isCheck(!whiteTurn)) {
                draw();
                Operations::gotoxy(0, 10);
                Operations::setTextColor(12);
                std::cout << (!whiteTurn ? "White" : "Black") << " is in check!";
                _getch();
            }
        }

        Operations::gotoxy(0, 12);
        Operations::setTextColor(11);
        std::cout << "Play again? (y/n): ";

        char again = _getch();
        if (again != 'y' && again != 'Y') break;
    }
}


bool Game::canCastle(ChessPiece* king, ChessPiece* rook, bool kingside) {
    if (!king || !rook) return false;

    if (king->hasMoved() || rook->hasMoved()) return false;

    int y = king->isWhite() ? 7 : 0;
    int kingX = king->getX();
    int rookX = kingside ? 7 : 0;
    int direction = kingside ? 1 : -1;

    for (int x = king->getX() + direction; x != rook->getX(); x += direction)
        if (!board.isEmpty(x, y)) return false;

    if (isCheck(king->isWhite())) return false;

    for (int x = king->getX(); x != king->getX() + (direction * 3); x += direction)
        if (board.isAttacked(x, king->getY(), !king->isWhite())) return false;

    if (king->hasMoved() || rook->hasMoved()) return false;

    return true;
}

void Game::performCastle(ChessPiece* king, bool kingside) {
    int y = king->isWhite() ? 7 : 0;
    ChessPiece* rook = board.getChessPiece(kingside ? 7 : 0, y);
    if (!rook) return;

    board.moveChessPiece(4, y, kingside ? 6 : 2, y);
    king->setMoved(true);

    board.moveChessPiece(kingside ? 7 : 0, y, kingside ? 5 : 3, y);
    rook->setMoved(true);
}

uint64_t Game::computeZobristHash() const {
    uint64_t hash = 0;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            ChessPiece* piece = board.getChessPiece(x, y);
            if (piece) {
                int color = piece->isWhite() ? 0 : 1;
                int type = piece->getTypeIndex();
                hash ^= Zobrist::pieceHash[color][type][x][y];
            }
        }
    }
    if (!whiteTurn)
        hash ^= Zobrist::sideHash;
    return hash;
}

void Game::updatePositionHistory() {
    uint64_t hash = computeZobristHash();
    positionHistory[hash]++;
}

bool Game::isThreefoldRepetition() const {
    uint64_t hash = computeZobristHash();
    auto it = positionHistory.find(hash);
    return it != positionHistory.end() && it->second >= 3;
}

bool Game::isStalemate(bool white) {
    if (isCheck(white))
        return false;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            ChessPiece* piece = board.getChessPiece(x, y);
            if (piece && piece->isWhite() == white) {
                for (int ty = 0; ty < 8; ++ty) {
                    for (int tx = 0; tx < 8; ++tx) {
                        if (piece->canMove(tx, ty, board)) {
                            ChessPiece* captured = board.getChessPiece(tx, ty);
                            board.moveChessPiece(x, y, tx, ty);
                            bool inCheck = isCheck(white);
                            board.moveChessPiece(tx, ty, x, y);
                            board.board[ty][tx] = captured;
                            if (captured) captured->setPosition(tx, ty);
                            if (!inCheck)
                                return false;
                        }
                    }
                }
            }
        }
    }
    return true;
}

bool Game::isInsufficientMaterial() const {
    int whiteKnights = 0, blackKnights = 0;
    std::vector<std::pair<int, int>> bishopPositions;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            ChessPiece* piece = board.getChessPiece(x, y);
            if (!piece) continue;

            if (dynamic_cast<King*>(piece)) continue;

            int type = piece->getTypeIndex();

            if (type == 3) {
                bishopPositions.emplace_back(x, y);
            }
            else if (type == 4) {
                piece->isWhite() ? whiteKnights++ : blackKnights++;
            }
            else {
                return false;
            }
        }
    }

    if (bishopPositions.empty() && whiteKnights == 0 && blackKnights == 0)
        return true;

    if (bishopPositions.empty() && whiteKnights + blackKnights == 1)
        return true;

    if (bishopPositions.size() == 1 && whiteKnights + blackKnights == 0)
        return true;

    if (!bishopPositions.empty() && whiteKnights + blackKnights == 0) {
        bool firstColor = (bishopPositions[0].first + bishopPositions[0].second) % 2;
        for (const auto& [x, y] : bishopPositions) {
            if ((x + y) % 2 != firstColor)
                return false;
        }
        return true;
    }

    return false;
}

void Game::handlePawnPromotion(int x, int y, bool isWhite) {
    while (true) {
        draw();
        Operations::gotoxy(0, 10);
        Operations::setTextColor(11);

        std::string queenIcon = isWhite ? "♛" : "♕";
        std::string rookIcon = isWhite ? "♜" : "♖";
        std::string bishopIcon = isWhite ? "♝" : "♗";
        std::string knightIcon = isWhite ? "♞" : "♘";

        std::cout << "Promote pawn to ("
                  << queenIcon << " - q, "
                  << rookIcon << " - r, "
                  << bishopIcon << " - b, "
                  << knightIcon << " - k): ";

        char choice = _getch();
        delete board.board[y][x];

        switch (choice) {
        case 'q':
            board.board[y][x] = new Queen(x, y, isWhite);
            return;
        case 'r':
            board.board[y][x] = new Rook(x, y, isWhite);
            return;
        case 'b':
            board.board[y][x] = new Bishop(x, y, isWhite);
            return;
        case 'k':
            board.board[y][x] = new Knight(x, y, isWhite);
            return;
        default:
            Operations::gotoxy(0, 11);
            Operations::setTextColor(12);
            std::cout << "Invalid choice! Use q, r, b, k";
            _getch();
            break;
        }
    }
}

void Game::chooseSide() {
    while (true) {
        Operations::clearScreen();
        Operations::setTextColor(11);
        std::cout << "Choose your side:\n";
        std::cout << "  [w] White (♙)\n";
        std::cout << "  [b] Black (♟)\n";
        std::cout << "Your choice: ";

        char input = _getch();
        if (input == 'w' || input == 'W') {
            playerIsWhite = true;
            return;
        }
        else if (input == 'b' || input == 'B') {
            playerIsWhite = false;
            return;
        }
        else {
            std::cout << "\nInvalid input. Press 'w' or 'b'.";
            _getch();
        }
    }
}

void Game::startLogging() {
    time_t now = time(nullptr);
    tm* local = localtime(&now);
    char filename[64];
    strftime(filename, sizeof(filename), "game_%Y%m%d_%H%M%S.txt", local);

    gameLog.open(filename);

    char dateTime[64];
    strftime(dateTime, sizeof(dateTime), "%Y-%m-%d %H:%M:%S", local);

    gameLog << "Date: " << dateTime << "\n";
    gameLog << "White: " << (playerIsWhite ? "Player" : "Computer") << "\n";
    gameLog << "Black: " << (playerIsWhite ? "Computer" : "Player") << "\n";
}

void Game::logMove(const std::string& move) {
    moveHistory.push_back(move);
}

void Game::saveGameResult(const std::string& result) {
    if (!gameLog.is_open()) return;

    std::string resultText;
    if (result == "1-0")
        resultText = (playerIsWhite ? "Player win" : "Computer win");
    else if (result == "0-1")
        resultText = (playerIsWhite ? "Computer win" : "Player win");
    else
        resultText = "Tie";

    gameLog << "Result: " << resultText << "\n\n";

    for (size_t i = 0; i < moveHistory.size(); ++i) {
        if (i % 2 == 0)
            gameLog << (i / 2 + 1) << ".";
        gameLog << moveHistory[i] << " ";
    }

    gameLog << "\n";
    gameLog.close();
}

std::string Game::getAlgebraicNotation(ChessPiece* piece, int toX, int toY, bool capture) {
    std::string notation;
    char files[] = "abcdefgh";
    char ranks[] = "87654321";

    int type = piece->getTypeIndex();
    if (type != 5) {
        switch (type) {
            case 0: notation += "K"; break;
            case 1: notation += "Q"; break;
            case 2: notation += "R"; break;
            case 3: notation += "B"; break;
            case 4: notation += "N"; break;
        }
    } else if (capture) {
        notation += files[piece->getX()];
    }

    if (capture)
        notation += "x";

    notation += files[toX];
    notation += ranks[toY];

    if (isCheckmate(!piece->isWhite())) {
        notation += "#";
    } else if (isCheck(!piece->isWhite())) {
        notation += "+";
    }

    return notation;
}

void Game::makeComputerMove() {
    Move move = getBestMove(3);

    ChessPiece* piece = move.movedPiece;
    if (!piece) return;

    board.moveChessPiece(move.fromX, move.fromY, move.toX, move.toY);
    piece->setMoved(true);

    if (dynamic_cast<Pawn*>(piece)) {
        if ((piece->isWhite() && move.toY == 0) || (!piece->isWhite() && move.toY == 7)) {
            handlePawnPromotion(move.toX, move.toY, piece->isWhite());
        }

        if (abs(move.toY - move.fromY) == 2) {
            lastDoublePawnX = move.toX;
            lastDoublePawnY = move.toY;
        } else {
            lastDoublePawnX = -1;
            lastDoublePawnY = -1;
        }
    } else {
        lastDoublePawnX = -1;
        lastDoublePawnY = -1;
    }

    if (dynamic_cast<Pawn*>(piece) || move.capturedPiece)
        halfmoveClock = 0;
    else
        halfmoveClock++;

    bool isCapture = (move.capturedPiece != nullptr);
    std::string notation = getAlgebraicNotation(piece, move.toX, move.toY, isCapture);
    logMove(notation);

    whiteTurn = !whiteTurn;
}


int Game::evaluatePosition(const Board& board) const {
    int score = 0;
    int mobilityScore = 0;
    int developmentScore = 0;
    int centerControl = 0;
    int kingSafetyScore = 0;

    const std::vector<std::pair<int, int>> centralSquares = {
        {3, 3}, {3, 4}, {4, 3}, {4, 4}
    };

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            ChessPiece* piece = board.getChessPiece(x, y);
            if (!piece) continue;

            int type = piece->getTypeIndex();
            int value = pieceValues[type];

            bool isComputer = piece->isWhite() == !playerIsWhite;
            score += isComputer ? value : -value;

            int legalMoves = 0;
            for (int ty = 0; ty < 8; ++ty) {
                for (int tx = 0; tx < 8; ++tx) {
                    if (piece->canMove(tx, ty, board)) {
                        legalMoves++;

                        for (auto& sq : centralSquares) {
                            if (tx == sq.first && ty == sq.second) {
                                centerControl += isComputer ? +15 : -15;
                            }
                        }
                    }
                }
            }

            mobilityScore += (isComputer ? +1 : -1) * legalMoves * 2;

            if ((type == 3 || type == 4) && !piece->hasMoved()) {
                developmentScore += isComputer ? -15 : +15;
            }
        }
    }

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            ChessPiece* piece = board.getChessPiece(x, y);
            if (!piece || piece->getTypeIndex() != 0) continue;

            bool isComputer = piece->isWhite() == !playerIsWhite;
            int direction = piece->isWhite() ? -1 : 1;

            for (int dx = -1; dx <= 1; ++dx) {
                int px = piece->getX() + dx;
                int py = piece->getY() + direction;

                if (px < 0 || px > 7 || py < 0 || py > 7) continue;

                ChessPiece* front = board.getChessPiece(px, py);
                if (front && front->getTypeIndex() == 5 && front->isWhite() == piece->isWhite()) {
                    kingSafetyScore += isComputer ? +10 : -10;
                } else {
                    kingSafetyScore += isComputer ? -8 : +8;
                }
            }
        }
    }

    return score + mobilityScore + developmentScore + centerControl + kingSafetyScore;
}

int Game::minimax(Board& board, int depth, int alpha, int beta, bool maximizingPlayer, bool isRoot) {
    if (!isRoot) {
        auto now = std::chrono::steady_clock::now();
        int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - searchStartTime).count();
    
        if (elapsed >= timeLimitMillis) {
            timeUp = true;
            return evaluatePosition(board);
        }
    }    

    uint64_t hash = computeZobristHash();
    if (transpositionTable.count(hash)) {
        return transpositionTable[hash];
    }

    if (depth == 0) {
        int eval = evaluatePosition(board);
        transpositionTable[hash] = eval;
        return eval;
    }

    int bestEval = maximizingPlayer ? -100000 : 100000;
    std::vector<Move> moves = generateSortedMoves(maximizingPlayer == !playerIsWhite);

    for (const Move& move : moves) {
        ChessPiece* piece = move.movedPiece;
        ChessPiece* captured = move.capturedPiece;

        board.moveChessPiece(move.fromX, move.fromY, move.toX, move.toY);

        if (isCheck(piece->isWhite())) {
            board.moveChessPiece(move.toX, move.toY, move.fromX, move.fromY);
            board.board[move.toY][move.toX] = captured;
            if (captured) captured->setPosition(move.toX, move.toY);
            continue;
        }

        if (isCheckmate(!piece->isWhite())) {
            int mateScore = maximizingPlayer
                ? 100000 - (maxSearchDepth - depth)
                : -100000 + (maxSearchDepth - depth);
            board.moveChessPiece(move.toX, move.toY, move.fromX, move.fromY);
            board.board[move.toY][move.toX] = captured;
            if (captured) captured->setPosition(move.toX, move.toY);
            transpositionTable[hash] = mateScore;
            return mateScore;
        }

        int bonus = 0;
        if (isCheck(!piece->isWhite())) bonus = 1;
        if (captured) bonus = std::max(bonus, 1);
        int nextDepth = std::min(depth - 1 + bonus, maxSearchDepth);

        int eval = minimax(board, nextDepth, alpha, beta, !maximizingPlayer, false);

        board.moveChessPiece(move.toX, move.toY, move.fromX, move.fromY);
        board.board[move.toY][move.toX] = captured;
        if (captured) captured->setPosition(move.toX, move.toY);

        if (maximizingPlayer) {
            bestEval = std::max(bestEval, eval);
            alpha = std::max(alpha, eval);
        } else {
            bestEval = std::min(bestEval, eval);
            beta = std::min(beta, eval);
        }

        if (beta <= alpha)
            break;
    }

    transpositionTable[hash] = bestEval;
    return bestEval;
}

Move Game::getBestMove(int depth) {
    int bestEval = -100000;
    Move bestMove;

    searchStartTime = std::chrono::steady_clock::now();
    timeUp = false;

    std::vector<Move> moves = generateSortedMoves(!playerIsWhite);

    for (const Move& move : moves) {
        if (timeUp) break;

        ChessPiece* piece = move.movedPiece;
        ChessPiece* captured = move.capturedPiece;

        board.moveChessPiece(move.fromX, move.fromY, move.toX, move.toY);

        if (isCheck(piece->isWhite())) {
            board.moveChessPiece(move.toX, move.toY, move.fromX, move.fromY);
            board.board[move.toY][move.toX] = captured;
            if (captured) captured->setPosition(move.toX, move.toY);
            continue;
        }

        int eval = minimax(board, depth - 1, -100000, 100000, false, true);

        board.moveChessPiece(move.toX, move.toY, move.fromX, move.fromY);
        board.board[move.toY][move.toX] = captured;
        if (captured) captured->setPosition(move.toX, move.toY);

        if (eval > bestEval) {
            bestEval = eval;
            bestMove = move;
        }
    }

    if (bestMove.movedPiece) {
        std::string symbol = bestMove.movedPiece->getSymbol();
        std::string name = symbol == "♛" || symbol == "♕" ? "Q" :
                           symbol == "♜" || symbol == "♖" ? "R" :
                           symbol == "♝" || symbol == "♗" ? "B" :
                           symbol == "♞" || symbol == "♘" ? "N" :
                           symbol == "♚" || symbol == "♔" ? "K" :
                           "";

        char files[] = "abcdefgh";
        char ranks[] = "87654321";

        std::string from = std::string(1, files[bestMove.fromX]) + ranks[bestMove.fromY];
        std::string to   = std::string(1, files[bestMove.toX]) + ranks[bestMove.toY];

        int displayedEval = bestEval;
        if (!playerIsWhite) displayedEval *= -1;

        int elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::steady_clock::now() - searchStartTime
                      ).count();

        Operations::gotoxy(0, 11);
        Operations::setTextColor(10);
        std::cout << "Computer chose: " << name << from << " → " << to
                  << " | Eval: " << (displayedEval > 0 ? "+" : "") << displayedEval
                  << " | Time: " << elapsed / 1000.0 << "s";
    }

    return bestMove;
}

std::vector<Move> Game::generateSortedMoves(bool forComputer) {
    std::vector<Move> moves;

    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            ChessPiece* piece = board.getChessPiece(x, y);
            if (!piece || piece->isWhite() != forComputer) continue;

            for (int ty = 0; ty < 8; ++ty) {
                for (int tx = 0; tx < 8; ++tx) {
                    if (!piece->canMove(tx, ty, board)) continue;

                    ChessPiece* captured = board.getChessPiece(tx, ty);
                    moves.push_back({ x, y, tx, ty, piece, captured });
                }
            }
        }
    }

    std::sort(moves.begin(), moves.end(), [&](const Move& a, const Move& b) {
        int scoreA = 0;
        int scoreB = 0;
    
        if (a.capturedPiece) {
            int victim = pieceValues[a.capturedPiece->getTypeIndex()];
            int attacker = pieceValues[a.movedPiece->getTypeIndex()];
            scoreA = 10 * victim - attacker;
        }
    
        if (b.capturedPiece) {
            int victim = pieceValues[b.capturedPiece->getTypeIndex()];
            int attacker = pieceValues[b.movedPiece->getTypeIndex()];
            scoreB = 10 * victim - attacker;
        }
    
        return scoreA > scoreB;
    });
    
    return moves;
}
