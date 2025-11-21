#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>

#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#include <termios.h>
#include <sys/time.h>
#include <fcntl.h>
#endif

#define RECORD_FILE "record.txt"
#define MAXLINE 64

/* --- color theme (cyber look) --- */
static int useAnsi = 1;
static void c_reset() { if (useAnsi) printf("\033[0m"); }
static void c_green() { if (useAnsi) printf("\033[1;32m"); }
static void c_blue() { if (useAnsi) printf("\033[1;34m"); }
static void c_cyan() { if (useAnsi) printf("\033[1;36m"); }
static void c_yellow() { if (useAnsi) printf("\033[1;33m"); }
static void c_red() { if (useAnsi) printf("\033[1;31m"); }

/* --- small utility sleep --- */
static void msleep(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

/* --- attempt to enable ANSI on Win --- */
#ifdef _WIN32
static void enableAnsiOnWindows() {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) { useAnsi = 0; return; }
    DWORD mode = 0;
    if (!GetConsoleMode(hOut, &mode)) { useAnsi = 0; return; }
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, mode)) useAnsi = 0;
}
#endif

/* --- non-blocking input helpers (kept same as original) --- */
#ifdef _WIN32
static int kbhit_nb() { return _kbhit(); }
static int getch_nb() { return _getch(); }
#else
static struct termios orig_tio;
static int orig_flags = -1; /* to restore O_NONBLOCK state */

/* restore terminal attributes and blocking flag */
static void restore_terminal() {
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_tio);
    if (orig_flags != -1) {
        fcntl(STDIN_FILENO, F_SETFL, orig_flags);
        orig_flags = -1;
    }
}

/* configure terminal for non-blocking raw input (gameplay) */
static void configure_terminal_nb() {
    struct termios tio;
    tcgetattr(STDIN_FILENO, &orig_tio);
    tio = orig_tio;
    tio.c_lflag &= ~(ICANON | ECHO);
    tio.c_cc[VMIN] = 0;
    tio.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &tio);

    /* save and set file status flags to non-blocking */
    orig_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, orig_flags | O_NONBLOCK);
}

/* helper: set terminal to blocking canonical mode (for scanf/getchar) */
static void configure_terminal_blocking() {
    /* restore attributes then ensure blocking flag */
    tcsetattr(STDIN_FILENO, TCSANOW, &orig_tio);
    if (orig_flags != -1) {
        /* orig_flags already stored by configure_terminal_nb earlier; restore it */
        fcntl(STDIN_FILENO, F_SETFL, orig_flags);
        orig_flags = -1;
    } else {
        /* if orig_flags not set (e.g. first time), ensure blocking */
        int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
        fcntl(STDIN_FILENO, F_SETFL, flags & ~O_NONBLOCK);
    }
}

static int kbhit_nb() {
    int c = getchar();
    if (c == EOF) return 0;
    ungetc(c, stdin);
    return 1;
}
static int getch_nb() {
    int c = getchar();
    if (c == EOF) return -1;
    return c;
}
#endif

/* --- clear screen --- */
static void clear_screen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

/* --- typing effect --- */
static void typeText(const char *s, int delay) {
    for (int i = 0; s[i]; ++i) {
        putchar(s[i]);
        fflush(stdout);
        msleep(delay);
    }
}

/* --- center print helper --- */
static void center_print(const char *s, int width) {
    int len = (int)strlen(s);
    int pad = (width - len) / 2;
    if (pad < 0) pad = 0;
    for (int i = 0; i < pad; i++) putchar(' ');
    puts(s);
}

/* --- Big-letter patterns for P A N D A (5 lines each, ASCII-safe) --- */
static const char *big_P[5] = {
    "PPPP   ",
    "P   P  ",
    "PPPP   ",
    "P      ",
    "P      "
};
static const char *big_A[5] = {
    " AAAAA ",
    " A   A ",
    " AAAAA ",
    " A   A ",
    " A   A "
};
static const char *big_N[5] = {
    "N   N ",
    "NN  N ",
    "N N N ",
    "N  NN ",
    "N   N "
};
static const char *big_D[5] = {
    "DDDD  ",
    "D   D ",
    "D   D ",
    "D   D ",
    "DDDD  "
};

/* show single big letter centered */
static void show_big_letter(const char **pattern, int lines) {
    clear_screen();
    int width = 80;
    for (int i = 0; i < lines; i++) center_print(pattern[i], width);
    fflush(stdout);
}

/* show PANDA intro - letter by letter (Option A large style) */
static void panda_intro_letters() {
    const char letters[5] = {'P', 'A', 'N', 'D', 'A'};
    const char *names[5] = {"Prince Sanchela", "Aakash", "Nivedya", "Dev", "Aastha"};
    const char *tagline[5] = {
        "Power of Innovation",
        "Aim for Excellence",
        "Never Give Up",
        "Dedication and Discipline",
        "Achieve the Impossible"
    };

    for (int i = 0; i < 5; i++) {
        if (letters[i] == 'P') { c_green(); show_big_letter(big_P, 5); c_reset(); }
        else if (letters[i] == 'A') { c_green(); show_big_letter(big_A, 5); c_reset(); }
        else if (letters[i] == 'N') { c_green(); show_big_letter(big_N, 5); c_reset(); }
        else if (letters[i] == 'D') { c_green(); show_big_letter(big_D, 5); c_reset(); }

        msleep(700);

        clear_screen();
        c_blue(); center_print("TEAM PANDA", 80); c_reset();
        printf("\n");
        c_cyan(); typeText("Team Member : ", 20); c_reset(); typeText(names[i], 30); putchar('\n');
        c_cyan(); typeText("\t - ", 20); c_yellow(); typeText(tagline[i], 25); putchar('\n');
        fflush(stdout);
        msleep(1100);
    }

    clear_screen();
    c_green();
    center_print("PPPP     AAAAA   N   N   DDDD     AAAAA", 80);
    center_print("P   P   A     A  NN  N   D   D   A     A", 80);
    center_print("PPPP    AAAAAAA  N N N   D   D   AAAAAAA", 80);
    center_print("P       A     A  N  NN   D   D   A     A", 80);
    center_print("P       A     A  N   N   DDDD    A     A", 80);
    c_reset();
    msleep(900);
}

/* --- ASCII progress loader (hash style) --- */
static void ascii_loader() {
    c_cyan(); typeText("\nLoading Game...\n", 18); c_reset();
    int width = 30;
    printf("[");
    fflush(stdout);
    for (int i = 0; i < width; i++) {
        c_green(); putchar('#'); c_reset();
        fflush(stdout);
        msleep(50);
    }
    printf("] 100%%\n\n");
    msleep(400);
}

/* ===== The original game code (kept behavior identical) ===== */

/* load/save best time */
static int load_best() {
    FILE *f = fopen(RECORD_FILE, "r");
    if (!f) return -1;
    int t = -1;
    if (fscanf(f, "%d", &t) != 1) t = -1;
    fclose(f);
    return t;
}
static void save_best(int t) {
    FILE *f = fopen(RECORD_FILE, "w");
    if (!f) return;
    fprintf(f, "%d\n", t);
    fclose(f);
}

/* redraw prompt */
static void redraw_prompt(const char *prompt, const char *buf, int seconds_left) {
    printf("\r\x1b[2K");
    c_cyan(); printf("[Time %2ds] ", seconds_left); c_reset();
    printf("%s%s", prompt, buf);
    fflush(stdout);
}

/* play_round (unchanged logic) */
static int play_round(int maxnum, int time_limit) {
    int secret = (rand() % maxnum) + 1;
    char buf[MAXLINE]; buf[0] = '\0'; int buflen = 0;
    char prompt[] = "Guess: ";
    time_t start = time(NULL);
    int prev_sec = -1;

    while (1) {
        int left = time_limit - (int)(time(NULL) - start);
        if (left < 0) left = 0;
        if (left != prev_sec) { redraw_prompt(prompt, buf, left); prev_sec = left; }

        if (left == 0) {
            printf("\n");
            c_red(); printf("Time expired. The number was %d\n", secret); c_reset();
            return -1;
        }

        if (kbhit_nb()) {
            int ch = getch_nb();
            if (ch == '\r' || ch == '\n') {
                buf[buflen] = '\0';
                printf("\n");
                if (buflen == 0) {
                    buflen = 0; buf[0] = '\0'; redraw_prompt(prompt, buf, left);
                } else {
                    int guess = atoi(buf);
                    if (guess < 1 || guess > maxnum) {
                        c_yellow(); printf("Out of range. Use 1..%d\n", maxnum); c_reset();
                    } else if (guess == secret) {
                        int used = time_limit - left;
                        c_green(); printf("Correct. %d found in %d seconds\n", secret, used); c_reset();
                        return used;
                    } else if (guess < secret) {
                        c_red(); printf("Hint: GREATER than %d\n", guess); c_reset();
                    } else {
                        c_red(); printf("Hint: SMALLER than %d\n", guess); c_reset();
                    }
                    buflen = 0; buf[0] = '\0'; redraw_prompt(prompt, buf, left);
                }
            } else if (ch == 127 || ch == 8) {
                if (buflen > 0) { buflen--; buf[buflen] = '\0'; redraw_prompt(prompt, buf, left); }
            } else if (ch >= 32 && ch < 127) {
                if (buflen < MAXLINE - 2) { buf[buflen++] = (char)ch; buf[buflen] = '\0'; redraw_prompt(prompt, buf, left); }
            }
        }
        msleep(60);
    }
}

/* main */
int main(void) {
    srand((unsigned)time(NULL));
#ifdef _WIN32
    enableAnsiOnWindows();
#else
    /* Initially set terminal to blocking canonical mode (save orig settings) */
    tcgetattr(STDIN_FILENO, &orig_tio);
    orig_flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    atexit(restore_terminal);
#endif

    /* run PANDA letter-by-letter intro (large option A) */
    panda_intro_letters();

    /* progress loader */
    ascii_loader();

    bool per = true;
    while (per) {
        /* clear and start game */
        clear_screen();

        int best = load_best();
        c_blue(); printf("=== Guess-the-Number (Professional Build) ===\n"); c_reset();
        if (best > 0) printf("Fastest solve: %d s (stored in %s)\n", best, RECORD_FILE);
        else printf("No record yet.\n");

        /* --- IMPORTANT: ensure blocking canonical mode for menu input --- */
#ifdef _WIN32
        /* Windows stdin is fine */
#else
        configure_terminal_blocking(); /* restore canonical + blocking */
#endif

        printf("\nSelect difficulty:\n");
        printf(" 1) Easy  (1-50 , 30s)\n");
        printf(" 2) Normal(1-100, 20s)\n");
        printf(" 3) Hard  (1-200, 12s)\n");
        printf("Choose [1-3]: ");

        int diff = 2;
        int c = 0;
        if (scanf("%d", &c) != 1) {
            /* fallback if scanf fails, keep default */
            c = diff;
            /* clear input */
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        } else {
            /* flush rest of line */
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        if (c >= 1 && c <= 3) diff = c;

        int maxnum = 100, tlim = 20;
        if (diff == 1) { maxnum = 50; tlim = 30; }
        else if (diff == 2) { maxnum = 100; tlim = 20; }
        else { maxnum = 200; tlim = 12; }

        printf("\nYou have %d seconds to guess a number between 1 and %d.\n", tlim, maxnum);
        printf("Press Enter to start...");
        /* wait for Enter in blocking mode */
        while (getchar() != '\n');

        /* Switch to non-blocking raw mode for gameplay */
#ifdef _WIN32
        /* Windows: no change required */
#else
        configure_terminal_nb();
#endif

        int used = play_round(maxnum, tlim);

        /* After gameplay, restore blocking before printing results & asking to replay */
#ifdef _WIN32
        /* nothing */
#else
        configure_terminal_blocking();
#endif

        if (used >= 0) {
            int score = 100 + (tlim - used) * 3;
            printf("Score: %d\n", score);
            int best2 = load_best();
            if (best2 == -1 || used < best2) {
                printf("New record time. Saving.\n");
                save_best(used);
            }
        }

        char yn = 'n';
        printf("\nAre you want to Play Again Game [Yes = y and No = n]: ");
        /* blocking read for yes/no */
        if (scanf(" %c", &yn) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            yn = 'n';
        } else {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        per = (yn == 'y' || yn == 'Y');
    }

    /* restore terminal before exit */
#ifndef _WIN32
    restore_terminal();
#endif

    return 0;
}
