#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "view.h"
#include "edit.h"
#include "types.h"

// Function to print a helpful user guide menu with proper usage examples
void to_do_helping()
{
    printf(CYAN"\n-------------------------------------------------------------------------------\n\n"RESET);
    printf(MAGENTA"\t\t\t\tUsage\n"RESET);
    printf(CYAN"\n-------------------------------------------------------------------------------\n\n"RESET);
    printf(MAGENTA"For viewing please use  :\n");
    printf(YELLOW"\n ./a.out  -v  mp3filename\n"RESET);
    printf(CYAN"\n-------------------------------------------------------------------------------\n\n"RESET);
    printf(MAGENTA"For editing please use  :\n"RESET);
    printf(YELLOW"\n ./a.out  -e -y new year mp3filename\t\t(to edit year of song)");
    printf("\n ./a.out  -e -t new title mp3filename\t\t(to edit title of song)");
    printf("\n ./a.out  -e -A new album name mp3filename\t(to edit album name of song)");
    printf("\n ./a.out  -e -a new artist name mp3filename\t(to edit artist name of song)");
    printf("\n ./a.out  -e -g new genre name mp3filename\t(to edit genre of song)");
    printf("\n ./a.out  -e -c new comment mp3filename\t\t(to edit comment of song)\n"RESET);
    printf(CYAN"\n-------------------------------------------------------------------------------\n\n"RESET);
    return;
}

// Function to check what operation (-h, -v, or -e) the user typed in the command line
Status operation_to_do(char *argv[])
{
    // If the first option is "-h", return help mode status
    if(strcmp(argv[1], "-h") == 0)
    {
        return h_help;
    }
    // If the first option is "-v", return view mode status
    else if(strcmp(argv[1], "-v") == 0)
    {
        return v_view;
    }
    // If the first option is "-e", return edit mode status
    else if(strcmp(argv[1], "-e") == 0)
    {
        return e_edit;
    }
    // If it is anything else, it is an unsupported wrong flag
    else
    {
        return unsupported;
    }
}

int main(int argc, char *argv[])
{
    // Make sure the user typed at least 2 command line arguments (e.g., ./a.out -h)
    if(argc >= 2)
    {
        // Identify which tool operation mode the user wants to run
        Operationtype res = operation_to_do(argv);
        
        // VIEW MODE 
        if(res == v_view)
        {
            VIEW view; // Structure variable to hold MP3 tag view information
            Status to_view = to_do_viewing(argv, &view); // Call the viewing function
            
            // If viewing hits an error or fails, close the program
            if(to_view == e_failure)
            {
                printf(RED"\nFailed to View the details of mp3 file!!\n"RESET);
                exit(0);
            }
        }
        // EDIT MODE 
        else if(res == e_edit)
        {
            EDIT edit; // Structure variable to hold MP3 tag editing details
            Status ed = to_do_editing(argv, &edit); // Call the editing function
            
            // If modification fails, stop the application execution
            if(ed == e_failure)
            {
                printf(RED"\nFailed to edit the details of mp3 file!!\n"RESET);
                exit(0);
            }
            else
            {
                printf(CYAN"\n-------------------------------------------------------------------------------\n\n"RESET);   
                printf(GREEN"\t\tSuccesfully edited  the content of the mp3 file\n"RESET);
                printf(CYAN"\n-------------------------------------------------------------------------------\n\n"RESET);
            }
        }
        // HELP MODE 
        else if(res == h_help)
        {
            to_do_helping(); // Call the help information print module
        } 
        // INVALID MODE
        else
        {
            printf(RED"\nfor help please  use  : ./a.out -h \n"RESET);
        }
    }
    // Executed if the user runs just "./a.out" without any option flags
    else
    {
        printf(RED"\nShould pass atleast 2 arguments\n");
        printf("\nFor help please use : ./a.out -h \n"RESET);
        exit(0);
    }
    
    return 0;
}
