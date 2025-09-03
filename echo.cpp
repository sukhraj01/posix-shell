#include "header.h"

using namespace std;

void echo(const command& cmd) {
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

    // Existing echo logic
    if (cmd.args.empty()) {
        cout << endl;
    } else {
        for (size_t i = 0; i < cmd.args.size(); ++i) {
            cout << cmd.args[i];
            if (i != cmd.args.size() - 1) {
                cout << " ";
            }
        }
        cout << endl;
    }

    // Restore original stdout
    if (!cmd.output_file.empty()) {
        dup2(original_stdout, STDOUT_FILENO);
    }
    close(original_stdout);
}