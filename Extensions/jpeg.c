/*
 * tkImgFmtJPEG.c --
 *
 *	A photo image file handler for JPEG files.
 *
 * Author: Andrew Swan (aswan@cs.berkeley.edu)
 *	   Department of Computer Science,
 *	   University of California, Berkeley
 *
 * $Id: jpeg.c 1.1 Sat, 03 Jan 1998 13:46:25 +0100 eg $
 *
 * Parts of this file are based on code under the following
 * copyrights.  Include these copyrights if you do anything
 * with this code
 */

/*
 *
 * Copyright (C) 1994-1996, Thomas G. Lane.
 * This file is part of the Independent JPEG Group's software.
 */

/*
 *
 * Copyright (c) 1995 The Regents of the University of California.
 * All rights reserved.
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose, without fee, and without written agreement is
 * hereby granted, provided that the above copyright notice and the following
 * two paragraphs appear in all copies of this software.
 * 
 * IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE TO ANY PARTY FOR
 * DIRECT, INDIRECT, SPECIAL, INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
 * OF THE USE OF THIS SOFTWARE AND ITS DOCUMENTATION, EVEN IF THE UNIVERSITY OF
 * CALIFORNIA HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 * 
 * THE UNIVERSITY OF CALIFORNIA SPECIFICALLY DISCLAIMS ANY WARRANTIES,
 * INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.  THE SOFTWARE PROVIDED HEREUNDER IS
 * ON AN "AS IS" BASIS, AND THE UNIVERSITY OF CALIFORNIA HAS NO OBLIGATION TO
 * PROVIDE MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
 *

 */

#ifdef USE_TK
#ifdef STk_CODE
#  include <stk.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <tk.h>

/*
 * The format record for the JPEG file format:
 */

static int		FileMatchJPEG _ANSI_ARGS_((FILE *f, char *fileName,
			    char *formatString, int *widthPtr,
			    int *heightPtr));
static int		FileReadJPEG  _ANSI_ARGS_((Tcl_Interp *interp,
			    FILE *f, char *fileName, char *formatString,
			    Tk_PhotoHandle imageHandle, int destX, int destY,
			    int width, int height, int srcX, int srcY));

Tk_PhotoImageFormat tkImgFmtJPEG = {
    "JPEG",			/* name */
    FileMatchJPEG,		/* fileMatchProc */
    NULL,			/* stringMatchProc */
    FileReadJPEG,		/* fileReadProc */
    NULL,			/* stringReadProc */
    NULL,			/* fileWriteProc */
    NULL,			/* stringWriteProc */
};

/*
 * Prototypes for local procedures defined in this file:
 */
static int ReadJPEGFileHeader	_ANSI_ARGS_((FILE *f, int *wPtr,
					     int *hPtr, int *decPtr,
					     unsigned char *lumqt,
					     unsigned char *chqt));
static unsigned short *CreateHufftab	_ANSI_ARGS_((unsigned char *bits,
						     unsigned char *vals));
static void rdct		_ANSI_ARGS_((short block[]));

#define	DECIMATION_411	1
#define	DECIMATION_422	2

/*
 * ----------------------------------------
 *
 * Jpeg_Init
 *
 * Init function to be used with dynamic loading
 * in Tcl 7.5
 *
 * ----------------------------------------
 */
int Jpeg_Init(Tcl_Interp* interp)
{
    Tk_CreatePhotoImageFormat(&tkImgFmtJPEG);
    return TCL_OK;
}

/*
 * Standard Quantization Tables
 */
static unsigned char JPEG_StandardLumQT[] = { 
    16, 11, 10, 16, 24, 40, 51, 61,
    12, 12, 14, 19, 26, 58, 60, 55,
    14, 13, 16, 24, 40, 57, 69, 56,
    14, 17, 22, 29, 51, 87, 80, 62,
    18, 22, 37, 56, 68, 109, 103, 77,
    24, 35, 55, 64, 81, 104, 113, 92,
    49, 64, 78, 87, 103, 121, 120, 101,
    72, 92, 95, 98, 112, 100, 103, 99 };

static unsigned char JPEG_StandardChQT[] = {
    17, 18, 24, 47, 99, 99, 99, 99,
    18, 21, 26, 66, 99, 99, 99, 99,
    24, 26, 56, 99, 99, 99, 99, 99,
    47, 66, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99,
    99, 99, 99, 99, 99, 99, 99, 99 };

/*
 * Standard Huffman Tables
 */
static unsigned char JPEG_LDC_HuffBits[]
    = { 0, 1, 5, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 };
static unsigned char JPEG_LDC_HuffVals[]
    = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

static unsigned char JPEG_CDC_HuffBits[]
    = { 0, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0 };
static unsigned char JPEG_CDC_HuffVals[]
    = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11 };

static unsigned char JPEG_LAC_HuffBits[]
    = { 0, 2, 1, 3, 3, 2, 4, 3, 5, 5, 4, 4, 0, 0, 1, 0x7d };
static unsigned char JPEG_LAC_HuffVals[] = {
    0x01, 0x02, 0x03, 0x00, 0x04, 0x11, 0x05, 0x12,
    0x21, 0x31, 0x41, 0x06, 0x13, 0x51, 0x61, 0x07,
    0x22, 0x71, 0x14, 0x32, 0x81, 0x91, 0xa1, 0x08,
    0x23, 0x42, 0xb1, 0xc1, 0x15, 0x52, 0xd1, 0xf0,
    0x24, 0x33, 0x62, 0x72, 0x82, 0x09, 0x0a, 0x16,
    0x17, 0x18, 0x19, 0x1a, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x34, 0x35, 0x36, 0x37, 0x38, 0x39,
    0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49,
    0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58, 0x59,
    0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68, 0x69,
    0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78, 0x79,
    0x7a, 0x83, 0x84, 0x85, 0x86, 0x87, 0x88, 0x89,
    0x8a, 0x92, 0x93, 0x94, 0x95, 0x96, 0x97, 0x98,
    0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5, 0xa6, 0xa7,
    0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4, 0xb5, 0xb6,
    0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3, 0xc4, 0xc5,
    0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2, 0xd3, 0xd4,
    0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda, 0xe1, 0xe2,
    0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9, 0xea,
    0xf1, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa };

static unsigned char JPEG_CAC_HuffBits[]
    = { 0, 2, 1, 2, 4, 4, 3, 4, 7, 5, 4, 4, 0, 1, 2, 0x77 };
static unsigned char JPEG_CAC_HuffVals[] = {
    0x00, 0x01, 0x02, 0x03, 0x11, 0x04, 0x05, 0x21,
    0x31, 0x06, 0x12, 0x41, 0x51, 0x07, 0x61, 0x71,
    0x13, 0x22, 0x32, 0x81, 0x08, 0x14, 0x42, 0x91,
    0xa1, 0xb1, 0xc1, 0x09, 0x23, 0x33, 0x52, 0xf0,
    0x15, 0x62, 0x72, 0xd1, 0x0a, 0x16, 0x24, 0x34,
    0xe1, 0x25, 0xf1, 0x17, 0x18, 0x19, 0x1a, 0x26,
    0x27, 0x28, 0x29, 0x2a, 0x35, 0x36, 0x37, 0x38,
    0x39, 0x3a, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x49, 0x4a, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x59, 0x5a, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x69, 0x6a, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x79, 0x7a, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87,
    0x88, 0x89, 0x8a, 0x92, 0x93, 0x94, 0x95, 0x96,
    0x97, 0x98, 0x99, 0x9a, 0xa2, 0xa3, 0xa4, 0xa5,
    0xa6, 0xa7, 0xa8, 0xa9, 0xaa, 0xb2, 0xb3, 0xb4,
    0xb5, 0xb6, 0xb7, 0xb8, 0xb9, 0xba, 0xc2, 0xc3,
    0xc4, 0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xd2,
    0xd3, 0xd4, 0xd5, 0xd6, 0xd7, 0xd8, 0xd9, 0xda,
    0xe2, 0xe3, 0xe4, 0xe5, 0xe6, 0xe7, 0xe8, 0xe9,
    0xea, 0xf2, 0xf3, 0xf4, 0xf5, 0xf6, 0xf7, 0xf8,
    0xf9, 0xfa };

/*
 * The following macro decodes a number is the format
 * that jpeg stores it.
 */
#define JPEG_EXTEND(v, t) \
  ((v) < (1 << (t-1) ) ? (v) + (((-1) << t) + 1) : (v))

static const char zz[] = { 0, 1, 8, 16, 9, 2, 3, 10, 17, 24, 32, 25, 18,
			       11, 4, 5, 12, 19, 26, 33, 40, 48, 41, 34,
			       27, 20, 13, 6, 7, 14, 21, 28, 35, 42, 49,
			       56, 57, 50, 43, 36, 29, 22, 15, 23, 30,
			       37, 44, 51, 58, 59, 52, 45, 38, 31, 39,
			       46, 53, 60, 61, 54, 47, 55, 62, 63 };

/*
 * Bounds checking array for after DCT (biased by 128)
 */
static unsigned char JPEG_BoundsFix[] = {
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -128, -128, -128, -128, -128, -128, -128,
    -128, -127, -126, -125, -124, -123, -122, -121,
    -120, -119, -118, -117, -116, -115, -114, -113,
    -112, -111, -110, -109, -108, -107, -106, -105,
    -104, -103, -102, -101, -100, -99, -98, -97, -96, -95, -94, -93,
    -92, -91, -90, -89, -88, -87, -86, -85, -84, -83, -82, -81, -80,
    -79, -78, -77, -76, -75, -74, -73, -72, -71, -70, -69, -68, -67,
    -66, -65, -64, -63, -62, -61, -60, -59, -58, -57, -56, -55, -54,
    -53, -52, -51, -50, -49, -48, -47, -46, -45, -44, -43, -42, -41,
    -40, -39, -38, -37, -36, -35, -34, -33, -32, -31, -30, -29, -28,
    -27, -26, -25, -24, -23, -22, -21, -20, -19, -18, -17, -16, -15,
    -14, -13, -12, -11, -10, -9, -8, -7, -6, -5, -4, -3, -2, -1,
    0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
    17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32,
    33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48,
    49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64,
    65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80,
    81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96,
    97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109,
    110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122,
    123, 124, 125, 126, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127,
    127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127, 127 };

static FILE* bitsFile;
static unsigned int currentBits;
static int bitCount;

#define getbyte(retval)	if (fread(&retval, 1, 1, bitsFile) == 0) return;
    
/* num should be <= 16 */
/* XXX should check fread() return value */
#define fillbits(num)				\
{						\
    unsigned char fillval;			\
    while (bitCount < (num)) {			\
        fread(&fillval, 1, 1, bitsFile);        \
        currentBits <<= 8;			\
        currentBits |= fillval;			\
        bitCount += 8;				\
        if (fillval == 0xff) {			\
	    fread(&fillval, 1, 1, bitsFile);    \
	    if (fillval != 0) {			\
	        currentBits <<= 8;		\
	        currentBits |= fillval;		\
	        bitCount += 8;			\
	    }					\
	}					\
    }						\
}


#define getbits(num, retval)			\
{						\
    fillbits(16);				\
    bitCount -= (num);			      	\
    (retval) = ((currentBits >> bitCount) & ((1<<(num)) - 1));	\
}

#define peekbits(num, retval)			\
{						\
    fillbits(16);				\
    (retval) = ((currentBits >> (bitCount-(num))) & ((1<<(num)) - 1)); \
}

#define flushbits(num)				\
{						\
    bitCount -= (num);				\
}


/*
 *----------------------------------------------------------------------
 *
 * FileMatchJPEG --
 *
 *	This procedure is invoked by the photo image type to see if
 *	a file contains image data in JPEG format.
 *
 * Results:
 *	The return value is >0 if the first characters in file "f" look
 *	like JPEG data, and 0 otherwise.
 *
 * Side effects:
 *	The access position in f may change.
 *
 *----------------------------------------------------------------------
 */

static int
FileMatchJPEG(f, fileName, formatString, widthPtr, heightPtr)
    FILE *f;			/* The image file, open for reading. */
    char *fileName;		/* The name of the image file. */
    char *formatString;		/* User-specified format string, or NULL. */
    int *widthPtr, *heightPtr;	/* The dimensions of the image are
				 * returned here if the file is a valid
				 * JPEG file. */
{
    unsigned char lumqt[64], chqt[64];
    int decimation;
    
    return ReadJPEGFileHeader(f, widthPtr, heightPtr, &decimation,
			      lumqt, chqt);
}

/*
 *----------------------------------------------------------------------
 *
 * FileReadJPEG --
 *
 *	This procedure is called by the photo image type to read
 *	JPEG format data from a file and write it into a given
 *	photo image.
 *
 * Results:
 *	A standard TCL completion code.  If TCL_ERROR is returned
 *	then an error message is left in interp->result.
 *
 * Side effects:
 *	The access position in file f is changed, and new data is
 *	added to the image given by imageHandle.
 *
 *----------------------------------------------------------------------
 */

static int
FileReadJPEG(interp, f, fileName, formatString, imageHandle, destX, destY,
	width, height, srcX, srcY)
    Tcl_Interp *interp;		/* Interpreter to use for reporting errors. */
    FILE *f;			/* The image file, open for reading. */
    char *fileName;		/* The name of the image file. */
    char *formatString;		/* User-specified format string, or NULL. */
    Tk_PhotoHandle imageHandle;	/* The photo image to write into. */
    int destX, destY;		/* Coordinates of top-left pixel in
				 * photo image to be written to. */
    int width, height;		/* Dimensions of block of photo image to
				 * be written to. */
    int srcX, srcY;		/* Coordinates of top-left pixel to be used
				 * in image being read. */
{
    int w, h, wFull, hFull, decimation;	/* width, height, decimation */
    unsigned char lumqt[64], chqt[64];	/* quantization tables */
    unsigned short *ldht, *laht, *cdht, *caht;	/* huffman tables */
    short yPredict, crPredict, cbPredict;
    Tk_PhotoImageBlock tkblock;
    int mcu, maxmcu;
    unsigned char *final;

    w = h = -1;
    memcpy(lumqt, JPEG_StandardLumQT, 64);
    memcpy(chqt, JPEG_StandardChQT, 64);
    ldht = CreateHufftab(JPEG_LDC_HuffBits, JPEG_LDC_HuffVals);
    laht = CreateHufftab(JPEG_LAC_HuffBits, JPEG_LAC_HuffVals);
    cdht = CreateHufftab(JPEG_CDC_HuffBits, JPEG_CDC_HuffVals);
    caht = CreateHufftab(JPEG_CAC_HuffBits, JPEG_CAC_HuffVals);

    if (ReadJPEGFileHeader(f, &w, &h, &decimation, lumqt, chqt) == 0) {
	Tcl_AppendResult(interp, "couldn't read file \"", fileName,
		"\" as a JPEG image.", NULL);
	return TCL_ERROR;
    }

    if ((srcX + width) > w) {
	width = w - srcX;
    }
    if ((srcY + height) > h) {
	height = h - srcY;
    }
    if ((width <= 0) || (height <= 0)
	|| (srcX >= w) || (srcY >= h)) {
	return TCL_OK;
    }

    wFull = w;
    if (w%16 != 0) {
	wFull += (16 - (w%16));
    }

    hFull = h;
    if (decimation == DECIMATION_422) {
	if (h%8 != 0) {
	    hFull += (8 - (h%8));
	}
	maxmcu = (wFull/16) * (hFull/8);
    } else if (decimation == DECIMATION_411) {
	if (h%16 != 0) {
	    hFull += (16 - (h%16));
	}
	maxmcu = (wFull/16) * (hFull/16);
    }

    final = (unsigned char *) ckalloc(4 * wFull * hFull);
    if (final == NULL) {
	return TCL_ERROR;
    }

    Tk_PhotoBlank(imageHandle);
    Tk_PhotoExpand(imageHandle, destX + width, destY + height);

    tkblock.pixelPtr = final;
    tkblock.width = w;
    tkblock.height = h;
    tkblock.pitch = 4 * wFull;
    tkblock.pixelSize = 4;
    tkblock.offset[0] = 0;
    tkblock.offset[1] = 1;
    tkblock.offset[2] = 2;

    yPredict = 0;
    crPredict = cbPredict = 0;
    
    /*
     * Main decode loop
     */
    for (mcu = 0; mcu < maxmcu; mcu++) {
	short block[64];
	unsigned short *dcht;
	unsigned short *acht;
	unsigned char *qt;
	short *predictor;
	int stride;

	int piece, pieces;
	int code, value;
	unsigned char difflen;
	short diff;
	int k;
	unsigned char mixed, run, coefflen, coeff;
	unsigned char *dest;
	int mcux, mcuy;
	signed char y[16*16], cr[8*8], cb[8*8];
	
	int row, col, rows, cols;
	signed char *outPtr;
	short *inPtr;
	unsigned char *BoundsFix = JPEG_BoundsFix + 128 + 128;

	mcux = 16 * (mcu % (wFull / 16));
	if (decimation == DECIMATION_422) {
	    mcuy = 8 * (mcu / (wFull / 16));
	    pieces = 4;
	} else if (decimation == DECIMATION_411) {
	    mcuy = 16 * (mcu / (wFull / 16));
	    pieces = 6;
	}
	
	for (piece = 0; piece < pieces; piece++) {
	    /*
	     * Zero the piece we are about to decode
	     */
	    memset(block, 0, sizeof(block));

	    /*
	     * Calculate everything specific to which plane we
	     * are decoding to avoid conditionals in the main loop
	     */

	    if (decimation == DECIMATION_411) {
		if (piece < 2) {
		    dcht = ldht;
		    acht = laht;
		    qt = lumqt;
		    predictor = &yPredict;

		    dest = y + (8 * piece);
		    stride = 16;
		} else if (piece < 4) {
		    dcht = ldht;
		    acht = laht;
		    qt = lumqt;
		    predictor = &yPredict;

		    dest = y + (16*8) + (8 * (piece-2));
		    stride = 16;
		} else if (piece == 4) {
		    dcht = cdht;
		    acht = caht;
		    qt = chqt;
		    predictor = &cbPredict;
		    
		    dest = cb;
		    stride = 8;
		} else {
		    dcht = cdht;
		    acht = caht;
		    qt = chqt;
		    predictor = &crPredict;
		    
		    dest = cr;
		    stride = 8;
		}
	    } else if (decimation == DECIMATION_422) {
		if (piece < 2) {
		    dcht = ldht;
		    acht = laht;
		    qt = lumqt;
		    predictor = &yPredict;

		    dest = y + (8 * piece);
		    stride = 16;
		} else if (piece == 2) {
		    dcht = cdht;
		    acht = caht;
		    qt = chqt;
		    predictor = &cbPredict;
		    
		    dest = cb;
		    stride = 8;
		} else {
		    dcht = cdht;
		    acht = caht;
		    qt = chqt;
		    predictor = &crPredict;
		    
		    dest = cr;
		    stride = 8;
		}
	    }

	    /*
	     * Get the DC value
	     */
	    peekbits(16, code);
	    value = dcht[code];
	    flushbits(value >> 8);
	    difflen = value & 0xff;
	    if (difflen) {
		getbits(difflen, diff);
		diff = JPEG_EXTEND(diff, difflen);
		*predictor += diff;
	    }
	    block[0] = (*predictor) * qt[0];

	    /*
	     * Get the AC values
	     */
	    for (k=1; k<64; ) {
		peekbits(16, code);
		value = acht[code];
		flushbits(value >> 8);
		mixed = value & 0xff;
		run = mixed >> 4;
		coefflen = mixed & 0xf;
		if (coefflen == 0) {
		    if (run == 15) {
			k += 16;
		    } else {
			k = 64;
		    }
		} else {
		    k += run;
		    getbits(coefflen, coeff);
		    block[zz[k]] = (JPEG_EXTEND(coeff, coefflen)) * qt[zz[k]];
		    k++;
		}
	    }

	    /*
	     * Do Reverse DCT and put the data where it belongs
	     */
	    rdct(block);

	    inPtr = block;
	    outPtr = dest;
	    for (row=0; row<8; row++) {
		outPtr[0] = BoundsFix[inPtr[0]];
		outPtr[1] = BoundsFix[inPtr[1]];
		outPtr[2] = BoundsFix[inPtr[2]];
		outPtr[3] = BoundsFix[inPtr[3]];
		outPtr[4] = BoundsFix[inPtr[4]];
		outPtr[5] = BoundsFix[inPtr[5]];
		outPtr[6] = BoundsFix[inPtr[6]];
		outPtr[7] = BoundsFix[inPtr[7]];
		outPtr += stride;
		inPtr += 8;
	    }
	}

	if (decimation == DECIMATION_422) {
	    rows = 8;
	    cols = 16;
	} else if (decimation == DECIMATION_411) {
	    rows = 16;
	    cols = 16;
	}
	/* XXX can this be made more efficient? */
	for (row=0; row<rows; row++) {
	    for (col=0; col<cols; col++) {
		int index = 4 * (wFull*(row+mcuy) + (col+mcux));
		int yval, crval, cbval, val;

		yval = ((int) y[16*row + col]) + (128-16);
		crval = cr[8*(row/2) + col/2];
		cbval = cb[8*(row/2) + col/2];
		    
		val = (1000*yval + 1596*crval)/1000;
		final[index] = (val<0) ? 0 : (val>255) ? 255 : val;

		val = (1000*yval - 813*crval - 391*cbval)/1000;
		final[index + 1] = (val<0) ? 0 : (val>255) ? 255 : val;

		val = (1000*yval + 2018*cbval)/1000;
		final[index + 2] = (val<0) ? 0 : (val>255) ? 255 : val;

		final[index + 3] = 0;
	    }
	}
    }

    ckfree(ldht);
    ckfree(laht);
    ckfree(cdht);
    ckfree(caht);

    Tk_PhotoPutBlock(imageHandle, &tkblock, 0, 0, w, h);

    return TCL_OK;
}


/*
 *----------------------------------------------------------------------
 *
 * ReadJPEGFileHeader --
 *
 *	This procedure reads the JPEG header from the beginning of a
 *	JPEG file and returns the dimensions of the image.
 *
 * Results:
 *	The return value is 1 if a valid JFIF header is found in
 *	the file, 0 otherwise.  If a valid JFIF header is found,
 *	then *widthPtr and *heightPtr are set to hold the dimensions
 *	of the image.
 *
 * Side effects:
 *	The access position in f advances.
 *
 *----------------------------------------------------------------------
 */

static int
ReadJPEGFileHeader(f, wPtr, hPtr, decPtr, lumqt, chqt)
    FILE *f;		/* Image file to read the header from */
    int *wPtr;		/* Pointer to int for storing width */
    int *hPtr;		/* Pointer to int for storing height */
    int *decPtr;    	/* Pointer to int for storing decimation */
    unsigned char *lumqt;	/* Luminance Quantization table */
    unsigned char *chqt;	/* Chrominance Quantization table */
{
    int marker, w, h, lumqtid, chqtid, decimation;
    unsigned char* qtables[256];
    int i;

    memset(qtables, 0, sizeof(qtables) );
    
    bitsFile = f;
    currentBits = 0;
    bitCount = 0;
    fillbits(16);

    w = h = decimation = -1;
    lumqtid = chqtid = -1;
    
    peekbits(8, marker);
    while (marker == 0xff) {
	flushbits(8);
	getbits(8, marker);
	
	switch (marker) {
	  case 0xc0:
	    {
		/* SOF: Start of Frame */
		int value, i;
		
		/* Flush length */
		flushbits(16);
		
		/* Check sample precision */
		getbits(8, value);
		if (value != 8) {
		    return 0;
		}
		
		/* Get w and h */
		getbits(16, h);
		getbits(16, w);
		
		/* Check the components */
		getbits(8, value);
		if (value != 3) {
		    return 0;
		}
		for (i=0; i<3; i++) {
		    /* Ignore ID number */
		    flushbits(8);
		    
		    /* Check decimation */
		    getbits(8, value);
		    if (i == 0) {
			if (value == 0x21) {
			    decimation = DECIMATION_422;
			} else if (value == 0x22) {
			    decimation = DECIMATION_411;
			} else {
			    return 0;
			}
			getbits(8, lumqtid);
		    } else {
			if (value != 0x11) {
			    return 0;
			}
			getbits(8, chqtid);
		    }
		}
	    }
	    break;
	    
	  case 0xdb:
	    {
		/* DQT: Define Quantization Table */
		int length, id, tab, i;
		unsigned char *qt;
		
		/* Find the length */
		getbits(16, length);
		if ((length-2) % 65 != 0) {
		    return 0;
		}
		length = (length - 2) / 65;
		
		for (tab = 0; tab < length; tab++) {
		    getbits(8, id);
		    if (qtables[id] == NULL) {
			qtables[id] = ckalloc(64 * sizeof(unsigned char));
		    }

		    qt = qtables[id];

		    for (i = 0; i < 64; i++) {
			getbits(8, qt[zz[i]]);
		    }
		}
	    }
	    break;
	    
	  case 0xc4:	/* DHT: Define Huffman Table */
	  case 0xdd:	/* DRI: Define Reset Interval */
	  case 0xda:	/* SOS: Start of Scan */
	    /* Ignore these codes */
	    /* XXX add support for DHT */
	    peekbits(16, marker);
	    flushbits(8 * marker);
	    break;
	    
	  case 0xd8:	/* SOI: Start of Image */
	  case 0xd9:	/* EOI: End of Image */
	    /* Ignore these codes */
	    break;
	    
	  default:
	    /* Unhandled code -- ignore it */
	    peekbits(16, marker);
	    flushbits(8 * marker);
	    break;
	}
	
	peekbits(8, marker);
    }
    
    if ((w == -1) || (h == -1) || (decimation == -1)) {
	return 0;
    }
    
    *wPtr = w;
    *hPtr = h;
    *decPtr = decimation;

    if (lumqtid != -1 && qtables[lumqtid] != NULL)
	memcpy(lumqt, qtables[lumqtid], 64 * sizeof(unsigned char));
    if (chqtid != -1 && qtables[chqtid] != NULL)
	memcpy(chqt, qtables[chqtid], 64 * sizeof(unsigned char));

    for (i=0; i<256; i++)
	if (qtables[i] != NULL)
	    ckfree(qtables[i]);

    return 1;
}


/*
 *--------------------------------------------------------------
 *
 * CreateHufftab
 *
 *	Convert a list of bits and values for a Huffman
 *	table in to a table that can be used in decoding.
 *
 * Results:
 *	The table itself is returned.  It should be freed
 *	with FreeHufftab
 *
 * Side effects:
 *	Memory is allocated for the table
 *
 *--------------------------------------------------------------
 */
static unsigned short *
CreateHufftab(unsigned char *bits, unsigned char *vals)
{
    int symbols, sizes[256], codes[256], code, i, j;
    unsigned short *table;

    /*
     * Allocate some memory
     */
    table = (unsigned short *) ckalloc(65536 * sizeof(short));
    if (table == NULL) {
	return NULL;
    }
    
    memset(table, 0, 65536 * sizeof(short) );

    /* 
     * Count the total number of symbols
     * and generate the codes
     */
    symbols = 0;
    code = 0;
    for (i=0; i<16; i++) {
	for (j=0; j<(int)(bits[i]); j++) {
	    codes[symbols] = code++;
	    sizes[symbols] = i + 1;
	    symbols++;
	}
	code <<= 1;
    }
    
    /*
     * Generate the values in the final table
     */
    for (i=0; i<symbols; i++) {
	int val, index, bits;
	
	val = (sizes[i] << 8) | vals[i];
	bits = 16 - sizes[i];
	index = codes[i] << bits;
	
	for (j=0; j< 1<<bits; j++) {
	    table[index | j] = val ;
	}
    }

    return table;
}


/*
 *--------------------------------------------------------------
 *
 * rdct --
 *
 *  The inverse DCT function.
 *
 * Results:
 *	None.
 *
 * Side effects:
 *	None.
 *
 *--------------------------------------------------------------
 */
static void
rdct(short *data)
{
    int tmp0, tmp1, tmp2, tmp3;
    int tmp10, tmp11, tmp12, tmp13;
    int z1, z2, z3, z4, z5;
    int d0, d1, d2, d3, d4, d5, d6, d7;
    register short *dataptr;
    int row;
   
#define FIX(x) ((x) * (1<<13) + 0.5)

    /* Pass 1: process rows. */
    /* Note results are scaled up by sqrt(8) compared to a true IDCT; */
    /* furthermore, we scale the results by 2**PASS1_BITS. */

    dataptr = data;

    for (row = 7; row >= 0; row--) {
	d0 = dataptr[0];
	d1 = dataptr[1];
	d2 = dataptr[2];
	d3 = dataptr[3];
	d4 = dataptr[4];
	d5 = dataptr[5];
	d6 = dataptr[6];
	d7 = dataptr[7];

	/* Even part */
	z1 = (d2 + d6) * FIX(0.541196100);
	tmp2 = z1 - (d6 * FIX(1.847759065));
	tmp3 = z1 + (d2 * FIX(0.765366865));

	tmp0 = (d0 + d4) << 13;
	tmp1 = (d0 - d4) << 13;

	tmp10 = tmp0 + tmp3;
	tmp13 = tmp0 - tmp3;
	tmp11 = tmp1 + tmp2;
	tmp12 = tmp1 - tmp2;

	/* Odd part */

	z1 = d7 + d1;
	z2 = d5 + d3;
	z3 = d7 + d3;
	z4 = d5 + d1;
	z5 = (z3 + z4) * FIX(1.175875602);
		    
	tmp0 = d7 * FIX(0.298631336); 
	tmp1 = d5 * FIX(2.053119869);
	tmp2 = d3 * FIX(3.072711026);
	tmp3 = d1 * FIX(1.501321110);
	z1 = -z1 * FIX(0.899976223);
	z2 = -z2 * FIX(2.562915447);
	z3 = -z3 * FIX(1.961570560);
	z4 = -z4 * FIX(0.390180644);
		    
	z3 += z5;
	z4 += z5;
		    
	tmp0 += z1 + z3;
	tmp1 += z2 + z4;
	tmp2 += z2 + z3;
	tmp3 += z1 + z4;

	/* Final output stage: inputs are tmp10..tmp13, tmp0..tmp3 */

	dataptr[0] = (short) ((tmp10 + tmp3 + (1 << 10)) >> 11);
	dataptr[7] = (short) ((tmp10 - tmp3 + (1 << 10)) >> 11);
	dataptr[1] = (short) ((tmp11 + tmp2 + (1 << 10)) >> 11);
	dataptr[6] = (short) ((tmp11 - tmp2 + (1 << 10)) >> 11);
	dataptr[2] = (short) ((tmp12 + tmp1 + (1 << 10)) >> 11);
	dataptr[5] = (short) ((tmp12 - tmp1 + (1 << 10)) >> 11);
	dataptr[3] = (short) ((tmp13 + tmp0 + (1 << 10)) >> 11);
	dataptr[4] = (short) ((tmp13 - tmp0 + (1 << 10)) >> 11);

	dataptr += 8;		/* advance pointer to next row */
    }

    /* Pass 2: process columns. */
    /* Note that we must descale the results by a factor of 8 == 2**3, */
    /* and also undo the PASS1_BITS scaling. */

    dataptr = data;
    for (row = 8-1; row >= 0; row--) {
	d0 = dataptr[8*0];
	d1 = dataptr[8*1];
	d2 = dataptr[8*2];
	d3 = dataptr[8*3];
	d4 = dataptr[8*4];
	d5 = dataptr[8*5];
	d6 = dataptr[8*6];
	d7 = dataptr[8*7];

	/* Even part */
	z1 = (d2 + d6) * FIX(0.541196100);
	tmp2 = z1 - (d6 * FIX(1.847759065));
	tmp3 = z1 + (d2 * FIX(0.765366865));

	tmp0 = (d0 + d4) << 13;
	tmp1 = (d0 - d4) << 13;

	tmp10 = tmp0 + tmp3;
	tmp13 = tmp0 - tmp3;
	tmp11 = tmp1 + tmp2;
	tmp12 = tmp1 - tmp2;

	/* Odd part */

	z1 = d7 + d1;
	z2 = d5 + d3;
	z3 = d7 + d3;
	z4 = d5 + d1;
	z5 = (z3 + z4) * FIX(1.175875602);
		    
	tmp0 = d7 * FIX(0.298631336); 
	tmp1 = d5 * FIX(2.053119869);
	tmp2 = d3 * FIX(3.072711026);
	tmp3 = d1 * FIX(1.501321110);
	z1 = -z1 * FIX(0.899976223);
	z2 = -z2 * FIX(2.562915447);
	z3 = -z3 * FIX(1.961570560);
	z4 = -z4 * FIX(0.390180644);
		    
	z3 += z5;
	z4 += z5;
		    
	tmp0 += z1 + z3;
	tmp1 += z2 + z4;
	tmp2 += z2 + z3;
	tmp3 += z1 + z4;

	dataptr[8*0] = (short) ((tmp10 + tmp3 + (1 << 17)) >> 18);
	dataptr[8*7] = (short) ((tmp10 - tmp3 + (1 << 17)) >> 18);
	dataptr[8*1] = (short) ((tmp11 + tmp2 + (1 << 17)) >> 18);
	dataptr[8*6] = (short) ((tmp11 - tmp2 + (1 << 17)) >> 18);
	dataptr[8*2] = (short) ((tmp12 + tmp1 + (1 << 17)) >> 18);
	dataptr[8*5] = (short) ((tmp12 - tmp1 + (1 << 17)) >> 18);
	dataptr[8*3] = (short) ((tmp13 + tmp0 + (1 << 17)) >> 18);
	dataptr[8*4] = (short) ((tmp13 - tmp0 + (1 << 17)) >> 18);

	dataptr++;			/* advance pointer to next column */
    }
}

#ifdef STk_CODE
PRIMITIVE STk_init_jpeg(void)
{
  extern Tcl_Interp *STk_main_interp;

  Jpeg_Init(STk_main_interp);
  return UNDEFINED;
}
#endif


#else
  /* Some compilers hate to produce an empty object file. */
  static char dumb = '?';
#endif
