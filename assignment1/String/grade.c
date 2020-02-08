#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include "my_string.h"

#define STR_EQUAL(a, b) (strcmp(a, b) == 0)

int main(int argc, char **argv) {
  srand(time(NULL));

  if (argc < 2) {
    printf("Usage: %s <function> <arguments>\n", argv[0]);
    exit(1);
  } else if (STR_EQUAL(argv[1], "strlen")) {
    if (argc != 3) {
      printf("Usage: %s strlen <string>\n", argv[0]);
      exit(1);
    }
    char* s1 = argv[2];
    printf("%s\n", s1);
    printf("%d\n", strlen(s1) == my_strlen(s1));
  } else if (STR_EQUAL(argv[1], "strcpy")) {
    if (argc != 3) {
      printf("Usage: %s strcpy <string>\n", argv[0]);
      exit(1);
    }
    char* s1 = argv[2];
    char* s2 = malloc(strlen(s1) + 1);
    char* s3 = malloc(strlen(s1) + 1);

    my_strcpy(s2, s1);
    strcpy(s3, s1);

    printf("%d\n", STR_EQUAL(s2, s3));

    free(s2);
    free(s3);
  } else if (STR_EQUAL(argv[1], "strncpy")) {
    if (argc != 4) {
      printf("Usage: %s strncpy <string> <number>\n", argv[0]);
      exit(1);
    }
    char* s1 = argv[2];
    int n = atoi(argv[3]);
    char* s2 = malloc(strlen(s1) + 1);
    char* s3 = malloc(strlen(s1) + 1);

    my_strncpy(s2, s1, n);
    strncpy(s3, s1, n);

    printf("%d\n", STR_EQUAL(s2, s3));

    free(s2);
    free(s3);
  } else if (STR_EQUAL(argv[1], "memmove")) {
    if (argc != 2) {
      printf("Usage: %s memmove\n", argv[0]);
      exit(1);
    }
    int len = 200;

    char* a = malloc(len);
    char* b = malloc(len);
    for (int i = 0 ; i < len ; i++) {
      a[i] = rand() % 255;
      b[i] = a[i];
    }

    int n = 100;
    int offset = (rand() % 30) + 20;

    memmove(a + offset, a, n);
    my_memmove(b + offset, b, n);

    int eq = memcmp(a, b, len);

    printf("%d\n", eq == 0 ? 1 : 0);

    free(a);
    free(b);
  } 
  
  return 0;
}
