#include "my_string.h"


size_t my_strlen ( const char * str ) 
{
    //your code goes here
    int index = 0;
    while(str[index] != 0)
    {
        index++;
    }
    return index;
}

char * my_strcpy ( char * destination, const char * source )
{
    //your code goes here
    int index = 0;
    while(source[index] != 0)
    {
        destination[index] = source[index];
        index++;
    }
    return destination;
}


char * my_strncpy ( char * destination, const char * source, size_t num)
{
    //your code goes here
    for(int i = 0; i < num; i++)
    {
        if(source[i] != 0)
        {
            destination[i] = source[i];
        }
    }
    return destination;
}

void* my_memmove (void* destination, const void* source, size_t num)
{
    //your code goes here
    char buffer[num];
    char* src = (char*) source;
    char* dest = (char*) destination;
    //load into buffer
    for(int i = 0; i < num; i++)
    {
        if(src[i] != 0)
            buffer[i] = src[i];
    }
    //unload from buffer into destination
    for(int i = 0; i < num; i++)
    {
        dest[i] = buffer[i];
    }
    return;
}

