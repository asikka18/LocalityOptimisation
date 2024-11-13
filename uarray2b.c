/*
 *      uarray2b.c
 *      by: Armaan Sikka & Nate Pfeffer
 *      utln: asikka01 & npfeff01
 *      date: 10/06/24
 *      assignment: locality
 *
 *      summary: 
 *              this file contains the implementation for uarray2b interface
 *              that stores a 2D array within blocks rather than columns or
 *              rows. This ensures that all elements within a block
 *              will be close together in memory.
 *      
 */

#include "uarray2b.h"
#include <stdlib.h>
#include <stdio.h>
#include "uarray.h"
#include "except.h"
#include <math.h>

Except_T Malloc_Failb = { "Malloc Failed" };
Except_T Invalid_Pb = { "NULL Pointer to Array" };
Except_T Invalid_BS = { "Invalid Block-size Provided" };
Except_T Out_Of_Range = { "Provided Index is Out of Range" };

struct UArray2b_T {
        UArray_T theArray;
        int height;
        int width;
        int size;
        int blocksize;
};

typedef struct UArray2b_T *T;

/********** UArray2b_new ********
 *
 *      creates a new blocked array based on the width, height, size, and
 *      blocksize specified by the parameters and based on the struct defined
 *      above
 *
 *      Parameters:
 *              int width: the width of the image array
 *              int height: the height of the image array
 *              int size: the size of the elements in the array
 *              int blocksize: the size of the blocks
 *
 *      Return: 
 *              a pointer to the blocked array struct
 *
 *      Expects:
 *              valid ints to be passed as parameters
 *
 *      Notes:
 *              exit with a checked runtime error if blocksize is invalid
 *              exit with a checked runtime error if the array struct is NULL
 *      
 ******************************/
T UArray2b_new (int width, int height, int size, int blocksize)
{
        if (blocksize < 1)
                RAISE(Invalid_BS);
        
        int numElems = blocksize * blocksize * 
                        ((width + blocksize - 1) / blocksize) * 
                        ((height + blocksize - 1) / blocksize);
                        
        UArray_T arr = UArray_new(numElems, size);
        T uarray2b = malloc(sizeof(*uarray2b));

        if (uarray2b == NULL)
                RAISE(Malloc_Failb);

        uarray2b->theArray = arr;
        uarray2b->height = height;
        uarray2b->width = width;
        uarray2b->size = size;
        uarray2b->blocksize = blocksize;
        return uarray2b;
}

/********** UArray2b_new_64k_block ********
 *
 *      creates a new blocked array based on the width, height, and size
 *      specified by the parameters and based on the struct defined
 *      above. Allocates as big of a blocksize as possible below 64kb
 *
 *      Parameters:
 *              int width: the width of the image array
 *              int height: the height of the image array
 *              int size: the size of the elements in the array
 *
 *      Return: 
 *              a pointer to the blocked array struct
 *
 *      Expects:
 *              valid ints to be passed as parameters
 *
 *      Notes:
 *              exit with a checked runtime error if blocksize is invalid
 *              exit with a checked runtime error if the array struct is NULL
 *      
 ******************************/
T UArray2b_new_64K_block(int width, int height, int size)
{
        int blocksize = 0; 
        if (size > (64 * 1024)) {
                blocksize = 1;
        } else {
                blocksize = (int)sqrt((64 * 1024) / size);
        }
        
        return UArray2b_new(width, height, size, blocksize);
}

/********** UArray2b_free ********
 *
 *      frees each elements stored in the array and then the struct
 *
 *      Parameters:
 *              T *array2b: a pointer to the array struct
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              none of the elements to have been freed previously
 *
 *      Notes:
 *              exit with a checked runtime error if array2b is NULL
 *      
 ******************************/
void UArray2b_free (T *array2b)
{
        if (array2b == NULL) {
                RAISE(Invalid_Pb);
        }

        UArray_free(&((*array2b)->theArray));
        free(*array2b);
}

/********** UArray2b_width ********
 *
 *      returns the width of the array
 *
 *      Parameters:
 *              T array2b: a pointer to the array
 *
 *      Return: 
 *              width value of the array
 *
 *      Expects:
 *              the width value of the struct to not be NULL
 *
 *      Notes:
 *              exit with a checked runtime error if array2b is NULL
 *      
 ******************************/
int UArray2b_width (T array2b)
{
        if (array2b == NULL)
                RAISE(Invalid_Pb);
        
        return array2b->width;
}

/********** UArray2b_height ********
 *
 *      returns the height of the array
 *
 *      Parameters:
 *              T array2b: a pointer to the array
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              the height value of the struct to not be NULL
 *
 *      Notes:
 *              exit with a checked runtime error if array2b is NULL
 *      
 ******************************/
int UArray2b_height (T array2b)
{
        if (array2b == NULL)
                RAISE(Invalid_Pb);
        
        return array2b->height;
}

/********** UArray2b_size ********
 *
 *      returns the size of the array
 *
 *      Parameters:
 *              T array2b: a pointer to the array
 *
 *      Return: 
 *              int value of the size of each element
 *
 *      Expects:
 *              the size value of the struct to not be NULL
 *
 *      Notes:
 *              exit with a checked runtime error if array2b is NULL
 *      
 ******************************/
int UArray2b_size (T array2b)
{
        if (array2b == NULL)
                RAISE(Invalid_Pb);
        
        return array2b->size;
}

/********** UArray2b_blocksize ********
 *
 *      returns the blocksize of the blocked array
 *
 *      Parameters:
 *              T array2b: a pointer to the array
 *
 *      Return: 
 *              int value of the blocksize
 *
 *      Expects:
 *              the blocksize value of the struct to not be NULL
 *
 *      Notes:
 *              exit with a checked runtime error if array2b is NULL
 *      
 ******************************/
int UArray2b_blocksize(T array2b)
{
        if (array2b == NULL)
                RAISE(Invalid_Pb);
        
        return array2b->blocksize;
}

/********** UArray2b_at ********
 *
 *      finds and returns the element at the specified index
 *
 *      Parameters:
 *              T array2b: the array of the image
 *              int column: the column index
 *              int row: the row index
 *
 *      Return: 
 *              void pointer to the element at the index specified
 *
 *      Expects:
 *              the struct variable to be non-null
 *
 *      Notes:
 *              exits with checked runtime error if the array is NULL or the
 *              indices are out of bounds
 *      
 ******************************/
void *UArray2b_at(T array2b, int column, int row)
{
        if (array2b == NULL)
                RAISE(Invalid_Pb);
        if (column >= array2b->width || row >= array2b->height)
                RAISE(Out_Of_Range);

        int b = array2b->size;
        int height = array2b->height;

        int index = (b * height * (column / b)) + (b * b * (row / b))
                + (column % b) + ((row % b) * b);

        return UArray_at(array2b->theArray, index);
}

/********** UArray2b_map ********
 *
 *      will map over the entire 2D array, applying the 'apply' function
 *      to every element visited and visits every cell in one block before 
 *      moving to another block
 *
 *      Parameters: 
 *              T array2b: the array that is being mapped over
 *              void apply(): apply function with the following parameters
 *                      int col: current column index
 *                      int row: current row index
 *                      T array2b: array currently being mapped over
 *                      void *elem: ppinter to current element
 *                      void *cl: closure being passed
 *              void *cl: a persisting variable throughout the map
 *              
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              a valid apply function to exist
 *
 *      Notes:
 *              CRE if array2b passed is null
 *              
 *      
 ******************************/
void UArray2b_map(T array2b,
                void apply(int col, int row, T array2b, void *elem, void *cl),
                void *cl)
{
        if (array2b == NULL)
                RAISE(Invalid_Pb);

        int bs = array2b->blocksize;
        int rows = array2b->height;
        int cols = array2b->width;

        /* loop through blocks column major */
        for (int bc = 0; bc < (cols + bs - 1) / bs; bc++) {
                for (int br = 0; br < (rows + bs - 1) / bs; br++) {

                        /* loop through elements within the blocks row-major */
                        for (int i = 0; i < bs; i++) {
                                for (int j = 0; j < bs; j++) {

                                int row = br * bs + i;
                                int col = bc * bs + j;

                                if (row < rows && col < cols) {
                                        apply(col, 
                                              row, 
                                              array2b, 
                                              UArray2b_at(array2b, col, row), 
                                              cl);
                                }
                                }
                        }
                }
        }
}
