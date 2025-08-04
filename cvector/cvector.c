#include "cvector.h"


void init_vector(vector *v, size_t element){
    v->size = 0; 
    v->capacity = 4;
    v->element_size = element;
    v->data = malloc(v->capacity * v->element_size);
};

void check_size_and_reallocate(vector *v){
    if(v->capacity == v->size)
    { 
        v->capacity *= 2;
        v->data = realloc(v->data,v->capacity * v->element_size);
    }

};

void push_back(vector *v, void *elements){
    check_size_and_reallocate(v);
    memcpy((char*)v->data + v->size * v->element_size, elements, v->element_size);
    v->size++;
};

void get_vector_element(vector *v, int index, void *result){
    memcpy(result, (char*)v->data + index * v->element_size, v->element_size);
};

void pop_back_vector_element(vector *v, void *result){
    if(v->size <= 0){
        return;
    }
    void *last_elem_ptr = (char*)v->data + (v->size - 1) * v->element_size;
    memcpy(result, last_elem_ptr, v->element_size);  
    v->size--;

    return;
};

void pop_front_vector_element(vector *v, void *result){
    if(v->size <= 0){
        return;
    }

    memcpy(result, v->data, v->element_size);
    memmove(v->data, (char*)v->data + v->element_size,(v->size - 1) * v->element_size);
    v->size--;
    return;
};

void remove_vector_element_at_index(vector *v, int index){
    if (index < 0 || index >= v->size) {
        return;
    }

    void* target = (char*)v->data + index * v->element_size;
    memset(target, 0, v->element_size);
};

void resize_vector(vector *v, int size){
    if(size < 0){
        return;
    }

    void* new_data = realloc(v->data, size * v->element_size);
    if (new_data == NULL && size > 0) {
        return;
    }
    v->data = new_data;
    v->size = size;
    v->capacity = size;
    

};