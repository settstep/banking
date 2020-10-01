/**
 * \file    main.c
 *
 * \date    2020-10-01
 *
 * \author  settstep
 *
 * \section sec_main_c_history Historie
 *
 * \par     2020-10-01 settstep
 * - added main function and program argument checking
 * - file created
 */

#include <stdio.h>
#include <string.h>

/**
 *  \brief list of valid arguments
 */
typedef enum
{
    ARG_INPUT,  ///< input file
    ARG_PPM,    ///< parts per million
    ARG_OUTPUT, ///< output file
    ARG_MAX     ///< number of valid arguments
}   argument_te;

const char *argList[] = {"-in", "-ppm", "-out"};    ///<  list of valid arguments

static void printInfo_v(void);

/**
 *  \brief main function
 *  \param arguments_i number of arguments
 *  \param argument    list of arguments
 *  \return 0
 */
int main(int arguments_i, char **argument)
{
    int i;
    int j;
    int error_i = 0;
    int argumentsUnknown_i = 0;
    int argumentCount_ai[ARG_MAX] = {0};
    int ppm = 1;
    argument_te argLast_e = ARG_MAX;
    FILE *fin = NULL;
    FILE *fout = NULL;
    char value[ARG_MAX][32];

    for (j = 0; j < ARG_MAX; j++)
    {
        strcpy(value[j], "");
    }

    if (arguments_i == 1)
    {
        error_i++;
        printInfo_v();
    }

    if (!error_i)
    {
//        printf("%d arguments\n", arguments_i);
        for (i = 0; i < arguments_i; i++)
        {
//            printf("#%d %s\n", i, argument[i]);
            if (i > 0)
            {
                if (argLast_e == ARG_MAX)
                {
                    for (j = 0; j < ARG_MAX; j++)
                    {
                        if (strcmp(argument[i], argList[j]) == 0)
                        {
                            argumentCount_ai[j]++;
//                            printf("found valid argument %s %d times\n", argList[j], argumentCount_ai[j]);
                            argLast_e = j;
                            break;
                        }
                    }
                    if (j == ARG_MAX)
                    {
                        argumentsUnknown_i++;
                    }
                }
                else
                {
                    strcpy(value[argLast_e], argument[i]);
//                    printf("value for %s: %s\n", argList[argLast_e], value[argLast_e]);
                    argLast_e = ARG_MAX;
                }
            }
        }

        for (j = 0; j < ARG_MAX; j++)
        {
            if (argumentCount_ai[j] > 1)
            {
                printf("error: same argument used more than once (%s)!\n", argList[j]);
                error_i++;
            }
        }
    }

    if (argumentsUnknown_i > 0)
    {
        printf("error: found %d unknown argument(s)\n", argumentsUnknown_i);
        error_i++;
    }

    if (argumentCount_ai[ARG_INPUT] == 0)
    {
        printf("error: missing required parameter -in!\n");
        error_i++;
    }

    if (strlen(value[ARG_INPUT]) == 0)
    {
        printf("error: missing input filename!\n");
        error_i++;
    }

    if (strlen(value[ARG_PPM]) > 0)
    {
        if (sscanf(value[ARG_PPM], "%d", &ppm) != 1)
        {
            printf("error: invalid numeric value for ppm : \"%s\"!\n", value[ARG_PPM]);
            error_i++;
        }
    }

    if (error_i)
    {
        printInfo_v();
    }

    if (!error_i)
    {
        fin = fopen(value[ARG_INPUT], "r");
        if (!fin)
        {
            printf("error opening input file \"%s\"!\n", value[ARG_INPUT]);
            error_i++;
        }
    }
    if (!error_i)
    {
        char fname[256];

        if (strlen(value[ARG_OUTPUT]) == 0)
        {
            strcpy(fname, value[ARG_INPUT]);
            strcat(fname, "_out.csv");
        }
        else
        {
            strcpy(fname, value[ARG_OUTPUT]);
        }
        fout = fopen(fname, "w");
        if (!fout)
        {
            printf("error opening output file \"%s\"!\n", fname);
            error_i++;
        }
    }

    if (!error_i)
    {
        printf("starting...");

        printf("done\n");
    }

    if (fin)
    {
        fclose(fin);
    }
    if (fout)
    {
        fclose(fout);
    }

    return 0;
}

static void printInfo_v(void)
{
    printf("usage: banking -in <input> [-out <output>] [-ppm <value>]\n");
    printf("\n <input> filename of csv file with banking data\n");
    printf("<output> (optional) filename for csv output file - default is <input>_out.csv\n");
    printf(" <value> (optional) numeric value for ppm of incoming payment - default is 1 (0,000001)\n");
    printf("\nexample:\n");
    printf("banking -in account.csv -out basicIncome.csv -ppm 5\n");
}
