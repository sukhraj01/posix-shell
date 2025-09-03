#include "header.h"

using namespace std;

cmdlist parser(const string& cmdLine) {
    cmdlist list;

    char *line = strdup(cmdLine.c_str());
    char *saveptr1;
    char *token = strtok_r(line, ";", &saveptr1);

    while (token) {
        string singleCmd(token);
        command cmd;
        cmdParser(singleCmd, cmd);

        if (!cmd.name.empty()) {
            list.commands.push_back(cmd);
        }
        token = strtok_r(nullptr, ";", &saveptr1);
    }

    free(line);
    return list;
}

void cmdParser(const string& singleCmd, command& cmd) {
    string temp = singleCmd;

    // trim front spaces
    temp.erase(temp.begin(), find_if(temp.begin(), temp.end(), [](int ch) { 
        return !isspace(ch); 
    }));
    // trim back spaces
    temp.erase(find_if(temp.rbegin(), temp.rend(), [](int ch) { 
        return !isspace(ch); 
    }).base(), temp.end());

    if (!temp.empty() && temp.back() == '&') {
        cmd.background = true;
        temp.pop_back();
    }

    char *line = strdup(temp.c_str());
    char *saveptr2;
    char *tok = strtok_r(line, " \t", &saveptr2);
    bool isCommand = true;

    while (tok) {
        // Check for redirection symbols
        if (strcmp(tok, "<") == 0) {
            tok = strtok_r(nullptr, " \t", &saveptr2);
            if (tok) {
                cmd.input_file = tok;
            }
        } else if (strcmp(tok, ">") == 0) {
            tok = strtok_r(nullptr, " \t", &saveptr2);
            if (tok) {
                cmd.output_file = tok;
                cmd.append_output = false; // Overwrite
            }
        } else if (strcmp(tok, ">>") == 0) {
            tok = strtok_r(nullptr, " \t", &saveptr2);
            if (tok) {
                cmd.output_file = tok;
                cmd.append_output = true; // Append
            }
        } else {
            // Regular command or argument
            if (isCommand) {
                cmd.name = tok;
                isCommand = false;
            } else {
                cmd.args.push_back(tok);
            }
        }

        tok = strtok_r(nullptr, " \t", &saveptr2);
    }

    free(line);
}


// Print parseInfo (for debugging)
void print_info(const cmdlist &info) {
    for (const auto &comm : info.commands) {
        cout << "Command: " << comm.name << "\n";
        cout << "Args: ";
        for (const auto &arg : comm.args) {
            cout << arg << " ";
        }
        cout << "\nInput File: " << comm.input_file << "\n";
        cout << "Output File: " << comm.output_file << "\n";
        cout << "Append: " << (comm.append_output ? "Yes" : "No") << "\n";
        cout << "Background: " << (comm.background ? "Yes" : "No") << "\n\n";
    }
}