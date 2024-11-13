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

#include "uarray.h"
#include <stdlib.h>

typedef struct UArray_T *T;
typedef struct UArray2_T *UArray2_T;

struct UArray2_T {
        UArray_T theArray;
        int numRows;
        int numCols;
        int size;
};

UArray2_T UArray2_new(int rows, int columns, int elemSize);
int UArray2_width(UArray2_T arr);
int UArray2_height(UArray2_T arr);
int UArray2_size(UArray2_T arr);
void *UArray2_at(UArray2_T arr, int rowIdx, int colIdx);
void UArray2_map_col_major(UArray2_T a, 
                void apply(int i, int j, UArray2_T a, void *elem, void *cl), 
                void *cl);
void UArray2_map_row_major(UArray2_T a, 
                void apply(int i, int j, UArray2_T a, void *elem, void *cl), 
                void *cl);
void UArray2_free(UArray2_T *arr);