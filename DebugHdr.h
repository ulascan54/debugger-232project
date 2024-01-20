#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAXIMUM_COLUMN_LENGTH 100
#define MAXIMUM_ROW_LENGTH 80
#define TRACES_COUNT 20

struct tr
{
    char name;
    int value;
};

void add_TT(char var);
void display_TT(char var);
void update_TT(char var, int val);
void process_temp_file(const char *input_filename, const char *output_filename);

struct tr Traces[TRACES_COUNT];
int nTr = 0;

void add_TT(char var)
{

    if (nTr < TRACES_COUNT)
    {
        Traces[nTr].name = var;
        Traces[nTr].value = 0;
        nTr++;
    }
    else
    {
        fprintf(stderr, "There is no place left to put traces\n");
        exit(EXIT_FAILURE);
    }
}

void display_TT(char var)
{

    for (int i = 0; i < nTr; i++)
    {
        if (Traces[i].name == var)
        {

            printf("%c: %d\n", var, Traces[i].value);
            printf("Press enter to continue..");
            getchar();
            return;
        }
    }

    printf("Variable %c not found in traces.\n", var);
}

void update_TT(char var, int val)
{

    for (int i = 0; i < nTr; i++)
    {
        if (Traces[i].name == var)
        {

            Traces[i].value = val;
            return;
        }
    }

    printf("Variable %c not found in traces.\n", var);
}

void process_temp_file(const char *input_filename, const char *output_filename)
{
    FILE *input_file = fopen(input_filename, "r");
    if (!input_file)
    {
        perror("error when opening input file");
        exit(EXIT_FAILURE);
    }

    FILE *output_file = fopen(output_filename, "w");
    if (!output_file)
    {
        perror("error when opening output file");
        fclose(input_file);
        exit(EXIT_FAILURE);
    }

    char line[MAXIMUM_ROW_LENGTH];
    while (fgets(line, MAXIMUM_ROW_LENGTH, input_file) != NULL)
    {
        int i = 0;
        while (line[i] == ' ' || line[i] == '\t')
        {
            i++;
        }

        if (line[i] == '@')
        {
            char directive[MAXIMUM_ROW_LENGTH], var;
            sscanf(line + i, "%s %c", directive, &var);

            if (directive[1] == 'v')
            {
                fprintf(output_file, "\tadd_TT('%c');\n", var);
                add_TT(var);
            }
            else if (directive[1] == 't')
            {
                fprintf(output_file, "\tdisplay_TT('%c');\n", var);
            }
        }
        else if (line[i] != '\n')
        {
            char lhs_var;
            sscanf(line + i, "%c", &lhs_var);

            int traced = 0;
            for (int j = 0; j < nTr; j++)
            {
                if (Traces[j].name == lhs_var)
                {
                    fprintf(output_file, "%s", line + i);
                    fprintf(output_file, "\tupdate_TT('%c', %c);\n", lhs_var, lhs_var);
                    traced = 1;
                    break;
                }
            }

            if (!traced)
            {

                fprintf(output_file, "%s", line);
            }
        }
        else
        {
            fprintf(output_file, "\n");
        }
    }

    fclose(input_file);
    fclose(output_file);
}
