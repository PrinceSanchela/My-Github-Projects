
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
  #include <sys/select.h>
  #include <fcntl.h>
#endif

#define RECORD_FILE "record.txt"
#define MAXLINE 64

/* color theme */
static int useAnsi = 1;
static void c_reset(){ if(useAnsi) printf("\033[0m"); }
static void c_green(){ if(useAnsi) printf("\033[1;32m"); }
static void c_blue(){ if(useAnsi) printf("\033[1;34m"); }
static void c_cyan(){ if(useAnsi) printf("\033[1;36m"); }
static void c_yellow(){ if(useAnsi) printf("\033[1;33m"); }
static void c_red(){ if(useAnsi) printf("\033[1;31m"); }

static void msleep(int ms){
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms*1000);
#endif
}

#ifdef _WIN32
static void enableAnsiOnWindows(){
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) { useAnsi = 0; return; }
    DWORD mode = 0;
    if (!GetConsoleMode(hOut,&mode)) { useAnsi = 0; return; }
    mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    if (!SetConsoleMode(hOut, mode)) useAnsi = 0;
}
#endif

/* ========== Terminal handling for POSIX (Linux/macOS) ========== */
#ifndef _WIN32
static struct termios saved_tio;
static int saved_tio_saved = 0;

/* Save current canonical settings (call once at startup) */
static void save_orig_terminal(void){
    if (!saved_tio_saved) {
        tcgetattr(STDIN_FILENO, &saved_tio);
        saved_tio_saved = 1;
    }
}

/* Restore original terminal settings */
static void restore_terminal(void){
    if (saved_tio_saved) {
        tcsetattr(STDIN_FILENO, TCSANOW, &saved_tio);
    }
}

/* Put terminal in canonical blocking mode (normal) */
static void set_terminal_canonical(void){
    if (!saved_tio_saved) save_orig_terminal();
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_tio);
}

/* Put terminal in raw-like mode but keep fd BLOCKING.
   We will use select() to poll stdin with a timeout. */
static void set_terminal_raw_blocking(void){
    struct termios tio;
    if (!saved_tio_saved) save_orig_terminal();
    tcgetattr(STDIN_FILENO, &tio);
    tio.c_lflag &= ~(ICANON | ECHO); /* turn off canonical and echo */
    tio.c_cc[VMIN] = 1; /* ensure read() will block once data available */
    tio.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &tio);
}
#endif

/* ---------- cross-platform kbhit / getch for gameplay ---------- */
/* We will implement kbhit_nb using select() on POSIX (no O_NONBLOCK).
   getch_nb will read exactly one char with read() after select indicates data. */

static int kbhit_nb(void){
#ifdef _WIN32
    return _kbhit();
#else
    fd_set rfds;
    struct timeval tv = {0, 0}; /* zero timeout -> poll */
    FD_ZERO(&rfds);
    FD_SET(STDIN_FILENO, &rfds);
    int r = select(STDIN_FILENO + 1, &rfds, NULL, NULL, &tv);
    return (r > 0) && FD_ISSET(STDIN_FILENO, &rfds);
#endif
}

static int getch_nb(void){
#ifdef _WIN32
    return _getch();
#else
    unsigned char ch;
    ssize_t n = read(STDIN_FILENO, &ch, 1);
    if (n == 1) return (int)ch;
    return -1;
#endif
}

/* ---------- utility UI & game code (unchanged semantics) ---------- */

static void clear_screen(void){
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void typeText(const char *s,int delay){
    for (int i=0; s[i]; ++i){
        putchar(s[i]);
        fflush(stdout);
        msleep(delay);
    }
}

static void center_print(const char *s,int width){
    int len = (int)strlen(s);
    int pad = (width - len) / 2;
    if (pad < 0) pad = 0;
    for (int i=0;i<pad;i++) putchar(' ');
    puts(s);
}

/* PANDA ASCII art (5x) */
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

static void show_big_letter(const char **pattern,int lines){
    clear_screen();
    int width = 80;
    for (int i=0;i<lines;i++) center_print(pattern[i], width);
    fflush(stdout);
}

static void panda_intro_letters(void){
    const char letters[5] = {'P','A','N','D','A'};
    const char *names[5] = {"Prince Sanchela","Aakash","Nivedya","Dev","Aastha"};
    const char *tag[5] = {
        "Power of Innovation",
        "Aim for Excellence",
        "Never Give Up",
        "Dedication and Discipline",
        "Achieve the Impossible"
    };
    for (int i=0;i<5;i++){
        if (letters[i]=='P'){ c_green(); show_big_letter(big_P,5); c_reset(); }
        else if (letters[i]=='A'){ c_green(); show_big_letter(big_A,5); c_reset(); }
        else if (letters[i]=='N'){ c_green(); show_big_letter(big_N,5); c_reset(); }
        else if (letters[i]=='D'){ c_green(); show_big_letter(big_D,5); c_reset(); }

        msleep(700);

        clear_screen();
        c_blue(); center_print("TEAM PANDA",80); c_reset();
        putchar('\n');
        c_cyan(); typeText("Team Member : ",20); c_reset();
        typeText(names[i],30); putchar('\n');
        c_cyan(); typeText(" - ",20); c_yellow(); typeText(tag[i],25); putchar('\n');
        c_reset();
        msleep(1100);
    }
    clear_screen();
    c_green();
    center_print("PPPP     AAAAA   N   N   DDDD     AAAAA",80);
    center_print("P   P   A     A  NN  N   D   D   A     A",80);
    center_print("PPPP    AAAAAAA  N N N   D   D   AAAAAAA",80);
    center_print("P       A     A  N  NN   D   D   A     A",80);
    center_print("P       A     A  N   N   DDDD    A     A",80);
    c_reset();
    msleep(900);
}

static void ascii_loader(void){
    c_cyan(); typeText("\nLoading Game...\n",18); c_reset();
    int width = 40;
    printf("[");
    fflush(stdout);
    for (int i=0;i<width;i++){
        c_green(); putchar('#'); c_reset();
        fflush(stdout);
        msleep(45);
    }
    printf("] 100%%\n\n");
    msleep(300);
}

/* load/save best */
static int load_best(void){
    FILE *f = fopen(RECORD_FILE,"r");
    if (!f) return -1;
    int t=-1;
    if (fscanf(f,"%d",&t)!=1) t=-1;
    fclose(f);
    return t;
}
static void save_best(int t){
    FILE *f = fopen(RECORD_FILE,"w");
    if (!f) return;
    fprintf(f,"%d\n",t);
    fclose(f);
}

/* redraw prompt */
static void redraw_prompt(const char *prompt,const char *buf,int seconds_left){
    printf("\r\x1b[2K");
    c_cyan(); printf("[Time %2ds] ", seconds_left); c_reset();
    printf("%s%s", prompt, buf);
    fflush(stdout);
}

/* gameplay loop using select() polling (POSIX) or _kbhit/_getch (Windows) */
static int play_round(int maxnum, int time_limit){
    int secret = (rand() % maxnum) + 1;
    char buf[MAXLINE]; buf[0] = '\0';
    int buflen = 0;
    char prompt[] = "Guess: ";
    time_t start = time(NULL);
    int prev_sec = -1;

    /* If POSIX, ensure terminal is in raw-blocking mode before calling play_round */
#ifndef _WIN32
    set_terminal_raw_blocking:
    ; /* label placeholder - done by caller */
#endif

    while (1){
        int left = time_limit - (int)(time(NULL) - start);
        if (left < 0) left = 0;
        if (left != prev_sec){
            redraw_prompt(prompt, buf, left);
            prev_sec = left;
        }

#ifdef _WIN32
        if (_kbhit()){
            int ch = _getch();
            if (ch == '\r' || ch == '\n'){
                buf[buflen]='\0';
                printf("\n");
                if (buflen==0){
                    buflen=0; buf[0]='\0'; redraw_prompt(prompt,buf,left);
                } else {
                    int guess = atoi(buf);
                    if (guess < 1 || guess > maxnum){
                        c_yellow(); printf("Out of range. Use 1..%d\n", maxnum); c_reset();
                    } else if (guess == secret){
                        int used = time_limit - left;
                        c_green(); printf("Correct. %d found in %d seconds\n", secret, used); c_reset();
                        return used;
                    } else if (guess < secret){
                        c_red(); printf("Hint: GREATER than %d\n", guess); c_reset();
                    } else {
                        c_red(); printf("Hint: SMALLER than %d\n", guess); c_reset();
                    }
                    buflen = 0; buf[0] = '\0'; redraw_prompt(prompt,buf,left);
                }
            } else if (ch == 8 || ch == 127){
                if (buflen>0) buf[--buflen]=0;
                redraw_prompt(prompt,buf,left);
            } else if (ch >= '0' && ch <= '9'){
                if (buflen < MAXLINE-1) buf[buflen++] = (char)ch, buf[buflen]=0;
                redraw_prompt(prompt,buf,left);
            }
        }
#else
        /* POSIX: use select() with small timeout */
        fd_set rfds;
        struct timeval tv;
        FD_ZERO(&rfds);
        FD_SET(STDIN_FILENO, &rfds);
        tv.tv_sec = 0;
        tv.tv_usec = 100000; /* 100 ms - keeps UI responsive and timer accurate */
        int rv = select(STDIN_FILENO+1, &rfds, NULL, NULL, &tv);
        if (rv > 0 && FD_ISSET(STDIN_FILENO, &rfds)){
            /* read one byte */
            unsigned char ch;
            ssize_t n = read(STDIN_FILENO, &ch, 1);
            if (n == 1){
                if (ch == '\r' || ch == '\n'){
                    buf[buflen] = '\0';
                    printf("\n");
                    if (buflen == 0){
                        buflen = 0; buf[0] = '\0'; redraw_prompt(prompt,buf,left);
                    } else {
                        int guess = atoi(buf);
                        if (guess < 1 || guess > maxnum){
                            c_yellow(); printf("Out of range. Use 1..%d\n", maxnum); c_reset();
                        } else if (guess == secret){
                            int used = time_limit - left;
                            c_green(); printf("Correct. %d found in %d seconds\n", secret, used); c_reset();
                            return used;
                        } else if (guess < secret){
                            c_red(); printf("Hint: GREATER than %d\n", guess); c_reset();
                        } else {
                            c_red(); printf("Hint: SMALLER than %d\n", guess); c_reset();
                        }
                        buflen = 0; buf[0] = '\0'; redraw_prompt(prompt,buf,left);
                    }
                } else if (ch == 127 || ch == 8){
                    if (buflen > 0){ buf[--buflen] = '\0'; redraw_prompt(prompt,buf,left); }
                } else if (ch >= '0' && ch <= '9'){
                    if (buflen < MAXLINE-1){ buf[buflen++] = (char)ch; buf[buflen]='\0'; redraw_prompt(prompt,buf,left); }
                } else {
                    /* Ignore other bytes (including escape sequences for arrows) */
                }
            }
        } else {
            /* select timed out - loop will update timer */
        }
#endif

        if (left == 0) { /* check again if time up after blocking select/read */
            printf("\n");
            c_red(); printf("Time expired. The number was %d\n", secret); c_reset();
            return -1;
        }
        /* small sleep to avoid 100% CPU when select had zero timeout (safe) */
        /* On POSIX, select already waited up to 100ms; on Windows we used kbhit loop so sleep a bit */
#ifndef _WIN32
        /* nothing extra */
#else
        msleep(60);
#endif
    }
}

/* --------------------- main --------------------- */
int main(void){
    srand((unsigned)time(NULL));

#ifdef _WIN32
    enableAnsiOnWindows();
#else
    save_orig_terminal:
    save_orig_terminal: ; /* placeholder */
    save_orig_terminal: /* we will call save_orig_terminal below */ ;
    save_orig_terminal();
    atexit(restore_terminal);
#endif

    /* Intro + loader */
    panda_intro_letters();
    ascii_loader();

    bool play = true;
    while (play){
        /* Ensure canonical blocking mode for menus */
#ifndef _WIN32
        set_terminal_canonical();
#endif
        clear_screen();
        int best = load_best();
        c_blue(); printf("=== Guess-the-Number (PANDA) ===\n"); c_reset();
        if (best > 0) printf("Fastest solve: %d s (stored in %s)\n", best, RECORD_FILE);
        else printf("No record yet.\n");

        printf("\nSelect difficulty:\n 1) Easy  (1-50 , 30s)\n 2) Normal(1-100, 20s)\n 3) Hard  (1-200, 12s)\nChoose [1-3]: ");
        int choice = 2;
        if (scanf("%d", &choice) != 1){
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            choice = 2;
        } else {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }

        int maxnum = 100, tlim = 20;
        if (choice == 1){ maxnum = 50; tlim = 30; }
        else if (choice == 3){ maxnum = 200; tlim = 12; }
        else { maxnum = 100; tlim = 20; }

        printf("\nYou have %d seconds to guess a number between 1 and %d.\n", tlim, maxnum);
        printf("Press Enter to start...");
        while (getchar() != '\n'); /* blocking */

        /* Switch terminal to raw-blocking mode and use select() for input */
#ifndef _WIN32
        set_terminal_raw_blocking();
#endif

        int used = play_round(maxnum, tlim);

        /* After gameplay restore canonical blocking for menus */
#ifndef _WIN32
        set_terminal_canonical();
#endif

        if (used >= 0){
            int score = 100 + (tlim - used) * 3;
            printf("Score: %d\n", score);
            int best2 = load_best();
            if (best2 == -1 || used < best2){
                printf("New record! Saving...\n");
                save_best(used);
            }
        }

        printf("\nPlay again? (y/n): ");
        char reply = 'n';
        if (scanf(" %c", &reply) != 1){
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
            reply = 'n';
        } else {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);
        }
        play = (reply == 'y' || reply == 'Y');
    }

    /* restore terminal on exit */
#ifndef _WIN32
    restore_terminal();
#endif

    return 0;
}
