#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "view.h"
#include "types.h"

// Validates the input arguments to confirm it is a valid .mp3 file path
Status read_and_validate(char *argv[], VIEW *view)
{
    // Ensure the third argument is empty and the file string ends with ".mp3"
    if(((strstr(argv[2], ".mp3") != NULL)) && (argv[3] == NULL))
    {
        view->file_name = argv[2]; // Store file name inside structural container
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

// Opens the targeted MP3 file input stream in read-binary mode safely
Status to_open_mp3file_(VIEW *view)
{
    view->fptr_fname = fopen(view->file_name, "rb");
    if(view->fptr_fname == NULL)
    {
        perror("fopen"); // Prints system standard error description
        fprintf(stderr, RED"ERROR: Unable to open file %s\n"RESET, view->file_name);
        return e_failure;
    }
    return e_success;
}

// Verifies if the file header structure matches the expected ID3v2 metadata format
Status checking_header(VIEW *view)
{
    // Try reading the first 5 bytes containing signature and version values
    if((fread(view->header_data, 5, 1, view->fptr_fname)) != 1)
    {
        printf(RED"Cant able to access the header of mp3 file\n"RESET);
        return e_failure;
    }
    
    // Check if tag starts with "ID3" and version values match 03 00
    if (memcmp(view->header_data, "ID3", 3) != 0 || view->header_data[3] != 0x03 || view->header_data[4] != 0x00)
    {
        printf(RED"Header: %.3s\n"RESET, view->header_data);
        printf(RED"Version: %02X\n"RESET, (unsigned char)view->header_data[3]);
        printf(RED"Revision: %02X\n"RESET, (unsigned char)view->header_data[4]);
        return e_failure;
    }
    
    //view->header_data[5] = '\0'; // Properly null terminate string array
    return e_success;
}

// Extracts and reads the total size metadata value from the file stream
/*Status store_size_of_file_(VIEW *view)
{
    fseek(view->fptr_fname, 2, SEEK_CUR); // Skip unused flag frame bits
    if(ferror(view->fptr_fname))
    {
        return e_failure;
    }
    if(fread(&view->file_size, 1, 4, view->fptr_fname) != 4)
    {
        return e_failure;
    }
    
    do_litend_size_(&view->file_size); // Reassemble byte ordering sequence to little-endian
    return e_success;
}*/
Status store_size_of_file_(VIEW *view)
{
    unsigned char size_bytes[4];

    if(fseek(view->fptr_fname, 6, SEEK_SET) != 0)
        return e_failure;

    if(fread(size_bytes, 1, 4, view->fptr_fname) != 4)
        return e_failure;

    view->file_size =
        ((uint)(size_bytes[0] & 0x7F) << 21) |
        ((uint)(size_bytes[1] & 0x7F) << 14) |
        ((uint)(size_bytes[2] & 0x7F) << 7) |
        (uint)(size_bytes[3] & 0x7F);

    return e_success;
}


// Byte conversion engine: flips Big-Endian sizes from ID3 tags into Little-Endian integers
void do_litend_size_(uint *dsize)
{
    unsigned char *bytes = (unsigned char*)(dsize);
    for(int i = 0; i < sizeof(int) / 2; i++)
    {
        unsigned char temp = bytes[i];
        bytes[i] = bytes[sizeof(int) - 1 - i];
        bytes[sizeof(int) - 1 - i] = temp;
    }
}

// Parses through tag frames and displays text details sequentially
Status chekcktags_and_print(VIEW *view)
{
    int count = 0;
    char *str;
    fseek(view->fptr_fname, 10, SEEK_SET); // Skip past the initial file header
    
    printf(CYAN"\n----------------------------SELECTED VIEW DETAILS------------------------------\n"RESET);
    printf("\n\n");
    printf(CYAN"-------------------------------------------------------------------------------\n"RESET);
    printf(YELLOW"\t\t\t MP3 TAG READER AND EDITOR FOR ID3V2\n"RESET);
    printf(CYAN"-------------------------------------------------------------------------------\n\n"RESET);

    // Read blocks continuously until all 6 necessary meta fields are found
    while(count < 6)
    {
        if(fread(view->tag, 1, 4, view->fptr_fname) != 4)
        {
            break;
        }
        view->tag[4] = '\0'; // Set terminator boundary string wrapper
        
        // 1. Song Title tag check
        if (strcmp(view->tag, "TIT2") == 0)
        {
            str = display(view);
            if(str == NULL)
            {
                return e_failure;
            }
            printf(MAGENTA"TITLE OF THE SONG      : %s\n"RESET, str);
            free(str); // Release dynamically allocated block
            count++; str = NULL; continue;
        }
        
        // 2. Song Artist tag check
        if (strcmp(view->tag, "TPE1") == 0)
        {
            str = display(view);
            if(str == NULL)
            {
                return e_failure;
            }
            printf(MAGENTA"ARTIST OF THE SONG     : %s\n"RESET, str);
            free(str);
            count++; str = NULL; continue;
        }
        
        // 3. Album Title tag check
        if (strcmp(view->tag, "TALB") == 0)
        {
            str = display(view);
            if(str == NULL)
            {
                return e_failure;
            }
            printf(MAGENTA"ALBUM NAME OF THE FILE : %s\n"RESET, str);
            count++;
            free(str); str = NULL; continue;
        }
        
        // 4. Release Year tag check
        if (strcmp(view->tag, "TYER") == 0)
        {
            str = display(view);
            if(str == NULL)
            {
                return e_failure;
            }
            printf(MAGENTA"YEAR OF THE SONG       : %s\n"RESET, str);
            count++;
            free(str); str = NULL; continue;
        }
        
        // 5. Genre tag check
        if (strcmp(view->tag, "TCON") == 0)
        {
            str = display(view);
            if(str == NULL)
            {
                return e_failure;
            }
            printf(MAGENTA"GENRE OF THE SONG      : %s\n"RESET, str);
            free(str);
            count++; str = NULL; continue;
        }
        
        // 6. Comments tag check
        if (strcmp(view->tag, "COMM") == 0)
        {
            str = display(view);
            if (str == NULL)
            {
                return e_failure;
            }   

            // Skip past language metadata attributes hidden inside the comment block framework
            char *ptr = memchr(str + 3, '\0', view->size_of_data - 4);
            if (ptr != NULL)
            {
                printf(MAGENTA"COMMENT OF THE SONG    : %s\n"RESET, ptr + 1);
            }

            count++;
            free(str);
            str = NULL;
            continue;
        }
    }
    
    printf(CYAN"-------------------------------------------------------------------------------\n\n"RESET);
    printf(GREEN"\t\t\tCONTENTS DISPLAYED SUCCESFULLY\n"RESET); // Updated with GREEN color macro
    printf(CYAN"-------------------------------------------------------------------------------\n\n"RESET);
    return e_success;
}

// Extracts individual frame strings dynamically from text segment blocks
char* display(VIEW *view)
{
    if(fread(&view->size_of_data, 1, 4, view->fptr_fname) != 4)
    {
        return NULL;
    }
    
    do_litend_size_(&view->size_of_data); // Parse integer from big endian to little endian
    fseek(view->fptr_fname, 3, SEEK_CUR);  // Jump past unneeded status flag attributes bytes
    
    char *data = malloc(view->size_of_data);
    if(data == NULL)
    {
        return NULL;
    }
    
    // Read the character string block data payload content
    if(fread(data, 1, view->size_of_data - 1, view->fptr_fname) != view->size_of_data - 1)
    {
        free(data);
        return NULL;
    }
    
    data[view->size_of_data - 1] = '\0'; // Force safe string boundary termination closing
    return data;
}

// Coordinator function driving the full mp3 metadata reader pipeline layout
Status to_do_viewing(char *argv[], VIEW *view)
{
    Status val = read_and_validate(argv, view);
    if(val == e_failure)
    {
        printf(RED"The file must be of .mp3 extension\n"RESET);
        return e_failure;
    }
    if(to_open_mp3file_(view) == e_failure)
    {
        printf(RED"Mp3 File failed to open\n"RESET);
        return e_failure;
    }
    if(checking_header(view) == e_failure)
    {
        printf(RED"The mp3 file you have sent is not ID3 v2.0 type\n"RESET);
        return e_failure;
    }
    if(store_size_of_file_(view) == e_failure)
    {
        printf(RED"Unable to read the size of mp3 file\n"RESET);
        return e_failure;
    }
    
    rewind(view->fptr_fname); // Reset tracking position pointer back to zero index bounds
    
    if(chekcktags_and_print(view) == e_failure)
    {
        printf(RED"tag not found\n"RESET);
        return e_failure;
    }
    
    fclose(view->fptr_fname); // Close the active file link safely
    return e_success;
}
