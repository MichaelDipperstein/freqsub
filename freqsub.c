/***************************************************************************
*             Frequency Substitution Encoding/Decoding Functions
*
*   File    : freqsub.c
*   Purpose : Provide functions for Frequency Substitution encoding and
*             decoding of file streams.
*
*             Two forms are provided a normal form and a "sparse" form.
*             The sparse form is intended for files that use less than
*             half of the available symbol set.  It encodes the
*             substitution key as a (symbol offset, code) pair.  The
*             normal from encodes the key as a position based table of the
*             entire symbol set (even the unused symbols).
*
*   Author  : Michael Dipperstein
*   Date    : December 21, 2008
*
****************************************************************************
*
* freqsub: An ANSI C Frequency Substitution Encoding/Decoding Library
*          Example
* Copyright (C) 2008, 2014 by
*   Michael Dipperstein (mdipper@alumni.engr.ucsb.edu)
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
#include <limits.h>

/***************************************************************************
*                            TYPE DEFINITIONS
***************************************************************************/
typedef struct symbol_freq_t
{
    unsigned char symbol;
    unsigned long freq;
} symbol_freq_t;

/***************************************************************************
*                                CONSTANTS
***************************************************************************/

/***************************************************************************
*                                  MACROS
***************************************************************************/

/***************************************************************************
*                            GLOBAL VARIABLES
***************************************************************************/

/***************************************************************************
*                               PROTOTYPES
***************************************************************************/

/***************************************************************************
*                                FUNCTIONS
***************************************************************************/

/***************************************************************************
*   Function   : FreqCompare
*   Description: This function may be used by qsort to order symbol_freq_t
*                type data highest frequency to lowest.  If frequencies
*                match, a comparison of symbols is used to break the tie.
*   Parameters : freq1 - pointer to symbol_freq_t data.
*                freq2 - pointer to symbol_freq_t data.
*   Effects    : None
*   Returned   : < 0 if freq2 < freq1
*                  0 if freq2 == freq1
*                > 0 if freq2 > freq1
***************************************************************************/
int FreqCompare(const void *freq1, const void *freq2)
{
    unsigned int f1, f2;

    f1 = ((symbol_freq_t *)freq1)->freq;
    f2 = ((symbol_freq_t *)freq2)->freq;

    if (f1 < f2)
    {
        return 1;
    }
    else if (f1 > f2)
    {
        return -1;
    }
    else
    {
        return (int)((symbol_freq_t *)freq2)->symbol -
            (int)((symbol_freq_t *)freq1)->symbol;
    }
}

/***************************************************************************
*   Function   : FreqEncodeFile
*   Description: This routine reads an input file and counts character
*                frequencies.  The file is then read again and an output
*                file is created where each input character is encoded with
*                a value associated with its frequency.  The more frequent
*                the symbol, the lower the value.
*   Parameters : inFile - Name of file to encode
*                outFile - Name of file to write encoded output to
*   Effects    : File is encoded using frequency substitution.
*   Returned   : EXIT_SUCCESS for success, otherwise EXIT_FAILURE.
***************************************************************************/
int FreqEncodeFile(char *inFile, char *outFile)
{
    FILE *fpIn;                         /* uncoded input */
    FILE *fpOut;                        /* encoded output */
    symbol_freq_t freqs[UCHAR_MAX + 1]; /* frequency counts */
    unsigned char codes[UCHAR_MAX + 1]; /* frequency based codes */
    int c;

    /* open input and output files */
    if ((fpIn = fopen(inFile, "rb")) == NULL)
    {
        perror(inFile);
        return EXIT_FAILURE;
    }

    if (outFile == NULL)
    {
        fpOut = stdout;
    }
    else
    {
        if ((fpOut = fopen(outFile, "wb")) == NULL)
        {
            fclose(fpIn);
            perror(outFile);
            return EXIT_FAILURE;
        }
    }

    /* initialize frequency counts */
    for (c = 0; c <= UCHAR_MAX; c++)
    {
        freqs[c].symbol = c;
        freqs[c].freq = 0;
    }

    /* count frequencies */
    while((c = fgetc(fpIn)) != EOF)
    {
        freqs[c].freq++;

        if (0 == freqs[c].freq)
        {
            freqs[c].freq = ULONG_MAX;
            fprintf(stderr,
                "Warning: Frequency of %02X too large to count\n", c);
        }
    }

    /* sort freqs by frequency */
    qsort(freqs, UCHAR_MAX + 1, sizeof(symbol_freq_t), FreqCompare);

    /* determine frequency based codes */
    for (c = 0; c <= UCHAR_MAX; c++)
    {
        codes[freqs[c].symbol] = c;
    }

    /* output code table */
    c = 0;

    while (freqs[c].freq > 0)
    {
        fputc(freqs[c].symbol, fpOut);
        c++;
    }

    /* signal end of code table with duplicate symbol */
    if (c > 0)
    {
        fputc(freqs[c - 1].symbol, fpOut);
    }

    /* output encoded file */
    rewind(fpIn);

    while((c = fgetc(fpIn)) != EOF)
    {
        fputc(codes[c], fpOut);
    }

    /* clean up */
    fclose(fpIn);
    fclose(fpOut);
    return EXIT_SUCCESS;
}

/***************************************************************************
*   Function   : FreqDecodeFile
*   Description: This routine reads a frequency substitution encode file
*                writes a decoded version to the specified output file.
*   Parameters : inFile - Name of file to decode
*                outFile - Name of file to write decoded output to
*   Effects    : Frequency substitution encoded file is decoded.
*   Returned   : EXIT_SUCCESS for success, otherwise EXIT_FAILURE.
***************************************************************************/
int FreqDecodeFile(char *inFile, char *outFile)
{
    FILE *fpIn;                         /* encoded input */
    FILE *fpOut;                        /* decoded output */
    unsigned char codes[UCHAR_MAX + 1]; /* frequency based codes */
    unsigned char prev, symbol;
    int c;

    /* open input and output files */
    if ((fpIn = fopen(inFile, "rb")) == NULL)
    {
        perror(inFile);
        return EXIT_FAILURE;
    }

    if (outFile == NULL)
    {
        fpOut = stdout;
    }
    else
    {
        if ((fpOut = fopen(outFile, "wb")) == NULL)
        {
            fclose(fpIn);
            perror(outFile);
            return EXIT_FAILURE;
        }
    }

    /* read in code */
    if ((c = fgetc(fpIn)) != EOF)
    {
        codes[0] = c;
        prev = (unsigned char)c;
    }
    else
    {
        prev = 0;
    }

    symbol = 1;

    while ((c = fgetc(fpIn)) != EOF)
    {
        if (((unsigned char)c) == prev)
        {
            break;
        }

        codes[symbol] = c;
        prev = (unsigned char)c;
        symbol++;
    }

    /* write decoded file */
    while ((c = fgetc(fpIn)) != EOF)
    {
        fputc(codes[c], fpOut);
    }

    /* clean up */
    fclose(fpIn);
    fclose(fpOut);
    return EXIT_SUCCESS;
}
