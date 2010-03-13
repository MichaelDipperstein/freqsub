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
*   UPDATES
*
*   $Id: sample.c,v 1.2 2008/12/23 07:15:14 michael Exp $
*   $Log: sample.c,v $
*   Revision 1.2  2008/12/23 07:15:14  michael
*   Add support for sparse file headers.
*
*   Revision 1.1.1.1  2008/12/22 15:17:38  michael
*   Initial Release
*
****************************************************************************
*
* sample: An ANSI C Frequency Substitution Encoding/Decoding Library Example
* Copyright (C) 2008 by
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
#include "optlist.h"

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/
typedef enum
{
    ENCODE,
    ENCODE_SPARSE,
    DECODE,
    DECODE_SPARSE
} MODES;

/***************************************************************************
*                                CONSTANTS
***************************************************************************/
#define FALSE   0
#define TRUE    1

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/
char *RemovePath(char *fullPath);
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
    option_t *optList, *thisOpt;
    char *inFile, *outFile;
    MODES mode;

    /* initialize variables */
    inFile = NULL;
    outFile = NULL;
    mode = ENCODE;

    /* parse command line */
    optList = GetOptList(argc, argv, "desi:o:h?");
    thisOpt = optList;

    while (thisOpt != NULL)
    {
        switch(thisOpt->option)
        {
            case 'd':       /* decode mode */
                if ((ENCODE == mode) || (DECODE == mode))
                {
                    mode = DECODE;
                }
                else
                {
                    mode = DECODE_SPARSE;
                }
                break;

            case 'e':       /* encode mode */
                if ((ENCODE == mode) || (DECODE == mode))
                {
                    mode = ENCODE;
                }
                else
                {
                    mode = ENCODE_SPARSE;
                }
                break;

            case 's':       /* sparse mode */
                if (ENCODE == mode)
                {
                    mode = ENCODE_SPARSE;
                }
                else if (DECODE == mode)
                {
                    mode = DECODE_SPARSE;
                }
                break;

            case 'i':       /* input file name */
                if (inFile != NULL)
                {
                    fprintf(stderr, "Multiple input files not allowed.\n");
                    free(inFile);

                    if (outFile != NULL)
                    {
                        free(outFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }
                else if ((inFile =
                    (char *)malloc(strlen(thisOpt->argument) + 1)) == NULL)
                {
                    perror("Memory allocation");

                    if (outFile != NULL)
                    {
                        free(outFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }

                strcpy(inFile, thisOpt->argument);
                break;

            case 'o':       /* output file name */
                if (outFile != NULL)
                {
                    fprintf(stderr, "Multiple output files not allowed.\n");
                    free(outFile);

                    if (inFile != NULL)
                    {
                        free(inFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }
                else if ((outFile =
                    (char *)malloc(strlen(thisOpt->argument) + 1)) == NULL)
                {
                    perror("Memory allocation");

                    if (inFile != NULL)
                    {
                        free(inFile);
                    }

                    FreeOptList(optList);
                    exit(EXIT_FAILURE);
                }

                strcpy(outFile, thisOpt->argument);
                break;

            case 'h':
            case '?':
                ShowUsage(stdout, RemovePath(argv[0]));
                FreeOptList(optList);
                return(EXIT_SUCCESS);
        }

        optList = thisOpt->next;
        free(thisOpt);
        thisOpt = optList;
    }

    /* validate command line */
    if (inFile == NULL)
    {
        fprintf(stderr, "Input file must be provided\n");
        ShowUsage(stderr, RemovePath(argv[0]));
        exit (EXIT_FAILURE);
    }

    /* execute selected function */
    switch (mode)
    {
        case ENCODE:
            status = FreqEncodeFile(inFile, outFile);
            break;

        case DECODE:
            status = FreqDecodeFile(inFile, outFile);
            break;

        case ENCODE_SPARSE:
            status = FreqEncodeSparseFile(inFile, outFile);
            break;

        case DECODE_SPARSE:
            status = FreqDecodeSparseFile(inFile, outFile);
            break;

        default:        /* error case */
            status = 0;
            break;
    }

    /* clean up*/
    free(inFile);
    if (outFile != NULL)
    {
        free(outFile);
    }

    if (status)
    {
        return (EXIT_SUCCESS);
    }
    else
    {
        return (EXIT_FAILURE);
    }
}

/****************************************************************************
*   Function   : RemovePath
*   Description: This is function accepts a pointer to the name of a file
*                along with path information and returns a pointer to the
*                character that is not part of the path.
*   Parameters : fullPath - pointer to an array of characters containing
*                           a file name and possible path modifiers.
*   Effects    : None
*   Returned   : Returns a pointer to the first character after any path
*                information.
****************************************************************************/
char *RemovePath(char *fullPath)
{
    int i;
    char *start, *tmp;                          /* start of file name */
    const char delim[3] = {'\\', '/', ':'};     /* path deliminators */

    start = fullPath;

    /* find the first character after all file path delimiters */
    for (i = 0; i < 3; i++)
    {
        tmp = strrchr(start, delim[i]);

        if (tmp != NULL)
        {
            start = tmp + 1;
        }
    }

    return start;
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
    fprintf(stream, "  -s : Use sparse data file header.\n");
    fprintf(stream, "  -i <filename> : Name of input file.\n");
    fprintf(stream, "  -o <filename> : Name of output file.\n");
    fprintf(stream, "  -h | ?  : Print out command line options.\n\n");
}
