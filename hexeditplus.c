#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char debug_mode;
    char file_name[128];
    int unit_size;
    unsigned char mem_buf[10000];
    size_t mem_count;
    char display_mode;  // New field for display mode
} state;

void toggle_debug_mode(state* s) {
    if (s->debug_mode) {
        printf("Debug flag now off\n");
        s->debug_mode = 0;
    } else {
        printf("Debug flag now on\n");
        s->debug_mode = 1;
    }
}

void set_file_name(state* s) {
    printf("Enter file name: ");
    fgets(s->file_name, sizeof(s->file_name), stdin);
    s->file_name[strcspn(s->file_name, "\n")] = 0; // Remove the newline character

    if (s->debug_mode) {
        fprintf(stderr, "Debug: file name set to '%s'\n", s->file_name);
    }
}

void set_unit_size(state* s) {
    printf("Enter unit size (1, 2, or 4): ");
    int size;
    scanf("%d", &size);

    if (size == 1 || size == 2 || size == 4) {
        s->unit_size = size;
        if (s->debug_mode) {
            fprintf(stderr, "Debug: set size to %d\n", s->unit_size);
        }
    } else {
        printf("Invalid size\n");
    }
    while (getchar() != '\n'); // Clear the input buffer
}

void load_into_memory(state *s)
{
    if (strcmp(s->file_name, "") == 0) {
        printf("Error: file name is empty\n");
        return;
    }

    FILE* file = fopen(s->file_name, "rb");
    if (!file) {
        printf("Error: cannot open file '%s'\n", s->file_name);
        return;
    }

    char input[256];
    //new field
    char location_str[256];
    //end new field
    unsigned int location;
    int length;

    printf("Please enter <location> <length>: ");
    fgets(input, sizeof(input), stdin);
    //new func
    // Extract location as a string first 
    sscanf(input, "%s %d", location_str, &length); 
    // Convert the location string to an unsigned int with base 16 
    location = (unsigned int)strtol(location_str, NULL, 16);
    //end new func


    // sscanf(input, "%x", "%d", &location, &length);

    if (s->debug_mode) {
        fprintf(stderr, "Debug: file name: %s, location: %#010x, length: %d\n", s->file_name, location, length);
    }


    fseek(file, location, SEEK_SET);
    fread(s->mem_buf, s->unit_size, length, file);
    printf("Loaded %d units into memory\n",length);
    
    s->mem_count = s->unit_size * length;
    printf("s->mem_count : %d\n",s->mem_count);
    fclose(file);
}

void toggle_display_mode(state* s) {
    if (s->display_mode) {
        printf("Display flag now off, decimal representation\n");
        s->display_mode = 0;
    } else {
        printf("Display flag now on, hexadecimal representation\n");
        s->display_mode = 1;
    }
}

void file_display(state *s) {
    if (strcmp(s->file_name, "") == 0) {
        printf("Error: file name is empty\n");
        return;
    }

    FILE* file = fopen(s->file_name, "rb");
    if (!file) {
        printf("Error: cannot open file '%s'\n", s->file_name);
        return;
    }

    unsigned int offset;
    int u;
    printf("Please enter <offset> <units>:\n");
    char input[256];
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %d", &offset, &u);

    if (s->debug_mode) {
        fprintf(stderr, "Debug: file name: %s, offset: %#x, units: %d\n", s->file_name, offset, u);
    }

    fseek(file, offset, SEEK_SET);

    for (int i = 0; i < u; i++) {
        unsigned char buffer[4]; // Maximum size is 4 bytes as per unit_size
        size_t read_size = fread(buffer, 1, s->unit_size, file);
        if (read_size < (size_t)s->unit_size) {
            printf("Error: could not read full unit at index %d\n", i);
            break;
        }
        unsigned int value = 0;
        memcpy(&value, buffer, s->unit_size);

        if (s->display_mode) {
            // Hexadecimal representation
            printf("%#x\n", value);
        } else {
            // Decimal representation
            printf("%u\n", value);
        }
    }

    fclose(file);    
}

void memory_display(state* s) {
    printf("Enter address and length:\n");
    char input[256];
    unsigned int addr;
    int u;
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %d", &addr, &u);

    if (s->display_mode) {
        printf("Hexadecimal\n");
        printf("===========\n");
        for (int i = 0; i < u; ++i) {
            unsigned int value = 0;
            memcpy(&value, s->mem_buf + addr + (i * s->unit_size), s->unit_size);
            // Mask the value to fit the unit size (avoiding overflow)
            if (s->unit_size == 1) value &= 0xFF;
            if (s->unit_size == 2) value &= 0xFFFF;
            if (s->unit_size == 4) value &= 0xFFFFFFFF;
            printf("%X\n", value);
        }
    } else {
        printf("Decimal\n");
        printf("=======\n");
        for (int i = 0; i < u; ++i) {
            unsigned int value = 0;
            memcpy(&value, s->mem_buf + addr + (i * s->unit_size), s->unit_size);

            // Mask the value to fit the unit size (avoiding overflow)
            if (s->unit_size == 1) value &= 0xFF;
            if (s->unit_size == 2) value &= 0xFFFF;
            if (s->unit_size == 4) value &= 0xFFFFFFFF;
            printf("%u\n", value);
        }
    }
}

void save_into_file(state* s) 
{
    if(strcmp("", s-> file_name)== 0) {
        printf("Error: empty file name\n");
        return;
    }
    FILE* file= fopen(s-> file_name, "r+");    
    if(file== NULL) {
        printf("Error: unable to open file\n");
        return;
    }
    printf("Please enter <source-address> <target-location> <length>:\n");
    int address= 0;
    int location= 0;
    int length= 0;
    scanf("%x %x %d", &address, &location, &length);
    fseek(file, 0, SEEK_END);   
    long file_size= ftell(file);
    if(location> file_size) {    
        printf("Error: target location is greater than the file size\n");
        fclose(file);
        return;
    }
    fseek(file, location, SEEK_SET);  
    if(address== 0) {
        fwrite(&s->mem_buf, s->unit_size, length, file);
    } 
    else {
        fwrite(&address, s->unit_size, length, file);
    }


    printf("Wrote %d units into file\n", length);       
    fclose(file);
}

void memory_modify(state* s) {
    printf("Please enter <location> <val>:\n");
    char input[256];
    unsigned int location;
    unsigned int val;
    fgets(input, sizeof(input), stdin);
    sscanf(input, "%x %x", &location, &val);

    if (s->debug_mode) {
        fprintf(stderr, "Debug: location: %#x, val: %#x\n", location, val);
    }

    if (location + s->unit_size > sizeof(s->mem_buf)) {
        printf("Error: Invalid location (out of bounds)\n");
        return;
    }

    memcpy(s->mem_buf + location, &val, s->unit_size);

    printf("Modified memory at %#x with %#x\n", location, val);
}


void quit(state* s) {
    if (s->debug_mode) {
        printf("quitting\n");
    }
    exit(0);
}

typedef struct {
    char* name;
    void (*fun)(state*);
} menu_item;

int main(void) {
    state s = {0, "deep_thought", 1, {0}, 0, 0}; // Initialize display_mode to 0 (decimal mode)
    menu_item menu[] = {
        {"Toggle Debug Mode", toggle_debug_mode},
        {"Set File Name", set_file_name},
        {"Set Unit Size", set_unit_size},
        {"Load Into Memory", load_into_memory},
        {"Toggle Display Mode", toggle_display_mode},
        {"File Display", file_display},
        {"Memory Display", memory_display},
        {"Save Into File", save_into_file},
        {"Memory Modify", memory_modify},
        {"Quit", quit},
        {NULL, NULL}
    };

    while (1) {
        if (s.debug_mode) {
            fprintf(stderr, "Debug:\nunit_size: %d\nfile_name: %s\nmem_count: %zu\n", s.unit_size, s.file_name, s.mem_count);
        }

        printf("Choose action:\n");
        for (int i = 0; menu[i].name != NULL; i++) {
            printf("%d-%s\n", i, menu[i].name);
        }

        char input[10];
        fgets(input, sizeof(input), stdin);
        int choice = atoi(input);

        if (choice >= 0 && choice < (int)((sizeof(menu) / sizeof(menu_item)) - 1)) {
            menu[choice].fun(&s);
        } else {
            printf("Invalid choice\n");
        }
        if (choice == 6) while (getchar() != '\n');
    }

    return 0;
}
