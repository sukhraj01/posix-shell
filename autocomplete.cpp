#include "header.h"

using namespace std;


// This is our hard-coded list of common commands
static const char* common_commands[] = {
    "ls", "cd", "pwd", "echo", "history", "pinfo", "cat", "grep", "sort", "wc", "touch", "exit", "clear", "sort", nullptr
};

static vector<string> get_matches(const string& text) {
    vector<string> matches;
    
    // Search for common commands
    for (int i = 0; common_commands[i]; ++i) {
        if (string(common_commands[i]).rfind(text, 0) == 0) {
            matches.push_back(common_commands[i]);
        }
    }
    
    // Search files/directories in the current path
    DIR* dir = opendir(".");
    if (dir) {
        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            string name = entry->d_name;
            if (name.rfind(text, 0) == 0) {
                matches.push_back(name);
            }
        }
        closedir(dir);
    }
    
    sort(matches.begin(), matches.end());
    return matches;
}


static char** autocomplete_callback(const char* text, int start, int end) {
    if (start != 0) {
        return nullptr;
    }
    vector<string> matches = get_matches(text);
    
    if (matches.empty()) {
        return nullptr;
    }
    (void)end;
    // Convert matches to a C-style array for readline
    char** completion_matches = new char*[matches.size() + 1];
    for(size_t i = 0; i < matches.size(); ++i) {
        completion_matches[i] = strdup(matches[i].c_str());
    }
    completion_matches[matches.size()] = nullptr;

    return completion_matches;
}


void initialize_readline() {
    rl_attempted_completion_function = autocomplete_callback;
}