#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DebugHdr.h"

#define MAXIMUM_ROW_LENGTH 80
#define MAXIMUM_COLUMN_LENGTH 100
#define INPUT_FILENAME "myprog.c"
#define OUTPUT_FILENAME "expanded.c"
#define TEMP_FILE "temp"

struct buf
{
    char line[MAXIMUM_ROW_LENGTH];
    int link;
};

struct buf Buffer[MAXIMUM_COLUMN_LENGTH];
int head = -1;
int programSize = 0;
int cursor_row = 0;

void read_to_buffer(const char *filename);
void add_to_buffer();
void write_to_file(const char *filename);
void display_program();

void setColors(int textColor, int cursorColor)
{
    start_color();

    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, textColor, cursorColor);

    attron(COLOR_PAIR(1));

    mousemask(ALL_MOUSE_EVENTS, NULL);
    mouseinterval(0);
    keypad(stdscr, TRUE);
    curs_set(0);
    attroff(COLOR_PAIR(1));
}

int main()
{
    initscr();

    setColors(COLOR_WHITE, COLOR_RED);

    read_to_buffer(INPUT_FILENAME);

    keypad(stdscr, TRUE);
    noecho();

    int ch;
    int temp_y, temp_x;
    getmaxyx(stdscr, temp_y, temp_x);
    cursor_row = temp_y;
    cursor_row = programSize - 1;
    move(cursor_row, 0);
    display_program();

    while ((ch = getch()) != 'x')
    {
        if (ch == 'd')
        {
            write_to_file(TEMP_FILE);
            read_to_buffer(TEMP_FILE);
            display_program();
        }
        else if (ch == 'w')
        {
            if (cursor_row <= 0)
            {
                cursor_row = programSize - 1;
            }
            else
            {
                cursor_row--;
            }
            move(cursor_row, 0);
            display_program();
        }
        else if (ch == 's')
        {
            if (cursor_row >= programSize - 1)
            {
                cursor_row = 0;
            }
            else
            {
                cursor_row++;
            }
            move(cursor_row, 0);
            display_program();
        }
        else if (ch == 'i')
        {
            add_to_buffer();
        }
    }

    endwin();

    process_stemp_file(TEMP_FILE, OUTPUT_FILENAME);

    return 0;
}

void read_to_buffer(const char *filename)
{
    head = -1;
    programSize = 0;

    FILE *file = fopen(filename, "r");
    if (file == NULL)
    {
        perror("Error occurs when opening file");
        exit(EXIT_FAILURE);
    }

    char line[MAXIMUM_ROW_LENGTH];
    while (fgets(line, sizeof(line), file) != NULL)
    {
        programSize++;
        int newNode = head + 1;

        if (newNode < MAXIMUM_COLUMN_LENGTH)
        {
            strcpy(Buffer[newNode].line, line);
            Buffer[newNode].link = -1;

            if (head != -1)
            {
                Buffer[head].link = newNode;
            }

            head = newNode;
        }
        else
        {
            fprintf(stderr, "Buffer overflow occurs here\n");
            fclose(file);
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);
}

void add_to_buffer()
{
    programSize++;
    int newNode = cursor_row + 1;

    if (newNode <= programSize)
    {
        move(LINES - 1, 0);
        printw("Enter directive type (var/trace): ");
        refresh();
        curs_set(2);

        char directive[MAXIMUM_ROW_LENGTH - 5];
        echo();
        getstr(directive);
        noecho();
        curs_set(0);

        char formattedDirective[MAXIMUM_ROW_LENGTH];
        snprintf(formattedDirective, MAXIMUM_ROW_LENGTH, "    %s\n", directive);

        for (int i = programSize; i >= newNode; --i)
        {
            Buffer[i + 1] = Buffer[i];
        }

        strcpy(Buffer[newNode].line, formattedDirective);

        Buffer[newNode - 1].link = newNode;
        Buffer[newNode].link = newNode + 1;
        Buffer[newNode + 1].link = -1;

        write_to_file(TEMP_FILE);
        read_to_buffer(TEMP_FILE);

        move(++cursor_row, 0);
        display_program();
    }
    else
    {
        fprintf(stderr, "Buffer overflow occurs here\n");
        exit(EXIT_FAILURE);
    }
}

void write_to_file(const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (file == NULL)
    {
        perror("Error occurs when open");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i <= programSize; ++i)
    {
        fprintf(file, "%s", Buffer[i].line);
    }

    fclose(file);
}

void display_program()
{
    clear();

    int i = 0;
    while (i < programSize)
    {
        if (cursor_row == i)
        {
            attron(COLOR_PAIR(2));
            mvprintw(i, 0, "%s", Buffer[i].line);
            attroff(COLOR_PAIR(2));
        }
        else
        {
            mvprintw(i, 0, "%s", Buffer[i].line);
        }

        i++;
    }

    refresh();
}
