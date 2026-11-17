#ifndef EDIT_H
#define EDIT_H

#include "types.h"

// Buffer size constants used for array limits definitions
#define sizee 5
#define sizze 6

// Structure to hold all the variables required for editing MP3 ID3 tags
typedef struct _EDIT
{
    char tag[sizee];      // Array buffer to store the 4-character tag frames (like TIT2)
    char *file_name;      // Stores the name/path string of the input MP3 file
    FILE *fptr_fname;     // File connection pointer for the original input MP3 file
    char *file_dname;     // Stores the temporary destination file name/path string
    FILE *fptr_dfname;    // File connection pointer for the new updated destination file
    char *r_Data;         // Pointer buffer to hold the new replacement tag text data
    uint size_of_data;    // Holds the text content string length size for the active tag
    char hdr_data[sizze]; // Array buffer to read and hold the file header signature bytes
    uint file_size;       // Holds the calculated total size value of the MP3 file tag

}EDIT;

/* Function Prototypes */

// Primary execution coordinator driving the full metadata writer pipeline process
Status to_do_editing(char *argv[],EDIT *edit);

// Validates the editing command line arguments and verifies input parameters
Status read_and_validate_ed(char *argv[],EDIT *edit);

// Opens the original input MP3 file in read-binary mode safely
Status to_open_mp3file(EDIT *edit);

// Verifies if the file header starts with "ID3" to confirm tag compatibility
Status checking_header_(EDIT *edit);

// Reads out the overall size metadata value of the active MP3 file tag block
Status store_size_of_file(EDIT *edit);

// Converts metadata integers from standard Big-Endian layout to Little-Endian format
void do_litend_size(uint *dsize);

// Finds the target tag inside the file structure and overwrites it with new data
Status editing_tag_data(EDIT *edit);

// Copies the remaining untouched audio stream data blocks to finish the file
Status copying_data(EDIT *edit);

#endif
