#ifndef TYPES_H
#define TYPES_H

// Color codes to change the text color inside the terminal screen window
#define RED     "\033[31m"   // Changes text to red color
#define GREEN   "\033[32m"   // Changes text to green color
#define YELLOW  "\033[33m"   // Changes text to yellow color
#define BLUE    "\033[34m"   // Changes text to blue color
#define MAGENTA "\033[35m"   // Changes text to magenta color
#define CYAN    "\033[36m"   // Changes text to cyan (sky blue) color
#define RESET   "\033[0m"    // Resets the text color back to the standard default color

// A short name for 'unsigned int' so we don't have to type out the whole thing every time
typedef unsigned int uint;

// Options to show if a function completed its task correctly or hit an error
typedef enum
{
    e_success, // Returned when the function works perfectly (value is 0)
    e_failure, // Returned when something goes wrong or fails (value is 1)
}Status;

// Options to keep track of what specific tool mode the user wants to run
typedef enum 
{
    v_view,      // Selected when the user chooses the view mode option
    e_edit,      // Selected when the user chooses the edit mode option
    h_help,      // Selected when the user requests the help menu option
    unsupported  // Selected when the user types an invalid or wrong option argument
}Operationtype;

#endif
