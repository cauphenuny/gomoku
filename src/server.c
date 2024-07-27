// author: Cauphenuny <https://cauphenuny.github.io/>
// date: 2024/07/24
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

#include "util.h"
#include "board.h"
#include "players.h"

void wrap_area(const board_t bd, int *top, int *bottom, int *left, int *right, int radius) {
    int n = BOARD_SIZE, mid = n / 2;
    *top = *left = mid - radius;
    *bottom = *right = mid + radius + 1;
    chkmin(*right, n), chkmin(*bottom, n);
    chkmax(*top, 0), chkmax(*left, 0);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            if (bd[i][j]) {
                chkmin(*top,  max(0, i - radius));
                chkmin(*left, max(0, j - radius));
                chkmax(*bottom, min(i + radius + 1, n));
                chkmax(*right,  min(j + radius + 1, n));
            }
        }
    }
}

char id2name(int id) {
    if (id < 10) {
        return id + '0';
    } else {
        return id - 10 + 'A';
    }
}

void print(const board_t bd) {
    // 0: empty, 1/2: prev p1/p2 piece, 3/4: cur p1/p2 piece
    char* ch[5] = { 
        " ", 
        "\033[" CLI_COLOR_GREEN "mo\033[0m",
        "\033[" CLI_COLOR_RED "mx\033[0m",
        "\033[" CLI_STYLE_UNDERLINE "m\033[" CLI_STYLE_BOLD ";" CLI_COLOR_GREEN "mo\033[0m",
        "\033[" CLI_STYLE_UNDERLINE "m\033[" CLI_STYLE_BOLD ";" CLI_COLOR_RED "mx\033[0m",
    };
    int top = 0, bottom = BOARD_SIZE, left = 0, right = BOARD_SIZE;
    wrap_area(bd, &top, &bottom, &left, &right, 3);
    printf("   |");
    for (int i = left; i < right; i++) {
        printf(" %c |%c", id2name(i), "\n"[i != right - 1]);
    }
    for (int i = left; i < right + 1; i++) {
        printf("---+%c", "\n"[i != right]);
    }
    for (int i = top; i < bottom; i++) {
        printf(" %c |", id2name(i));
        for (int j = left; j < right; j++) {
            printf(" %s |%c", ch[bd[i][j]], "\n"[j != right - 1]);
        }
        for (int j = left; j < right + 1; j++) {
            printf("---+%c", "\n"[j != right]);
        }
    }
}

bool inboard(point_t pos) {
    int n = BOARD_SIZE;
    return pos.x >= 0 && pos.x < n && pos.y >= 0 && pos.y < n;
}

void refresh(board_t board) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (board[i][j] > 2) board[i][j] -= 2;
        }
    }
}

bool put(board_t board, int id, point_t pos) {
    if (!inboard(pos) || board[pos.x][pos.y]) return true;
    board[pos.x][pos.y] = id + 2;
    return false;
}

int check_draw(const board_t bd) {
    for (int i = 0; i < BOARD_SIZE; i++) {
        for (int j = 0; j < BOARD_SIZE; j++) {
            if (!bd[i][j]) return 0;
        }
    }
    return 1;
}

int check(const board_t bd, point_t pos) {
    int id = bd[pos.x][pos.y];
    if (!id) return 0;
    static const int arrows[4][2] = {{0, 1}, {1, 0}, {1, 1}, {-1, 1}};
    for (int i = 0, a, b, cnt; i < 4; i++) {
        a = arrows[i][0], b = arrows[i][1];
        point_t np = {pos.x, pos.y};
        for (cnt = 0; inboard(np); np.x += a, np.y += b) {
            if (bd[np.x][np.y] == id) cnt++;
            else break;
        }
        np = (point_t){pos.x - a, pos.y - b};
        for (; inboard(np); np.x -= a, np.y -= b) {
            if (bd[np.x][np.y] == id) cnt++;
            else break;
        }
        if (cnt >= WIN_LENGTH) return id == 1 ? 1 : -1;
    }
    return 0;
}

/*
   | 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9 | A |
---+---+---+---+---+---+---+---+---+---+---+---+
 0 |   |   |   |   |   |   |   |   |   |   |   |
---+---+---+---+---+---+---+---+---+---+---+---+
 1 |   |   |   |   |   |   |   |   |   |   |   |
---+---+---+---+---+---+---+---+---+---+---+---+
 2 |   |   |   |   |   |   |   |   |   |   |   |
---+---+---+---+---+---+---+---+---+---+---+---+
 3 |   |   |   |   |   |   |   |   |   |   |   |
---+---+---+---+---+---+---+---+---+---+---+---+
 4 |   |   |   |   | x |   |   |   |   |   |   |
---+---+---+---+---+---+---+---+---+---+---+---+
 5 |   |   |   | x | o | o |   | o | x |   |   |
---+---+---+---+---+---+---+---+---+---+---+---+
 6 |   |   |   |   | x |   | o |   |   |   |   |
---+---+---+---+---+---+---+---+---+---+---+---+
 7 |   |   |   |   |   | o |   | o |   |   |   |
---+---+---+---+---+---+---+---+---+---+---+---+
 8 |   |   |   |   |   |   |   |   | x |   |   |
---+---+---+---+---+---+---+---+---+---+---+---+
 9 |   |   |   |   |   |   |   |   |   |   |   |
---+---+---+---+---+---+---+---+---+---+---+---+
 A |   |   |   |   |   |   |   |   |   |   |   |
---+---+---+---+---+---+---+---+---+---+---+---+
reverse_flag = 1;
board[4][4] = board[5][3] = board[6][4] = board[5][8] = board[8][8] = 2;
board[5][4] = board[5][5] = board[7][5] = board[5][7] = board[6][6] = board[7][7] = 1;
*/

int game(int player1, int player2, int first) {
    board_t board;
    memset(board, 0, sizeof(board));
    print(board);
    point_t pos;
    int players[2] = {player1, player2};
    int id = first;
    while (1) {
        log("player%d's turn.", id);
        while (put(board, id, pos = move(players[id - 1], board, id))) {
            loge("invalid position!");
            getchar();
        }
        print(board);
        refresh(board);
        if (check_draw(board)) { log("draw."); return 0; }
        if (check(board, pos)) { log("player%d wins.", id); return id; }
        id = 3 - id;
    }
}
