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

        // The parser now returns a vector of pipelines, so we must access it differently
        auto parsed_pipelines = parser(string(buffer));
        
        // Ensure the parser returned at least one command in the first pipeline
        if (!parsed_pipelines.empty() && !parsed_pipelines[0].commands.empty()) {
            g_hist.push_back(parsed_pipelines[0].commands[0]);
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

void history(const command &cmd) {
    // START I/O REDIRECTION LOGIC
    int original_stdout = dup(STDOUT_FILENO); // Save original stdout

    if (!cmd.output_file.empty()) {
        int fd_out;
        if (cmd.append_output) {
            fd_out = open(cmd.output_file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
        } else {
            fd_out = open(cmd.output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
        }
        
        if (fd_out < 0) {
            perror("Error opening output file");
            close(original_stdout);
            return;
        }

        dup2(fd_out, STDOUT_FILENO); // Redirect stdout to the file
        close(fd_out);
    }
    // END I/O REDIRECTION LOGIC

    // START OF ORIGINAL HISTORY LOGIC
    int toShow = 10; // default

    if (cmd.args.size() > 1) {
        fprintf(stderr, "history: invalid arguments\n");
    } else {
        if (!cmd.args.empty()) {
            string arg = cmd.args[0];
            if (arg[0] == '-') arg = arg.substr(1); // remove leading '-'
            for (char c : arg) {
                if (!isdigit(c)) {
                    fprintf(stderr, "history: invalid number: %s\n", cmd.args[0].c_str());
                    // This is important: return only if there are no redirection files to close
                    if (cmd.output_file.empty()) {
                        close(original_stdout);
                    }
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
    // END OF ORIGINAL HISTORY LOGIC

    // START RESTORATION LOGIC
    if (!cmd.output_file.empty()) {
        dup2(original_stdout, STDOUT_FILENO);
    }
    close(original_stdout);
    // END RESTORATION LOGIC
}