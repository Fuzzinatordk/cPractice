#ifndef CVECTOR_H
#define CVECTOR_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum {
    false,
    true
} bool;

typedef enum{
    TYPE_INT,
    TYPE_CHAR,
    TYPE_FLOAT
} vectorDataTypes;

typedef struct{
    void* data;
    int size;
    int capacity;
    size_t element_size;
    vectorDataTypes type;
} vector;

void init_vector(vector *v, size_t element, vectorDataTypes type);

void check_size_and_reallocate(vector *v);

void push_back(vector *v, void *elements);

void get_vector_element(vector *v, int index, void *result);

void pop_back_vector_element(vector *v, void *result);

void pop_front_vector_element(vector *v, void *result);

void remove_vector_element_at_index(vector *v, int index);

void resize_vector(vector *v, int size);

#endif