#include "utils.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void trim(char *str) {
    char *start = str;
    char *end;
    
    while (*start == ' ' || *start == '\t' || *start == '\r') start++;
    
    end = start + strlen(start) - 1;
    while (end > start && (*end == ' ' || *end == '\t' || *end == '\r' || *end == '\n')) end--;
    
    *(end + 1) = '\0';
    
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }
}

void clear_input() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void fix_gender(char *gender) {
    trim(gender);
    if (strlen(gender) == 0) {
        strcpy(gender, "?");
        return;
    }
    char first = gender[0];
    if (first == 'M' || first == 'm') {
        strcpy(gender, "М");
    } else if (first == 'F' || first == 'f' || first == 'Ж' || first == 'ж') {
        strcpy(gender, "Ж");
    } else if (first == 'М' || first == 'м') {
        strcpy(gender, "М");
    } else {
        strcpy(gender, "?");
    }
}
