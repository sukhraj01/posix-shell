// command.h
#ifndef STRUCT_H
#define STRUCT_H

#include <string>
#include <vector>

using namespace std;

// Represents a single command like ls, cd, pwd, echo
struct command {
    string name;           // Command name
    vector<string> args;   // Arguments of the command
    bool background = false; // True if command ends with &
    string input_file;     // Stores the name of the input file for '<'
    string output_file;    // Stores the name of the output file for '>' or '>>'
    bool append_output = false; // Flag to distinguish between '>' and '>>'
};

// Represents a list of commands entered in a single line
struct cmdlist {
    vector<command> commands; // All commands in the line
};

#endif // STRUCT_H  
