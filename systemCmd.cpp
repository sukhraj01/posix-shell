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
        // Convert command and arguments to C-style char* array for execvp
        vector<char*> args;
        args.push_back(const_cast<char*>(cmd.name.c_str()));
        for (const auto& arg : cmd.args) {
            args.push_back(const_cast<char*>(arg.c_str()));
        }
        args.push_back(nullptr); // execvp requires a nullptr at the end

        execvp(args[0], args.data());// arg[0] is cmmand name 

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