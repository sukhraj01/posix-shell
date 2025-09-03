#include "header.h"

using namespace std;

// ---- internal state ----
vector<command> g_hist;
const int HISTORY_LIMIT = 20;

// Load history from file into vector<command>
void loadHistory() {
    g_hist.clear();
    string path = root + "/.my_shell_history";

    FILE *fp = fopen(path.c_str(), "r");
    if (!fp) return;  // file doesn't exist yet, nothing to load

    char buffer[4096];
    while (fgets(buffer, sizeof(buffer), fp)) {
        size_t len = strlen(buffer);
        // strip newline characters
        while (len && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r')) buffer[--len] = '\0';
        if (len == 0) continue;

        auto parsed = parser(string(buffer));
        if (!parsed.commands.empty()) {
            g_hist.push_back(parsed.commands[0]);
            if (g_hist.size() > HISTORY_LIMIT)
                g_hist.erase(g_hist.begin());
        }
    }
    fclose(fp);
}

void flushHistoryToFile() {
    string path = root + "/.my_shell_history";
    FILE *fp = fopen(path.c_str(), "w"); // overwrite file
    if (!fp) {
        perror("history: cannot write to file");
        return;
    }

    for (const command &c : g_hist) {
        string line = c.name;
        for (const string &arg : c.args) line += " " + arg;
        fprintf(fp, "%s\n", line.c_str());
    }
    fclose(fp);
}


void recordHistory(const command &cmd) {
    if (cmd.name.empty()) return;

    // maintain memory limit
    if (g_hist.size() == HISTORY_LIMIT) g_hist.erase(g_hist.begin());
    g_hist.push_back(cmd);

    // flush to file to maintain last HISTORY_LIMIT commands
    flushHistoryToFile();
}

// Record a new command into memory
// void recordHistory(const command &cmd) {
//     if (cmd.name.empty()) return;

//     if (g_hist.size() == HISTORY_LIMIT) g_hist.erase(g_hist.begin());
//     g_hist.push_back(cmd);

//     string path = root + "/.my_shell_history";
//     FILE *fp = fopen(path.c_str(), "a"); // append mode
//     if (!fp) return;

//     string line = cmd.name;
 
//     for (const string &arg : cmd.args) {
//         line += " " + arg;
//     }
//     fprintf(fp, "%s\n", line.c_str());

//     fclose(fp);
// }

void history(const command &cmd) {
    int toShow = 10; // default

    if (cmd.args.size() > 1) {
        fprintf(stderr, "history: invalid arguments\n");
        return;
    }

    if (!cmd.args.empty()) {
        string arg = cmd.args[0];
        if (arg[0] == '-') arg = arg.substr(1); // remove leading '-'
        for (char c : arg) {
            if (!isdigit(c)) {
                fprintf(stderr, "history: invalid number: %s\n", cmd.args[0].c_str());
                return;
            }
        }
        toShow = stoi(arg);
        if (toShow < 0) toShow = 0;
        if (toShow > HISTORY_LIMIT) toShow = HISTORY_LIMIT;
    }

    int n = g_hist.size();
    int start = n - toShow;
    if (start < 0) start = 0;

    for (int i = start; i < n; ++i) {
        string line = g_hist[i].name;
        for (const string &arg : g_hist[i].args) line += " " + arg;
        printf("%s\n", line.c_str());
    }
}


// Built-in: history [N]
// void history(const command &cmd) {
//     int toShow = 10; // default number of commands

//     // validate arguments
//     if (cmd.args.size() > 1) {
//         fprintf(stderr, "history: invalid arguments\n");
//         return;
//     }
//     if (!cmd.args.empty()) {
//         string arg = cmd.args[0];
//         for (char c : arg) {
//             if (!isdigit(c)) {
//                 fprintf(stderr, "history: invalid number: %s\n", arg.c_str());
//                 return;
//             }
//         }
//         toShow = stoi(arg);
//         if (toShow < 0) toShow = 0;
//         if (toShow > HISTORY_LIMIT) toShow = HISTORY_LIMIT;
//     }

//     // calculate start index to show last `toShow` commands
//     int n = g_hist.size();
//     int start = n - toShow;
//     if (start < 0) start = 0;

//     // print last `toShow` commands from g_hist
//     for (int i = start; i < n; ++i) {
//         string line = g_hist[i].name;
//         for (const string &arg : g_hist[i].args) line += " " + arg;
//         printf("%s\n", line.c_str());
//     }
// }
