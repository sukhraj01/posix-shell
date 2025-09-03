#include "header.h"
#include <readline/readline.h>
#include <readline/history.h>

using namespace std;

int main() {
    initializeZombieCleaner();
    initShellHome();
    string cmdline;

    while (true) {

        // Use readline instead of getline for better input handling
        char* input = readline(shellInfo().c_str()); // shows shell info and waits for user
        if (input == nullptr) break;   // EOF (Ctrl+D)
        cmdline = string(input);
        if (!cmdline.empty()) add_history(cmdline.c_str());
        free(input);                   // readline allocates memory, free it

        if (cmdline.empty()) continue;
        if (cmdline == "exit") break;


        // Add command to history (from readline)
        // addToHistory(cmdline);          // <-- NEW: preserves your cmdline for other uses

        // Use parser
        cmdlist clist = parser(cmdline); // List of all commands with their args
    
        
        for (struct command cmd: clist.commands) {
            recordHistory(cmd);
            if (isBuiltIn(cmd)) {
                executeBuiltIn(cmd);
            } else {
                systemCmd(cmd);
            }
        }
        // print_info(clist);    
    }

    return 0;
}






// #include "header.h"

// using namespace std;

// int main() {
//     initShellHome();
//     string cmdline;

//     while (true) {

//         cout << shellInfo();   // prompt

//         if (!getline(cin, cmdline)) break;

//         if (cmdline.empty()) continue;
//         if (cmdline == "exit") break;

//         // Use parser
//         cmdlist clist = parser(cmdline); // List of all commands with their args

//         for (struct command cmd: clist.commands) {
//             if (isBuiltIn(cmd)) {
//                 executeBuiltIn(cmd);
//             }
//         }
//         // print_info(clist);    
//     }

//     return 0;
// }
