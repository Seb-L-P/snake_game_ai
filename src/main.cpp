#include "SnakeGame.h"
#include <thread>
#include <chrono>
#include <iostream>
#include <termios.h>
#include <unistd.h>
#include <fcntl.h>

// --- Non-blocking keyboard input functions ---
int kbhit() {
    struct termios oldt, newt;
    int ch;
    int oldf;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);

    ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);

    if (ch != EOF) {
        ungetc(ch, stdin);
        return 1;
    }
    return 0;
}

char getch_nonblocking() {
    char c = 0;
    if (kbhit()) {
        c = getchar();
    }
    return c;
}
// ----------------------------------------------

int main() {
    SnakeGame game(20, 10);
    Direction currentDir = RIGHT;

    while (!game.isGameOver()) {
        // Read key if pressed (non-blocking)
        char c = getch_nonblocking();
        switch (c) {
            case 'w': currentDir = UP; break;
            case 's': currentDir = DOWN; break;
            case 'a': currentDir = LEFT; break;
            case 'd': currentDir = RIGHT; break;
        }

        // Step the game regardless of input
        game.step(currentDir);

        // Render to console
        system("clear");
        game.render();

        // Control speed
        std::this_thread::sleep_for(std::chrono::milliseconds(150));
    }

    std::cout << "Game Over! Final Score: " << game.getScore() << "\n";
}
