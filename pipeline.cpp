#include "header.h"

using namespace std;

using namespace std;

void executePipeline(const cmdlist &pipeline) {
    if (pipeline.commands.empty()) return;

    int num_commands = pipeline.commands.size();
    int in_fd = STDIN_FILENO; // Initial input is stdin
    int fd[2]; // Pipe file descriptors

    // Loop through all but the last command
    for (int i = 0; i < num_commands; ++i) {
        const command& cmd = pipeline.commands[i];
        
        // If it's not the last command, create a pipe
        if (i < num_commands - 1) {
            if (pipe(fd) == -1) {
                perror("pipe failed");
                return;
            }
        }

        pid_t pid = fork();
        if (pid == -1) {
            perror("fork failed");
            return;
        }

        if (pid == 0) { // Child process
            // Set up input redirection
            if (in_fd != STDIN_FILENO) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            } else if (i == 0 && !cmd.input_file.empty()) {
                // Apply file input redirection only to the first command
                int fd_in = open(cmd.input_file.c_str(), O_RDONLY);
                if (fd_in < 0) { perror("input file error"); exit(1); }
                dup2(fd_in, STDIN_FILENO);
                close(fd_in);
            }

            // Set up output redirection
            if (i < num_commands - 1) {
                dup2(fd[1], STDOUT_FILENO);
                close(fd[0]);
                close(fd[1]);
            } else if (i == num_commands - 1 && !cmd.output_file.empty()) {
                // Apply file output redirection only to the last command
                int fd_out = open(cmd.output_file.c_str(), O_WRONLY | O_CREAT | (cmd.append_output ? O_APPEND : O_TRUNC), 0644);
                if (fd_out < 0) { perror("output file error"); exit(1); }
                dup2(fd_out, STDOUT_FILENO);
                close(fd_out);
            }
            
            // Execute the command
            // ... (rest of your original code for execvp) ...
            vector<char*> args;
            args.push_back(const_cast<char*>(cmd.name.c_str()));
            for (const auto& arg : cmd.args) {
                args.push_back(const_cast<char*>(arg.c_str()));
            }
            args.push_back(nullptr);

            execvp(args[0], args.data());
            perror("execvp failed");
            exit(1);
        } else { // Parent process
            // Close the parent's copy of the pipe ends
            if (in_fd != STDIN_FILENO) {
                close(in_fd);
            }
            if (i < num_commands - 1) {
                close(fd[1]);
                in_fd = fd[0];
            }
        }
    }
    
    // Wait for all child processes to finish
    int status;
    for (int i = 0; i < num_commands; ++i) {
        wait(&status);
    }
}