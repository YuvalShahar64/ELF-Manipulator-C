#include <stdio.h>

int count_digits(const char *s) {
    int count = 0;
    while (*s) {
        if (*s >= '0' && *s <= '9') {
            count++;
        }
        s++;
    }
    return count;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <string>\n", argv[0]);
        return 1;
    }

    int count = count_digits(argv[1]);
    printf("The number of digits in the string is: %d\n", count);

    return 0;
}
