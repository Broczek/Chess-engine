#include <iostream>
#include <windows.h>
#include "Game.h"

int main() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    Game game;
    game.run();

    return 0;
}
