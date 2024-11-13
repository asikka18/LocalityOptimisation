/*
 *      ppmtrans.c
 *      by: Armaan Sikka & Nate Pfeffer
 *      utln: asikka01 & npfeff01
 *      date: 10/06/24
 *      assignment: locality
 *
 *      summary: 
 *              This program will take in command line arguments that 
 *              specify the desired transformation to be done on the provided
 *              ppm image. It will also output data about the time it took
 *              to complete the transformation to a file. Lastly, the 
 *              transformed image will be output to stdout in raw ppm format.
 *      
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "assert.h"
#include "a2methods.h"
#include "a2plain.h"
#include "a2blocked.h"
#include "pnm.h"
#include "cputiming.h"

#define SET_METHODS(METHODS, MAP, WHAT) do {                    \
        methods = (METHODS);                                    \
        assert(methods);                                \
        map = methods->MAP;                                     \
        if (map == NULL) {                                      \
                fprintf(stderr, "%s does not support "          \
                                WHAT "mapping\n",               \
                                argv[0]);                       \
                exit(1);                                        \
        }                                                       \
} while (false)

static void
usage(const char *progname)
{
        fprintf(stderr, "Usage: %s [-rotate <angle>] "
                        "[-{row,col,block}-major] "
                        "[-time time_file] "
                        "[filename]\n",
                        progname);
        exit(1);
}


/********** open_or_abort ********
 *
 *      tries to open a file and returns the open file stream only if
 *      the file is a .pgm
 *
 *      Parameters:
 *              char *fname: the name of the file to the opened
 *              char *mode: the mode to open the file with
 *
 *      Return: 
 *              the opened file stream pointer
 *
 *      Expects:
 *              a non-null fname pointer and mode pointer
 *              a valid pnm
 *
 *      Notes:
 *              CRE if the file given is not opened properly
 *      
 ******************************/
static FILE *open_or_abort(char *fname, char *mode)
{
        FILE *fp = fopen(fname, mode);
        assert(fp);
        return fp;
}


/********** apply functions ********
 *
 *      The following seven functions are all apply functions used to 
 *      transform an image based on the desired transformation method.
 *
 *      Parameters:
 *              int i: the current column index
 *              int j: the current row index
 *              A2Methods_UArray2 new_a2: the current 2D array being mapped
 *              A2Methods_Object *elem: pointer to current element
 *              void *cl: the closure argument that holds the 
 *                      Pnm_ppm struct containing the image and methods
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              to be called within a 2D array mapping function
 *
 *      Notes:
 *              there are four rotate functions (r0, r90, r180, r270)
 *              there are two flip functions (flip_vert and flip_hori)
 *              there is one transpose function
 *              The only difference between these functions is the index of
 *                      the given image pixel that is set to the current index
 *      
 ******************************/
void r270(int i, int j, A2Methods_UArray2 new_a2, 
                                        A2Methods_Object *elem, void *cl)
{
        (void) new_a2;
        Pnm_ppm pixmap = (Pnm_ppm) cl;
        Pnm_rgb pix    = (Pnm_rgb) elem;
        *pix           = *(Pnm_rgb) pixmap->methods->at(pixmap->pixels, 
                                                (pixmap->width - j - 1), 
                                                 i);
}

void r180(int i, int j, A2Methods_UArray2 new_a2, 
                                        A2Methods_Object *elem, void *cl)
{
        (void) new_a2;
        Pnm_ppm pixmap = (Pnm_ppm) cl;
        Pnm_rgb pix    = (Pnm_rgb) elem;
        *pix           = *(Pnm_rgb) pixmap->methods->at(pixmap->pixels, 
                                                (pixmap->width - i - 1), 
                                                (pixmap->height - j - 1));
}

void r90(int i, int j, A2Methods_UArray2 new_a2, 
                                        A2Methods_Object *elem, void *cl)
{
        (void) new_a2;
        Pnm_ppm pixmap = (Pnm_ppm) cl;
        Pnm_rgb pix    = (Pnm_rgb) elem;
        *pix           = *(Pnm_rgb) pixmap->methods->at(pixmap->pixels, 
                                                j, 
                                                (pixmap->height - i - 1));
}

void r0(int i, int j, A2Methods_UArray2 new_a2, 
                                        A2Methods_Object *elem, void *cl)
{
        (void) new_a2;
        Pnm_ppm pixmap = (Pnm_ppm) cl;
        Pnm_rgb pix    = (Pnm_rgb) elem;
        *pix           = *(Pnm_rgb) pixmap->methods->at(pixmap->pixels, i, j);
}


void flip_vert(int i, int j, A2Methods_UArray2 new_a2, 
                                        A2Methods_Object *elem, void *cl)
{
        (void) new_a2;
        Pnm_ppm pixmap = (Pnm_ppm) cl;
        Pnm_rgb pix    = (Pnm_rgb) elem;
        *pix           = *(Pnm_rgb) pixmap->methods->at(pixmap->pixels, 
                                                (pixmap->width - i - 1),
                                                j);
}

void flip_hori(int i, int j, A2Methods_UArray2 new_a2, 
                                        A2Methods_Object *elem, void *cl)
{
        (void) new_a2;
        Pnm_ppm pixmap = (Pnm_ppm) cl;
        Pnm_rgb pix    = (Pnm_rgb) elem;
        *pix           = *(Pnm_rgb) pixmap->methods->at(pixmap->pixels, 
                                                i,
                                                (pixmap->height - j - 1));
}

void transpose(int i, int j, A2Methods_UArray2 new_a2, 
                                        A2Methods_Object *elem, void *cl)
{
        (void) new_a2;
        Pnm_ppm pixmap = (Pnm_ppm) cl;
        Pnm_rgb pix    = (Pnm_rgb) elem;
        *pix           = *(Pnm_rgb) pixmap->methods->at(pixmap->pixels, j, i);
}


/********** transform ********
 *
 *      handles flipping and transposing the image
 *
 *      Parameters:
 *              char *direction: a string containing direction to flip
 *                              note: NULL for transpose
 *              Pnm_ppm pixmap: the pixmap holding the original image
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              to be called only when not rotating an image
 *
 *      Notes:
 *              frees the 2D array holding the old image in pixmap
 *      
 ******************************/
void transform(char *direction, Pnm_ppm pixmap, A2Methods_mapfun *map)
{
        /* convience: variable to use throughout */
        int w = pixmap->methods->width(pixmap->pixels);
        int h = pixmap->methods->height(pixmap->pixels);
        int s = pixmap->methods->size(pixmap->pixels);

        /* code for transposing */
        if (direction == NULL) {
                A2Methods_UArray2 new_a2 = pixmap->methods->new(h, w, s);

                map(new_a2, transpose, pixmap);

                pixmap->methods->free(&(pixmap->pixels));
                pixmap->pixels = new_a2;
                pixmap->height = w;
                pixmap->width  = h;
                return;
        }

        /* the following is code to flip */
        A2Methods_UArray2 new_a2 = pixmap->methods->new(w, h, s);

        /* check which direction to flip */
        if (strcmp(direction, "vertical"))
                map(new_a2, flip_vert, pixmap);
        else
                map(new_a2, flip_hori, pixmap);

        pixmap->methods->free(&(pixmap->pixels));
        pixmap->pixels = new_a2;
} 


/********** rotate ********
 *
 *      handles rotating the image
 *
 *      Parameters:
 *              int rotation: degree of rotation being done
 *              Pnm_ppm pixmap: the pixmap holding the original image
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              to be called only when rotating an image
 *
 *      Notes:
 *              frees the 2D array holding the old image in pixmap
 *      
 ******************************/
void rotate(int rotation, Pnm_ppm pixmap, A2Methods_mapfun *map)
{
        /* convience: variable to use throughout */
        int w = pixmap->methods->width(pixmap->pixels);
        int h = pixmap->methods->height(pixmap->pixels);
        int s = pixmap->methods->size(pixmap->pixels);

        /* code to rotate where the dimensions of the array don't change */
        if (rotation == 0 || rotation == 180) {
                A2Methods_UArray2 new_a2 = pixmap->methods->new(w, h, s);

                /* determines if rotating 0 or 180 */
                if (rotation == 0)
                        map(new_a2, r0, pixmap);
                else
                        map(new_a2, r180, pixmap);

                pixmap->methods->free(&(pixmap->pixels));
                pixmap->pixels = new_a2;
        } else {
                /* code to rotate where the dimensions of the array flop */
                A2Methods_UArray2 new_a2 = pixmap->methods->new(h, w, s);

                /* determines if rotating 0 or 180 */
                if (rotation == 90)
                        map(new_a2, r90, pixmap);
                else
                        map(new_a2, r270, pixmap);
                
                pixmap->methods->free(&(pixmap->pixels));
                pixmap->pixels = new_a2;
                pixmap->height = w;
                pixmap->width  = h;
        }
} 


/********** time_output ********
 *
 *      handles outputting timing data to our output file by writing the 
 *      total time it took and time per pixel
 *
 *      Parameters:
 *              double time: time in nanoseconds for transformation
 *              char *time_file_name: name of file to write to
 *              int rotation: type of rotation done
 *                              note: -1 if image not rotated
 *              Pnm_ppm pixmap: the pixmap holding the original image
 *              char *direction: direction of flip
 *                              note: NULL if transposed or rotated
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              to only be called when the -time command line argument used
 *
 *      Notes:
 *              exit with a checked runtime error if failed to open file
 *      
 ******************************/
void time_output(double time, char *time_file_name, int rotation, 
                                Pnm_ppm pixmap, char *direction)
{
        /* opens or creates the time output file */
        FILE *time_file = fopen(time_file_name, "a");
        assert(time_file);
        
        /* check what type of transformation needs to occur */
        if (rotation == -1) {
                if (direction == NULL) {
                        fprintf(time_file, 
                        "Time taken to do transpose: %.0f ns.\n", time);
                        double time_per_pix = time / (pixmap->height * 
                                                                pixmap->width);
                        fprintf(time_file, 
                        "Time taken to do transpose per pixel: %.0f ns.\n",
                        time_per_pix);
                } else {
                        fprintf(time_file, 
                        "Time taken to do flip %sly: %.0f ns.\n", 
                                direction, time);
                        double time_per_pix = time / (pixmap->height * 
                                                                pixmap->width);
                        fprintf(time_file, 
                        "Time taken to do flip %sly per pixel: %.0f ns.\n",
                        direction, time_per_pix);
                }
        } else { 
                fprintf(time_file, 
                        "Time taken to do rotate %d: %.0f ns.\n",
                        rotation, time);
                double time_per_pix = time / (pixmap->height * pixmap->width);
                fprintf(time_file, 
                        "Time taken to do rotate %d per pixel: %.0f ns.\n",
                        rotation, time_per_pix);
        }
}


/********** ppmtrans ********
 *
 *      stores the provided image as a Pnm_ppm pixmap
 *      runs the provided transformation while timing it
 *      prints the transformed image to stdout
 *      writes time data to file if applicable
 *
 *      Parameters:
 *              A2Methods_T methods: a method suite to be given to the pixmap
 *              int rotation: degree to rotate the image
 *                              note: -1 if flipping or transposing
 *              char *time_file_name: name of desired file to output time data
 *              FILE *fp: file pointer to the image file provided
 *              char *direction: direction to flip the image
 *                              note: NULL if rotating or transposing
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              a valid ppm file to be provided
 *              the desired methods suite based on command line arguments
 *
 *      Notes:
 *              frees the pixmap and timer at the end of the function
 *      
 ******************************/
void ppmtrans(A2Methods_T methods, int rotation, char *time_file_name, 
                        FILE *fp, char *direction, A2Methods_mapfun *map)
{
        Pnm_ppm pixmap = Pnm_ppmread(fp, methods);
        CPUTime_T timer = CPUTime_New();

        /* times and runs the desired transformation */
        CPUTime_Start(timer);
        if (rotation == -1)
                transform(direction, pixmap, map);
        else
                rotate(rotation, pixmap, map);
        double time = CPUTime_Stop(timer);
        
        /* output transformed image */
        Pnm_ppmwrite(stdout, pixmap);

        /* write to the timing file */
        if (time_file_name != NULL)
                time_output(time, time_file_name, rotation, pixmap, direction);
        
        Pnm_ppmfree(&pixmap);
        CPUTime_Free(&timer);
}


/********** main ********
 *
 *      handles command line arguments 
 *
 *      Parameters:
 *              int argc: the number of arguments given to the command line
 *              char *argv[]: an array of the arguments given to the command
 *                            line
 *
 *      Return: 
 *              exit code
 *
 *      Expects:
 *              nothing
 *
 *      Notes:
 *              added command line handing for transpose, flip and rotating 270
 *              functions
 *      
 ******************************/
int main(int argc, char *argv[])
{
        char *time_file_name = NULL;
        int   rotation       = 0;
        char *direction      = NULL; /* to know which way to flip image */
        int   i;

        /* default to UArray2 methods */
        A2Methods_T methods = uarray2_methods_plain; 
        assert(methods != NULL);

        /* default to best map */
        A2Methods_mapfun *map = methods->map_default; 
        assert(map != NULL);

        for (i = 1; i < argc; i++) {
                if (strcmp(argv[i], "-row-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_row_major, 
                                    "row-major");
                } else if (strcmp(argv[i], "-col-major") == 0) {
                        SET_METHODS(uarray2_methods_plain, map_col_major, 
                                    "column-major");
                } else if (strcmp(argv[i], "-block-major") == 0) {
                        SET_METHODS(uarray2_methods_blocked, map_block_major,
                                    "block-major");
                } else if (strcmp(argv[i], "-rotate") == 0) {
                        if (!(i + 1 < argc)) {      /* no rotate value */
                                usage(argv[0]);
                        }
                        char *endptr;
                        rotation = strtol(argv[++i], &endptr, 10);
                        if (!(rotation == 0 || rotation == 90 ||
                            rotation == 180 || rotation == 270)) {
                                fprintf(stderr, 
                                        "Rotation must be 0, 90 180 or 270\n");
                                usage(argv[0]);
                        }
                        if (!(*endptr == '\0')) {    /* Not a number */
                                usage(argv[0]);
                        }
                        
                /* check for flip command line and horizontal or vertical */
                } else if (strcmp(argv[i], "-flip") == 0) {
                        if (!(i + 1 < argc)) {      /* no flip value */
                                usage(argv[0]);
                        }
                        direction = argv[++i];
                        if (!(strcmp(direction, "horizontal") == 0 || 
                                        strcmp(direction, "vertical") == 0)) {
                                fprintf(stderr, 
                                "Flip must be 'horizontal' or 'vertical'\n");
                                usage(argv[0]);
                        }
                        rotation = -1;
                        
                /* check for transpose command line */
                } else if (strcmp(argv[i], "-transpose") == 0) {
                        rotation = -1;
                } else if (strcmp(argv[i], "-time") == 0) {
                        if (!(i + 1 < argc)) {      /* no time file */
                                usage(argv[0]);
                        }
                        time_file_name = argv[++i];
                } else if (*argv[i] == '-') {
                        fprintf(stderr, "%s: unknown option '%s'\n", argv[0],
                                argv[i]);
                        usage(argv[0]);
                } else if (argc - i > 1) {
                        fprintf(stderr, "Too many arguments\n");
                        usage(argv[0]);
                } else {
                        break;
                }
        }

        if (argc == i) {
                ppmtrans(methods, rotation, time_file_name, stdin, 
                                                        direction, map);
        } else {
                FILE *fp = open_or_abort(argv[i], "rb");
                ppmtrans(methods, rotation, time_file_name, fp, 
                                                        direction, map);
                fclose(fp);
        }

        return 0;
}
