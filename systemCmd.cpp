#include "header.h"

using namespace std;

void systemCmd(const command &cmd) {
    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return;
    }

    if (pid == 0) { // Child process
        setpgid(0, 0);  // child becomes leader of its own PGID

        // Handle I/O Redirection
        
        // 1. Input Redirection
        if (!cmd.input_file.empty()) {
            int fd_in = open(cmd.input_file.c_str(), O_RDONLY);
            if (fd_in < 0) {
                perror(("Error opening input file: " + cmd.input_file).c_str());
                exit(1);
            }
            dup2(fd_in, STDIN_FILENO); // Redirect stdin to the input file
            close(fd_in);
        }

        // 2. Output Redirection
        if (!cmd.output_file.empty()) {
            int fd_out;
            if (cmd.append_output) {
                // Append mode (>>)
                fd_out = open(cmd.output_file.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            } else {
                // Overwrite mode (>)
                fd_out = open(cmd.output_file.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0644);
            }
            if (fd_out < 0) {
                perror(("Error opening output file: " + cmd.output_file).c_str());
                exit(1);
            }
            dup2(fd_out, STDOUT_FILENO); // Redirect stdout to the output file
            close(fd_out);
        }

        // Convert command and arguments to C-style char* array for execvp
        vector<char*> args;
        args.push_back(const_cast<char*>(cmd.name.c_str()));
        for (const auto& arg : cmd.args) {
            args.push_back(const_cast<char*>(arg.c_str()));
        }
        args.push_back(nullptr); // execvp requires a nullptr at the end

        execvp(args[0], args.data());

        // execvp only returns if an error occurred
        perror("execvp failed");
        exit(1);
    } else { // Parent process (the shell)
        if (cmd.background) {
            // Background process: print PID and continue
            cout << "[" << pid << "]\n";
        } else {
            // Foreground process: wait for the child to finish
            int status;
            do {
                waitpid(pid, &status, 0);
            } while (!WIFEXITED(status) && !WIFSIGNALED(status));
        }
    }
}

