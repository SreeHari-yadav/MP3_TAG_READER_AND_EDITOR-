#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "edit.h"
#include "types.h"

// Check command line options and choose the correct standard ID3v2 frame type
Status read_and_validate_ed(char *argv[], EDIT *edit)
{
    if (argv[2] != NULL && argv[3] != NULL &&
        argv[4] != NULL && strstr(argv[4], ".mp3") != NULL)
    {
        if (strcmp(argv[2], "-y") == 0)
            strcpy(edit->tag, "TYER"); // Map to year tag
        else if (strcmp(argv[2], "-t") == 0)
            strcpy(edit->tag, "TIT2"); // Map to title tag
        else if (strcmp(argv[2], "-A") == 0)
            strcpy(edit->tag, "TALB"); // Map to album tag
        else if (strcmp(argv[2], "-a") == 0)
            strcpy(edit->tag, "TPE1"); // Map to artist tag
        else if (strcmp(argv[2], "-g") == 0)
            strcpy(edit->tag, "TCON"); // Map to genre tag
        else if (strcmp(argv[2], "-c") == 0)
            strcpy(edit->tag, "COMM"); // Map to comment tag
        else
        {
            printf(RED "Invalid tag for %s\n" RESET, argv[4]);
            return e_failure;
        }

        edit->size_of_data = strlen(argv[3]);
        edit->r_Data = argv[3];
        edit->file_name = argv[4];
        edit->file_dname = "dup.mp3"; // Temporary copy file path

        return e_success;
    }

    printf(RED "Arguments are incorrect. Use ./a.out -h for help\n" RESET);
    return e_failure;
}

// Open the original source asset and create the empty duplicate working file
Status to_open_mp3file(EDIT *edit)
{
    edit->fptr_fname = fopen(edit->file_name, "rb");

    if (edit->fptr_fname == NULL)
    {
        perror("fopen");
        fprintf(stderr, RED "Unable to open %s\n" RESET, edit->file_name);
        return e_failure;
    }

    edit->fptr_dfname = fopen(edit->file_dname, "wb+");

    if (edit->fptr_dfname == NULL)
    {
        perror("fopen");
        fclose(edit->fptr_fname);
        return e_failure;
    }

    return e_success;
}

// Check if the file starts with "ID3" to confirm a valid metadata layout
Status checking_header_(EDIT *edit)
{
    rewind(edit->fptr_fname);

    if (fread(edit->hdr_data, 1, 5, edit->fptr_fname) != 5)
        return e_failure;

    if ((memcmp(edit->hdr_data, "ID3", 3) != 0 ||
        (unsigned char)edit->hdr_data[3] != 0x03 ||
        (unsigned char)edit->hdr_data[4] != 0x00))
    {
        printf("Header: %.3s\n", edit->hdr_data);
        printf("Version: %02X\n", (unsigned char)edit->hdr_data[3]);
        printf("Revision: %02X\n", (unsigned char)edit->hdr_data[4]);
        printf("File is not ID3 v2.3.0 \n");
        printf(RED "File is not ID3 v2.3.0\n" RESET);
        return e_failure;
    }

    return e_success;
}

// Extract and calculate the size of the original ID3 tag framework block
Status store_size_of_file(EDIT *edit)
{
    unsigned char f_size[4];

    if (fseek(edit->fptr_fname , 6, SEEK_SET) != 0)
        return e_failure;

    if (fread(f_size, 1, 4, edit->fptr_fname) != 4)
        return e_failure;

    // Convert the 7-bit synchsafe layout back to a standard integer size
    edit->file_size =
        ((uint)(f_size[0] & 0x7F) << 21) |
        ((uint)(f_size[1] & 0x7F) << 14) |
        ((uint)(f_size[2] & 0x7F) << 7) |
        (uint)(f_size[3] & 0x7F);

    return e_success;
}

// Swap integer byte directions to switch between Big-Endian and Little-Endian format
void do_litend_size(uint *dsize)
{
    unsigned char *bytes = (unsigned char *)dsize;

    for (int i = 0; i < sizeof(int) / 2; i++)
    {
        unsigned char temp = bytes[i];
        bytes[i] = bytes[sizeof(int) - 1 - i];
        bytes[sizeof(int) - 1 - i] = temp;
    }
}

// Core tag loop structure that locates headers and writes updated content fields
Status editing_tag_data(EDIT *edit)
{
    unsigned char header[10];
    unsigned char frame[10];
    unsigned char size_bytes[4];
    unsigned int old_size,new_size=0;
    unsigned int consumed_size=0;
    unsigned int frame_size,data_size,original_frame_size;
    unsigned char *data;
    int found=0;


    rewind (edit->fptr_fname);

    if(fread(header,1,10,edit->fptr_fname)!=10)
    {
        return e_failure;
    }
    if(fwrite(header,1,10,edit->fptr_dfname)!=10)
    {
        return e_failure;
    }
    /*
     * Preserve the global ID3 header.
     * Its size field will be updated after writing the frames.
     */

    old_size=edit->file_size;

    while(consumed_size < old_size)
    {
        /*
         * Read only the frame header if enough tag bytes remain.
         */
        if(old_size - consumed_size < 10)
        {
            break;
        }
        if(fread(frame,1,10,edit->fptr_fname)!=10)
        {
            return e_failure;
        }
        if(frame[0]==0)
        {
            break;
        }

        frame_size=((uint)frame[4] << 24) | 
                    ((uint)frame[5] << 16) |
                    ((uint)frame[6] << 8)  |
                    (uint)frame[7];
        original_frame_size=frame_size;

        data=malloc(frame_size);

        if(data==NULL)
        {
            return e_failure;
        }

        if(fread(data,1,frame_size,edit->fptr_fname)!=frame_size)
        {
            free(data);
            return e_failure;
        }
        consumed_size += 10 + original_frame_size;

        if(memcmp(frame,edit->tag,4)==0)
        {
            found=1;

            if(strcmp(edit->tag,"COMM")==0)
            {
                /*
                 * COMM payload:
                 * encoding byte + language (3) +
                 * empty description terminator (1) + comment
                 */
                
                data_size= edit ->size_of_data +5;
                free(data);
                data=calloc(data_size,1);

                if(data==NULL)
                {
                    return e_failure;
                }

                memcpy(data+1,"eng",3);
                memcpy(data+5,edit->r_Data,edit->size_of_data);
            }
            else
            {
                /*
                 * Text frame payload:
                 * encoding byte + text
                 */
                data_size=edit->size_of_data +1;
                free(data);
                data=malloc(data_size);

                if(data==NULL)
                {
                    return e_failure;
                }
                data[0]=0;
                memcpy(data+1,edit->r_Data,edit->size_of_data);
             }

            frame_size=data_size;
            frame[4]=(frame_size>>24) & 0XFF;
            frame[5]=(frame_size>>16) & 0XFF;
            frame[6]=(frame_size>>8) & 0XFF;
            frame[7]=(frame_size) & 0XFF;
        }

        fwrite(frame,1,10,edit->fptr_dfname);
        fwrite(data,1,frame_size,edit->fptr_dfname);

        new_size+=10 +frame_size;
        free(data);
    }
    if(found==0)
    {
        printf(RED "Tag not found in MP3 file \n" RESET);
        return e_failure;
    }
    /*
     * Convert the final calculated total size counter back to a 7-bit synchsafe layout 
     * and overwrite the master layout header block fields.
     */
    size_bytes[0]=(new_size>>21) & 0X7F;
    size_bytes[1]=(new_size>>14) & 0X7F;
    size_bytes[2]=(new_size>>7) & 0X7F;
    size_bytes[3]=(new_size) & 0X7F;

    fseek(edit->fptr_dfname,6,SEEK_SET);
    fwrite(size_bytes,1,4,edit->fptr_dfname);

    //fseek(edit->fptr_dfname,10 + old_size,SEEK_SET);
    // Reposition back to the end of the total metadata region
    fseek(edit->fptr_dfname,0,SEEK_END);

    if(fseek(edit->fptr_fname, 10 + old_size, SEEK_SET) != 0)
    {
        return e_failure;
    }


    return e_success;
}

// Copy the untouched audio track stream bytes directly over to the final output file
Status copying_data(EDIT *edit)
{
    int ch;
    while ((ch = fgetc(edit->fptr_fname)) != EOF)
    {
        if (fputc(ch, edit->fptr_dfname) == EOF)
        {
            return e_failure;
        }
    }

    if (ferror(edit->fptr_fname))
    {
        return e_failure;
    }
    return e_success;
}

// Coordinator function driving the entire metadata edit pipeline structure
Status to_do_editing(char *argv[], EDIT *edit)
{
    if (read_and_validate_ed(argv, edit) == e_failure)
    {
        printf(RED "Invalid arguments or tag\n" RESET);
        return e_failure;
    }
    if (to_open_mp3file(edit) == e_failure)
    {
        printf(RED "The file failed to open\n" RESET);
        return e_failure;
    }
    if (checking_header_(edit) == e_failure)
    {
        printf(RED "The file is not ID3 v2.3.0\n" RESET);
        goto failure;
    }
    if (store_size_of_file(edit) == e_failure)
    {
        printf(RED "Unable to read the ID3 tag size of mp3 file\n" RESET);
        goto failure;
    }
    if (editing_tag_data(edit) == e_failure)
    {
        printf(RED "Unable to edit the tag given by you\n" RESET);
        goto failure;
    }
    if (copying_data(edit) == e_failure)
    {
        printf(RED "Unable to copy the remaining MP3 audio data\n" RESET);
        goto failure;
    }
    fclose(edit->fptr_fname);
    fclose(edit->fptr_dfname);
    // Swap our temporary file over onto the original destination
    if (rename(edit->file_dname, edit->file_name) != 0)
    {
        perror("rename");
        return e_failure;
    }
    printf(CYAN"\n-------------------------------------------------------------------------------\n\n"RESET);
    printf(GREEN "\t\t\tMP3 Tag edited successfully\n" RESET);
    printf(CYAN"\n-------------------------------------------------------------------------------\n\n"RESET);
    return e_success;
failure:
    fclose(edit->fptr_fname);
    fclose(edit->fptr_dfname);
    remove(edit->file_dname); // Safely clean up duplicate working copies on structural script failure
    return e_failure;
}