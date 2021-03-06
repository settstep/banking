/**
 * \file    main.c
 *
 * \date    2020-10-01
 *
 * \author  settstep
 *
 * \section sec_main_c_history history
 *
 * \par     2020-10-04 settstep
 * - added version
 * - output is now 2 digits long
 * - added new parameters -cold and -cola to set the columns for date and amount
 * - removed unused code
 * \par     2020-10-01 settstep
 * - extract date and amount from input file and writing sum for every month in output file
 * - added main function and program argument checking
 * - file created
 */

#include <stdio.h>
#include <string.h>

#define MAX_DATA 128

#define VERSION_MAJOR 1
#define VERSION_MINOR 0
#define VERSION_PATCH 0

/**
 *  \brief list of valid arguments
 */
typedef enum
{
    ARG_INPUT,     ///< input file
    ARG_PPM,       ///< parts per million
    ARG_OUTPUT,    ///< output file
    ARG_COLDATE,   ///< column date
    ARG_COLAMOUNT, ///< column amount
    ARG_VERSION,   ///< version
    ARG_MAX        ///< number of valid arguments
}   argument_te;

typedef struct
{
    int amount;
    int year;
    short month;
}   data_ts;

const char *argList[] = {"-in", "-ppm", "-out", "-cold", "-cola", "-version"};    ///<  list of valid arguments
static data_ts data_as[MAX_DATA];
static int nofData = 0;

static void printInfo_v(void);

static void add_v(int year, short month, int amount)
{
    int i;

    if (amount > 0)
    {
        for (i = 0; i < nofData; i++)
        {
            if ((data_as[i].year == year) && (data_as[i].month == month))
            {
                data_as[i].amount += amount;
                break;
            }
        }
        if (i == nofData)
        {
            if (nofData < MAX_DATA)
            {
                data_as[i].year = year;
                data_as[i].month = month;
                data_as[i].amount = amount;
                nofData++;
            }
        }
    }
}

static void analyse_v(FILE *fin, FILE *fout, int ppm, short columnDate, short columnAmount)
{
    int i;
    int j;
    int k;
    int amount;
    short col;
    short month;
    int year;
    char buffer[1024];
    char str[64];
    char strDate[64];
    char seperator = ';';

    if (fin && fout && ppm)
    {
        while (fgets(buffer, 1024, fin))
        {
            col = 0;
            j = 0;
            k = 0;
            for (i = 0; i < strlen(buffer); i++)
            {
                if (buffer[i] == seperator)
                {
                    col++;
                }
                else if (buffer[i] == '\n')
                {
                    // ignore newline
                }
                else if (col == columnAmount)
                {
                    str[j++] = buffer[i];
                }
                else if (col == columnDate)
                {
                    strDate[k++] = buffer[i];
                }
            }
            str[j++] = '\0';
            strDate[k++] = '\0';

            month = (strDate[3] - 48) * 10 + strDate[4] - 48;
            year = (strDate[6] - 48) * 1000 + (strDate[7] - 48) * 100 + (strDate[8] - 48) * 10 + strDate[9] - 48;

            amount = 0;
            for (i = 0; i < strlen(str); i++)
            {
                switch (str[i])
                {
                    case '0':
                    case '1':
                    case '2':
                    case '3':
                    case '4':
                    case '5':
                    case '6':
                    case '7':
                    case '8':
                    case '9':
                        amount *= 10;
                        amount += str[i] - 48;
                        break;
                }
            }
            if (str[0] == '-')
            {
                amount *= -1;
            }

            add_v(year, month, amount);
        }

        fprintf(fout, "YYYY;MM;EUR\n");
        for (i = 0; i < nofData; i++)
        {
            fprintf(fout, "%04d;%02d;%.2f\n", data_as[i].year, data_as[i].month, ((float)data_as[i].amount * ppm) / 1000000 / 100);
        }
    }
}

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
    int colamount = 10;
    int coldate = 0;
    int ppm = 3000;
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
        for (i = 0; i < arguments_i; i++)
        {
            if (i > 0)
            {
                if (argLast_e == ARG_MAX)
                {
                    for (j = 0; j < ARG_MAX; j++)
                    {
                        if (strcmp(argument[i], argList[j]) == 0)
                        {
                            argumentCount_ai[j]++;
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

    if (argumentCount_ai[ARG_VERSION] == 1)
    {
        printf("banking version %d.%d.%d\n", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
    }
    else
    {
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

        if (strlen(value[ARG_COLDATE]) > 0)
        {
            if (sscanf(value[ARG_COLDATE], "%d", &coldate) != 1)
            {
                printf("error: invalid value for date column : \"%s\"!\n", value[ARG_COLDATE]);
                error_i++;
            }
        }

        if (strlen(value[ARG_COLAMOUNT]) > 0)
        {
            if (sscanf(value[ARG_COLAMOUNT], "%d", &colamount) != 1)
            {
                printf("error: invalid value for amount column : \"%s\"!\n", value[ARG_COLAMOUNT]);
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

            analyse_v(fin, fout, ppm, coldate, colamount);

            printf("done\n");
        }
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
    printf("usage: banking -in <input> [-out <output>] [-ppm <value>] [-cold <coldate>] [-cola <colamount>]\n\n");
    printf("    <input> filename of csv file with banking data\n");
    printf("   <output> (optional) filename for csv output file - default is <input>_out.csv\n");
    printf("    <value> (optional) numeric value for ppm of incoming payment - default is 3000 (0,003)\n");
    printf("  <coldate> (optional) column containing the date of the transaction - default is 0\n");
    printf("<colamount> (optional) column containing the amount - default is 10\n");
    printf("\nexample:\n");
    printf("banking -in account.csv -out basicIncome.csv -ppm 500 -cold 1 -cola 8\n");
}
