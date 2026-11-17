#ifndef VIEW_H
#define VIEW_H

#include "types.h"

// Buffer size constants used for array limits definitions
#define size 5
#define s_of_tag 5

// Structure to hold all the variables required for viewing MP3 ID3 tags
typedef struct _VIEW
{
    char *file_name;       // Stores the path/name string of the input MP3 file
    FILE *fptr_fname;      // Active file stream connection pointer for the MP3 file
    char header_data[size];// Array buffer to read and hold the 5-byte file header signature
    uint file_size;        // Holds the calculated total size value of the MP3 file tag
    char tag[s_of_tag];    // Array buffer to read 4-character tag frames (like TIT2, TALB)
    uint size_of_data;     // Holds the text content string length size for the active tag

}VIEW;

/* Function Prototypes */

// Checks the first input option to decide if it is a view, edit, or help command
Status operation_to_do(char *argv[]);

// Opens the target input MP3 file in read-binary mode safely
Status to_open_mp3file_(VIEW *view);

// Primary execution coordinator driving the full metadata reader pipeline process
Status to_do_viewing(char *argv[],VIEW *view);

// Validates the input command parameters and verifies the .mp3 filename suffix
Status read_and_validate(char *argv[],VIEW *view);

// Verifies if the file header starts with "ID3" and checks the framework version tags
Status checking_header(VIEW *view);

// Reads out the overall size metadata value of the active MP3 file tag block
Status store_size_of_file_(VIEW *view);

// Converts metadata integers from standard Big-Endian layout to Little-Endian format
void do_litend_size_(uint* dsize);

// Iterates through text segments inside the file to extract and print details
Status chekcktags_and_print(VIEW *view);

// Dynamically extracts and reads text content payloads matching individual frame headers
char * display(VIEW *view);

// Prints a detailed instruction usage layout help guide inside the console window
void to_do_helping();

#endif
