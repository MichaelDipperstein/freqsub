/***************************************************************************
*                Frequency Substitution Library Usage Sample
*
*   File    : sample.c
*   Purpose : Demonstrates the usage of Frequency Substitution library
*             encoding and decoding routines.
*   Author  : Michael Dipperstein
*   Date    : December 21, 2008
*
****************************************************************************
*
* sample: An ANSI C Frequency Substitution Encoding/Decoding Library Example
* Copyright (C) 2008, 2014, 2017 by
* Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
*
* This file is part of the Frequency Substitution library.
*
* The Frequency Substitution library is free software; you can redistribute
* it and/or modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either version 3 of
* the License, or (at your option) any later version.
*
* The Frequency Substitution library is distributed in the hope that it will
* be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser
* General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
***************************************************************************/

/***************************************************************************
*                             INCLUDED FILES
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freqsub.h"
#include "optlist/optlist.h"

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/

typedef enum
{
    MODE_UNSPECIFIED,
    MODE_ENCODE,
    MODE_DECODE
} modes_t;


/***************************************************************************
*                                CONSTANTS
***************************************************************************/

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
void ShowUsage(FILE *stream, char *progName);

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/****************************************************************************
*   Function   : Main
*   Description: This is the main function for this program, it validates
*                the command line input and, if valid, it will read the
*                input file and output a frequency substitution encoded or
*                decoded file.
*   Parameters : argc - number of parameters
*                argv - parameter list
*   Effects    : Creates a frequency substitution encoded/decoded version of
*                the specified file.
*   Returned   : EXIT_SUCCESS for success, otherwise EXIT_FAILURE.
****************************************************************************/
int main (int argc, char *argv[])
{
    int status;
    option_t *optList;
    option_t *thisOpt;
    FILE *inFile;
    FILE *outFile;
    modes_t mode;

    /* initialize variables */
    inFile = NULL;
    outFile = NULL;
    mode = MODE_UNSPECIFIED;

    /* parse command line */
    optList = GetOptList(argc, argv, "desi:o:h?");
    thisOpt = optList;

    while (thisOpt != NULL)
    {
        switch(thisOpt->option)
        {
            case 'd':       /* decode mode */
                mode = MODE_DECODE;
                break;

            case 'e':       /* encode mode */
                mode = MODE_ENCODE;
                break;

            case 'i':       /* input file name */
                if (inFile != NULL)
                {
                    fprintf(stderr, "Multiple input files not allowed.\n");
                    fclose(inFile);

                    if (outFile != NULL)
                    {
                        fclose(outFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }
                else if ((inFile = fopen(thisOpt->argument, "rb")) == NULL)
                {
                    perror("Opening Input File");

                    if (outFile != NULL)
                    {
                        fclose(outFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'o':       /* output file name */
                if (outFile != NULL)
                {
                    fprintf(stderr, "Multiple output files not allowed.\n");
                    fclose(outFile);

                    if (inFile != NULL)
                    {
                        fclose(inFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }
                else if ((outFile = fopen(thisOpt->argument, "wb")) == NULL)
                {
                    perror("Opening Output File");

                    if (inFile != NULL)
                    {
                        fclose(inFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'h':
            case '?':
                ShowUsage(stdout, FindFileName(argv[0]));
                FreeOptList(optList);
                return(EXIT_SUCCESS);
        }

        optList = thisOpt->next;
        free(thisOpt);
        thisOpt = optList;
    }

    /* validate command line */
    if ((inFile == NULL) || (outFile == NULL))
    {
        fprintf(stderr, "Input and output files must be provided\n");
        ShowUsage(stderr, FindFileName(argv[0]));
        exit (EXIT_FAILURE);
    }

    /* execute selected function */
    switch (mode)
    {
        case MODE_ENCODE:
            status = FreqEncodeFile(inFile, outFile);
            break;

        case MODE_DECODE:
            status = FreqDecodeFile(inFile, outFile);
            break;

        default:        /* error case */
            fprintf(stderr, "Error: must specify encode or decode.\n");
            ShowUsage(stderr, FindFileName(argv[0]));
            exit (EXIT_FAILURE);
    }

    /* close files.  they are always left open */
    fclose(inFile);
    fclose(outFile);

    if (0 == status)
    {
        return (EXIT_SUCCESS);
    }
    else
    {
        perror("Error");
        return (EXIT_FAILURE);
    }
}

/****************************************************************************
*   Function   : ShowUsage
*   Description: This function sends instructions for using this program to
*                stdout.
*   Parameters : progName - the name of the executable version of this
*                           program.
*                stream - output stream receiving instructions.
*   Effects    : Usage instructions are sent to stream.
*   Returned   : None
****************************************************************************/
void ShowUsage(FILE *stream, char *progName)
{
    fprintf(stream, "Usage: %s <options>\n\n", progName);
    fprintf(stream, "Options:\n");
    fprintf(stream, "  -d : Decode input file to output file.\n");
    fprintf(stream, "  -e : Encode input file to output file.\n");
    fprintf(stream, "  -i <filename> : Name of input file.\n");
    fprintf(stream, "  -o <filename> : Name of output file.\n");
    fprintf(stream, "  -h | ?  : Print out command line options.\n\n");
}
