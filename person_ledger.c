#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum {
    false,
    true
} bool;

typedef struct{
    char *name;
    int age;
} profile;

bool set_person_name(profile* person, char* new_name)
{
    if(new_name == NULL || person == NULL){
        return false;
    }
    person->name = new_name;
    return true;
};

int main() {
    profile *Profile = malloc(sizeof(profile));
    Profile->age = 10;
    char *personname = malloc(100 + 1);
    strcpy(personname, "Brian");
    if(set_person_name(Profile, personname)){
        printf("Person name is: %s\n", Profile->name);
    }
    else{
        printf("Couldn't set name");
    }
    free(Profile);
    free(personname);
    return 0;
}