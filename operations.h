#pragma once
#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <iostream>
#include <windows.h>
#include <cstdlib>

namespace Operations {

    inline void gotoxy(int x, int y) {
        COORD coord;
        coord.X = x;
        coord.Y = y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    }

    inline void clearScreen() {
        system("cls");
    }

    inline void hideCursor() {
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO info;
        info.dwSize = 100;
        info.bVisible = FALSE;
        SetConsoleCursorInfo(consoleHandle, &info);
    }

    inline void showCursor() {
        HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
        CONSOLE_CURSOR_INFO info;
        info.dwSize = 20;
        info.bVisible = TRUE;
        SetConsoleCursorInfo(consoleHandle, &info);
    }

    inline void setConsoleSize(int width, int height) {
        HWND console = GetConsoleWindow();
        RECT rect;
        GetWindowRect(console, &rect);
        MoveWindow(console, rect.left, rect.top, width, height, TRUE);
    }

    inline void setTextColor(int colorCode) {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), colorCode);
    }

    inline void resetTextColor() {
        SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    }

    inline bool isWithinBoard(int x, int y) {
        return (x >= 0 && x < 8 && y >= 0 && y < 8);
    }

}

#endif
