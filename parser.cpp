#include "header.h"

using namespace std;

// Forward declarations to maintain structure
void cmdParser(const string& singleCmd, command& cmd);
void print_info(const vector<cmdlist> &pipelines);

// The main parser function is now updated to handle both ';' and '|'
// It returns a vector of cmdlist, where each cmdlist represents a single pipeline.
vector<cmdlist> parser(const string& cmdLine) {
    vector<cmdlist> all_pipelines;

    char *semicolon_token = strdup(cmdLine.c_str());
    char *semicolon_saveptr;
    char *semicolon_cmd = strtok_r(semicolon_token, ";", &semicolon_saveptr);

    while (semicolon_cmd) {
        cmdlist current_pipeline;
        
        char *pipe_token = strdup(semicolon_cmd);
        char *pipe_saveptr;
        char *pipe_cmd = strtok_r(pipe_token, "|", &pipe_saveptr);
        
        while (pipe_cmd) {
            string single_cmd_str(pipe_cmd);
            command cmd;
            cmdParser(single_cmd_str, cmd);
            
            if (!cmd.name.empty()) {
                current_pipeline.commands.push_back(cmd);
            }
            
            pipe_cmd = strtok_r(NULL, "|", &pipe_saveptr);
        }
        
        free(pipe_token);
        
        if (!current_pipeline.commands.empty()) {
            all_pipelines.push_back(current_pipeline);
        }
        
        semicolon_cmd = strtok_r(NULL, ";", &semicolon_saveptr);
    }
    
    free(semicolon_token);
    return all_pipelines;
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
void print_info(const vector<cmdlist> &pipelines) {
    for (size_t i = 0; i < pipelines.size(); ++i) {
        cout << "--- Pipeline " << i + 1 << " ---\n";
        for (const auto &comm : pipelines[i].commands) {
            cout << "  Command: " << comm.name << "\n";
            cout << "  Args: ";
            for (const auto &arg : comm.args) {
                cout << arg << " ";
            }
            cout << "\n  Input File: " << comm.input_file << "\n";
            cout << "  Output File: " << comm.output_file << "\n";
            cout << "  Append: " << (comm.append_output ? "Yes" : "No") << "\n";
            cout << "  Background: " << (comm.background ? "Yes" : "No") << "\n\n";
        }
    }
}