/*
 *      uarray2.c
 *      by: Armaan Sikka & Nate Pfeffer
 *      utln: asikka01 & npfeff01
 *      date: 10/06/24
 *      assignment: locality
 *
 *      summary: TO DO
 *      
 */

#include "uarray2.h"
#include <except.h>

Except_T Malloc_Fail = { "Malloc Failed" };
Except_T Invalid_P = { "NULL Pointer to Array" };


/********** UArray2_new ********
 *
 *      Creates a new 2-D array
 *
 *      Parameters: 
 *              int rows: number of rows for the array
 *              int columns: number of columns for the array
 *              int elemSize: the size in bytes of the elements being held
 *
 *      Return: 
 *              returns a pointer to the array
 *
 *      Expects:
 *              valid ints passed in the parameters
 *
 *      Notes:
 *              check if memory allocated correctly
 *      
 ******************************/
UArray2_T UArray2_new(int columns, int rows, int elemSize)
{
        T arr = UArray_new((rows * columns), elemSize);
        UArray2_T uarray2 = malloc(sizeof(*uarray2));

        if (uarray2 == NULL) {
                RAISE(Malloc_Fail);
        }

        uarray2->theArray = arr;
        uarray2->numRows = rows;
        uarray2->numCols = columns;
        uarray2->size = elemSize;
        return uarray2;
}

/********** UArray2_width ********
 *
 *      gets the width of the 2-D array
 *
 *      Parameters:
 *              UArray2 arr: the pointer to 2D array
 *
 *      Return: 
 *              the width of array as an int
 *
 *      Expects:
 *              a pointer that is non-null to a proper array
 *
 *      Notes:
 *              if pointer to array is null, exit with checked runtime error
 *      
 ******************************/
int UArray2_width(UArray2_T arr) 
{
        if (arr == NULL) {
                RAISE(Invalid_P);
        }
        return arr->numCols;
}

/********** UArray2_height ********
 *
 *      get the height of the 2-D array
 *
 *      Parameters:
 *              UArray2 arr: the pointer to the array
 *
 *      Return: 
 *              an int of the height of the 2D array
 *
 *      Expects:
 *              a pointer that is non-null to a proper array
 *
 *      Notes:
 *              if pointer to array is null, exit with checked runtime error
 *      
 ******************************/
int UArray2_height(UArray2_T arr)
{
        if (arr == NULL) {
                RAISE(Invalid_P);
        }
        return arr->numRows;
}

/********** UArray2_size ********
 *
 *      get the size of the elements in array
 *
 *      Parameters:
 *              UArray2 arr: the pointer to the array
 *
 *      Return: 
 *              an int of the element size of the 2D array
 *
 *      Expects:
 *              a pointer that is non-null to a proper array
 *
 *      Notes:
 *              if pointer to array is null, exit with checked runtime error
 *      
 ******************************/
int UArray2_size(UArray2_T arr)
{
        if (arr == NULL) {
                RAISE(Invalid_P);
        }
        return arr->size;
}

/********** UArray2_at ********
 *
 *      gets the element at the index given
 *
 *      Parameters:
 *              UArray2 arr: the pointer to the 2D array
 *              int rowIdx: desired row index
 *              int colIdx: desired column index 
 *
 *      Return: 
 *              a pointer to the element at the index given
 *
 *      Expects:
 *              index that is within the bounds the array
 *
 *      Notes:
 *              if pointer to array is null, exit with checked runtime error
 *      
 ******************************/
void *UArray2_at(UArray2_T arr, int colIdx, int rowIdx)
{
        if (arr == NULL) {
                RAISE(Invalid_P);
        }
        return UArray_at(arr->theArray, ((colIdx * arr->numRows) + rowIdx));
}

/********** UArray2_map_col_major ********
 *
 *      maps over the array with columns getting priority, applying the 
 *      apply() func
 *
 *      Parameters:
 *              UArray2 arr: a pointer to the 2D array
 *              apply(int i, int j, UArray2_T a, void *elem, void *cl): 
 *                      the function to be applied to each element
 *                      params: int i: row index
 *                              int j: column index
 *                              UArray2_T a: the 2D array to be mapped
 *                              void *elem: element in the uarray2
 *                              void *cl: closure argument
 *              void *cl: closure argument
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              apply() to be a valid function with no errors that works
 *              on the element type that is stored in the list
 *              
 *      Notes:
 *              if pointer to array is null, exit with checked runtime error
 *      
 ******************************/
void UArray2_map_col_major(UArray2_T a, 
                void apply(int i, int j, UArray2_T a, void *elem, void *cl), 
                void *cl)
{
        if (a == NULL) {
                RAISE(Invalid_P);
        }

        for (int i = 0; i < a->numCols; i++) {
                for (int j = 0; j <  a->numRows; j++) {
                        apply(i, j, a, UArray2_at(a, i, j), cl);
                }
        }
}

/********** UArray2_map_row_major ********
 *
 *      maps over the array with rows getting priority, applying the 
 *      apply() func
 *
 *      Parameters:
 *              UArray2 arr: a pointer to the 2D array
 *              apply(int i, int j, UArray2_T a, void *elem, void *cl): 
 *                      the function to be applied to each element
 *                      params: int i: row index
 *                              int j: column index
 *                              UArray2_T a: the 2D array to be mapped
 *                              void *elem: element in the uarray2
 *                              void *cl: closure argument
 *              void *cl: closure argument
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              apply() to be a valid function with no errors that works
 *              on the element type that is stored in the list
 *              
 *      Notes:
 *              if pointer to array is null, exit with checked runtime error
 *      
 ******************************/
void UArray2_map_row_major(UArray2_T a, 
                void apply(int i, int j, UArray2_T a, void *elem, void *cl), 
                void *cl)
{
        if (a == NULL) {
                RAISE(Invalid_P);
        }

        for (int j = 0; j < a->numRows; j++) {
                for (int i = 0; i <  a->numCols; i++) {
                        apply(i, j, a, UArray2_at(a, i, j), cl);
                }
        }
}

/********** UArray2_free ********
 *
 *      frees each elements stored in the array
 *
 *      Parameters:
 *              UArray2 *arr: a pointer to the array
 *
 *      Return: 
 *              nothing
 *
 *      Expects:
 *              none of the elements to have been freed previously
 *
 *      Notes:
 *              exit with a checked runtime error if arr is null
 *      
 ******************************/
void UArray2_free(UArray2_T *arr)
{
        if (arr == NULL) {
                RAISE(Invalid_P);
        }

        UArray_free(&((*arr)->theArray));
        free(*arr);
}
