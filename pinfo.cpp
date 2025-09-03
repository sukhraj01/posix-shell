#include "header.h"

using namespace std;

void pinfo(const command &cmd) {
    // START I/O REDIRECTION LOGIC
    int original_stdout = dup(STDOUT_FILENO);
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

        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }
    // END I/O REDIRECTION LOGIC

    // START OF ORIGINAL PINFO LOGIC
    pid_t target_pid;
    if (cmd.args.empty()) {
        target_pid = getpid();
    } else {
        try {
            target_pid = stoi(cmd.args[0]);
        } catch (const invalid_argument& e) {
            cerr << "Invalid PID: " << cmd.args[0] << endl;
            // Restore stdout before returning on error
            if (!cmd.output_file.empty()) {
                dup2(original_stdout, STDOUT_FILENO);
            }
            close(original_stdout);
            return;
        }
    }

    char path_buffer[PROC_PIDPATHINFO_MAXSIZE];
    int ret = proc_pidpath(target_pid, path_buffer, sizeof(path_buffer));

    string exe_path = "Executable path not found.";
    if (ret > 0) {
        exe_path = path_buffer;
    }

    struct proc_taskallinfo task_info;
    ret = proc_pidinfo(target_pid, PROC_PIDTASKALLINFO, 0, &task_info, sizeof(task_info));

    if (ret <= 0) {
        cerr << "Error: Process with PID " << target_pid << " not found." << endl;
        // Restore stdout before returning on error
        if (!cmd.output_file.empty()) {
            dup2(original_stdout, STDOUT_FILENO);
        }
        close(original_stdout);
        return;
    }

    char status_char = 'S';
    switch (task_info.pbsd.pbi_status) {
        case 1: status_char = 'R'; break;
        case 2: status_char = 'S'; break;
        case 3: status_char = 'T'; break;
        case 4: status_char = 'Z'; break;
        default: status_char = 'S'; break;
    }

    string status(1, status_char);

    pid_t tty_pgid = tcgetpgrp(STDIN_FILENO);
    if (tty_pgid != -1 && tty_pgid == getpgid(target_pid)) {
        status += "+";
    }

    long long virtual_memory = task_info.ptinfo.pti_virtual_size;

    cout << "Process Status -- " << status << endl;
    cout << "Memory -- " << virtual_memory / 1024 << " {Virtual Memory}" << endl;
    cout << "Executable Path -- " << exe_path << endl;
    // END OF ORIGINAL PINFO LOGIC

    // START RESTORATION LOGIC
    if (!cmd.output_file.empty()) {
        dup2(original_stdout, STDOUT_FILENO);
    }
    close(original_stdout);
    // END RESTORATION LOGIC
}

// void pinfo(const command &cmd) {
//     pid_t target_pid;

//     if (cmd.args.empty()) {
//         target_pid = getpid();
//     } else {
//         try {
//             target_pid = stoi(cmd.args[0]);
//         } catch (const invalid_argument& e) {
//             cerr << "Invalid PID: " << cmd.args[0] << endl;
//             return;
//         }
//     }

//     // --- Step 1: Get Executable Path ---
//     char path_buffer[PROC_PIDPATHINFO_MAXSIZE];
//     int ret = proc_pidpath(target_pid, path_buffer, sizeof(path_buffer));

//     string exe_path = "Executable path not found.";
//     if (ret > 0) {
//         exe_path = path_buffer;
//     }

//     // --- Step 2: Get Process Info ---
//     struct proc_taskallinfo task_info;
//     ret = proc_pidinfo(target_pid, PROC_PIDTASKALLINFO, 0, &task_info, sizeof(task_info));

//     if (ret <= 0) {
//         cerr << "Error: Process with PID " << target_pid << " not found." << endl;
//         return;
//     }

//     // Map macOS status (pbsd.pbi_status) to assignment codes
//     char status_char = 'S'; // Default to Sleeping
//     switch (task_info.pbsd.pbi_status) {
//         case 1: status_char = 'R'; break;
//         case 2: status_char = 'S'; break;
//         case 3: status_char = 'T'; break;
//         case 4: status_char = 'Z'; break;
//         default: status_char = 'S'; break;
//     }

//     string status(1, status_char);

//     // Foreground check
//     pid_t tty_pgid = tcgetpgrp(STDIN_FILENO);
//     if (tty_pgid != -1 && tty_pgid == getpgid(target_pid)) {
//         status += "+";
//     }

//     long long virtual_memory = task_info.ptinfo.pti_virtual_size;

//     // --- Step 3: Print the Information ---
//     cout << "Process Status -- " << status << endl;
//     cout << "Memory -- " << virtual_memory / 1024 << " {Virtual Memory}" << endl;
//     cout << "Executable Path -- " << exe_path << endl;
// }
