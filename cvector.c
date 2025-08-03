#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum {
    false,
    true
} bool;

typedef struct{
    void* data;
    int size;
    int capacity;
    size_t element_size;
}vector;

void init_vector(vector *v, size_t element){
    v->size = 0; 
    v->capacity = 4;
    v->element_size = element;
    v->data = malloc(v->capacity * v->element_size);
};
void check_size_and_reallocate(vector *v)
{
    if(v->capacity == v->size)
    {
        v->capacity *= 2;
        v->data = realloc(v->data,v->capacity * v->element_size);
    }

};

void push_back(vector *v, void *elements)
{
    check_size_and_reallocate(v);
    memcpy((char*)v->data + v->size * v->element_size, elements, v->element_size);
    v->size++;
};

void get_vector_element(vector *v, int index, void *result)
{
    memcpy(result, (char*)v->data + index * v->element_size, v->element_size);
}
void pop_back_vector_element(vector *v, void *result){
    if(v->size <= 0){
        return;
    }

    void *last_elem_ptr = (char*)v->data + (v->size - 1) * v->element_size;
    memcpy(&result, last_elem_ptr, v->element_size);
    
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
}

int main(){
    vector v;
    init_vector(&v, sizeof(int));

    for(int i = 0; i < 10; i++)
    {
        int val = i;
        push_back(&v, &val);
    }

    for(int i = 0; i < v.size; i++)
    {
        int value;
        get_vector_element(&v, i, &value);
        printf("Element %u is containing %u '\n'", i, value);
    }
    int popped_back; 
    pop_back_vector_element(&v, &popped_back);
    printf("Back element equals: %u '\n'", popped_back);

    for(int i = 0; i < v.size; i++)
    {
        int value;
        get_vector_element(&v, i, &value);
        printf("Element after pop %u is containing %u '\n'", i, value);
    
    }

    int popped_front; 
    pop_front_vector_element(&v, &popped_front);
    printf("Back element equals: %u '\n'", popped_front);

    for(int i = 0; i < v.size; i++)
    {
        int value;
        get_vector_element(&v, i, &value);
        printf("Element after pop %u is containing %u '\n'", i, value);
    
    }
    free(v.data);
};
