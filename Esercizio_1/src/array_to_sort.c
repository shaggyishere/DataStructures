#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "array_to_sort.h"
#include <unistd.h>

/* Initial capacity for the array */
#define INITIAL_CAPACITY 1024

/* Value of K */
#define KEY_VALUE 4
unsigned long binary_search(UnsortedArray *, void *, long, long);
void merge(UnsortedArray *, unsigned long, unsigned long, unsigned long);
void array_to_sort_merge_sort_modified(UnsortedArray *, unsigned long, unsigned long);
void array_to_sort_binary_insertion_sort(UnsortedArray *, unsigned long, unsigned long);

/* Index of new element that will be inserted */
static unsigned long indexNewElem = 0;

/* It represents the internal structure of this implementation of ordered arrays */
typedef struct _UnsortedArray{
    void **array;
    unsigned long size;
    unsigned long array_capacity;
    int (*compare)(void *, void *);
} UnsortedArray;

UnsortedArray *array_to_sort_create(int (*compare)(void *, void *)){
    if (compare == NULL) {
        fprintf(stderr, "array_to_sort_create: compare parameter cannot be NULL");
        return NULL;
    }
    UnsortedArray *unsortedArray = (UnsortedArray*)malloc(sizeof(UnsortedArray));
    if (unsortedArray == NULL) {
        fprintf(stderr, "array_to_sort_create: unable to allocate memory for the unordere array");
        return NULL;
    }
    unsortedArray->array = (void**)malloc(INITIAL_CAPACITY * sizeof(void*));
    if (unsortedArray->array == NULL) {
        fprintf(stderr, "array_to_sort_create: unable to allocate memory for the internal array");
        return NULL;
    }
    unsortedArray->size = 0;
    unsortedArray->array_capacity = INITIAL_CAPACITY;
    unsortedArray->compare = compare;
    return unsortedArray;
}

int array_to_sort_is_empty(UnsortedArray *unsortedArray) {
    if (unsortedArray == NULL) {
        fprintf(stderr, "unsorted_array_is_empty: unsorted_array parameter cannot be NULL");
        return -1;
    }
    return unsortedArray->size == 0;
}

int array_to_sort_add(UnsortedArray *unsortedArray, void *element){
    if (unsortedArray == NULL){
        fprintf(stderr, "add_unsortedarrayElement: unsortedArray parameter cannot be NULL");
        return -1;
    }
    if (element == NULL){
        fprintf(stderr, "add_unsortedarrayElement: element parameter cannot be NULL");
        return -1;
    }

    if (unsortedArray->size >= unsortedArray->array_capacity) {
        unsortedArray->array_capacity = 2 * unsortedArray->array_capacity;
        unsortedArray->array = (void **)realloc(unsortedArray->array, unsortedArray->array_capacity * sizeof(void *));
        if (unsortedArray->array == NULL) {
            fprintf(stderr, "array_to_sort_add: unable to reallocate memory to host the new element");
            return -1;
        }
    }
    unsortedArray->array[indexNewElem] = element;
    indexNewElem++;
    unsortedArray->size++;
    return 0;
}

unsigned long array_to_sort_size(UnsortedArray *unsortedArray){
    if (unsortedArray == NULL) {
        fprintf(stderr, "_size: ordered_array parameter cannot be NULL");
        return 0;
    }
    return unsortedArray->size;
}

void *array_to_sort_get(UnsortedArray *unsortedArray, unsigned long index){
    if (unsortedArray == NULL) {
        fprintf(stderr, "array_to_sort_get: unsortedArray parameter cannot be NULL");
        return NULL;
    }
    if (index >= unsortedArray->size) {
        fprintf(stderr, "array_to_sort_get: Index %lu is out of the array bounds", index);
        return NULL;
    }
    return unsortedArray->array[index];
}

int array_to_sort_free_memory(UnsortedArray *unsortedArray){
    if (unsortedArray == NULL){
        fprintf(stderr, "unsortedArray_free_memory: unsortedArray parameter cannot be NULL");
        return -1;
    }
    indexNewElem = 0;
    free(unsortedArray->array);
    free(unsortedArray);
    return 0;
}

/* ------------------ SORT FUNCTIONS ------------------ */

/*
 * It returns the correct position where the element should be insert
 */
unsigned long binary_search(UnsortedArray *unsortedArray, void *selectedElem, long firstPosition, long lastPosition){
    if (lastPosition < firstPosition)
        return (unsigned) firstPosition;
    else {
        long middlePosition = (firstPosition + lastPosition) / 2;
        if (unsortedArray->compare(selectedElem, unsortedArray->array[middlePosition]) == -1)      
            return binary_search(unsortedArray, selectedElem, firstPosition, middlePosition - 1);   
        else                                                                                        
            return binary_search(unsortedArray, selectedElem, middlePosition + 1, lastPosition);    
    }
}

/*
 * It ordered the array with binary_insertion_sort
 */
void array_to_sort_binary_insertion_sort(UnsortedArray *unsortedArray, unsigned long firstPosition, unsigned long lastPosition){
    unsigned long positionElem, tempPos, finalPositionElem;
    void *selectedElem;

    for (positionElem = firstPosition + 1; positionElem <= lastPosition; positionElem++){
        tempPos = positionElem;
        selectedElem = unsortedArray->array[positionElem];

        finalPositionElem = binary_search(unsortedArray, selectedElem, (signed) firstPosition, (signed) positionElem - 1);

        while (tempPos > finalPositionElem) {
            unsortedArray->array[tempPos] = unsortedArray->array[tempPos-1];
            tempPos--;
        }
        unsortedArray->array[tempPos] = selectedElem;
    }
    return;
}

int array_to_sort_merge_binary_insertion_sort(UnsortedArray *unsortedArray, unsigned long firstPosition, unsigned long lastPosition){
    if(unsortedArray == NULL){
        fprintf(stderr, "Unsorted array parameter cannot be NULL\n");
        return -1;
    }
    if ((firstPosition > lastPosition) || (lastPosition >= unsortedArray->size)) {
        fprintf(stderr, "Invalid parameter\n");
        return -1;
    }
    array_to_sort_merge_sort_modified(unsortedArray, firstPosition, lastPosition);
    return 0;
}

/*
 * It is actually the merge_sort implementation
 */
void array_to_sort_merge_sort_modified(UnsortedArray *unsortedArray, unsigned long firstPosition, unsigned long lastPosition){
    unsigned long middlePosition;
    unsigned long currentSize = lastPosition - firstPosition + 1;
    if (firstPosition < lastPosition){
        middlePosition = (firstPosition + lastPosition) / 2;
        if (currentSize <= KEY_VALUE)
            array_to_sort_binary_insertion_sort(unsortedArray, firstPosition, lastPosition);
        else{
            array_to_sort_merge_sort_modified(unsortedArray, firstPosition, middlePosition);
            array_to_sort_merge_sort_modified(unsortedArray, middlePosition + 1, lastPosition);
            merge(unsortedArray, firstPosition, middlePosition, lastPosition);
        }
    }
    return;
}

/*
 * It merges two ordered sub-array
 */
void merge(UnsortedArray *unsortedArray, unsigned long firstPosition, unsigned long middlePosition, unsigned long lastPosition){
    unsigned long sizeArrayLeft;
    unsigned long sizeArrayRight;

    sizeArrayLeft = middlePosition - firstPosition + 1;
    sizeArrayRight = lastPosition - middlePosition;

    void **arrayLeftElement = malloc(sizeof(*arrayLeftElement) * sizeArrayLeft);
    void **arrayRightElement = malloc(sizeof(*arrayRightElement) * sizeArrayRight);

    for (unsigned long i = 0; i < sizeArrayLeft; i++)
        arrayLeftElement[i] = unsortedArray->array[firstPosition + i];
    for (unsigned long j = 0; j < sizeArrayRight; j++)
        arrayRightElement[j] = unsortedArray->array[middlePosition + 1 + j];

    unsigned long indexLeftArray, indexRightArray, indexArray;
    indexLeftArray = 0;
    indexRightArray = 0;
    indexArray = firstPosition;

    while (indexLeftArray < sizeArrayLeft && indexRightArray < sizeArrayRight){
        if (unsortedArray->compare(arrayLeftElement[indexLeftArray], arrayRightElement[indexRightArray]) != 1){
            unsortedArray->array[indexArray] = arrayLeftElement[indexLeftArray];
            indexLeftArray++;
        }
        else{
            unsortedArray->array[indexArray] = arrayRightElement[indexRightArray];
            indexRightArray++;
        }
        indexArray++;
    }

    while (indexLeftArray < sizeArrayLeft){
        unsortedArray->array[indexArray] = arrayLeftElement[indexLeftArray];
        indexLeftArray++;
        indexArray++;
    }

    while (indexRightArray < sizeArrayRight){
        unsortedArray->array[indexArray] = arrayRightElement[indexRightArray];
        indexRightArray++;
        indexArray++;
    }

    free(arrayLeftElement);
    free(arrayRightElement);
    return;
}
