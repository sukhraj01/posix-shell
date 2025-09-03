#include "header.h"

using namespace std;

void pinfo(const command &cmd) {
    pid_t target_pid;

    if (cmd.args.empty()) {
        target_pid = getpid();
    } else {
        try {
            target_pid = stoi(cmd.args[0]);
        } catch (const invalid_argument& e) {
            cerr << "Invalid PID: " << cmd.args[0] << endl;
            return;
        }
    }

    // --- Step 1: Get Executable Path ---
    char path_buffer[PROC_PIDPATHINFO_MAXSIZE];
    int ret = proc_pidpath(target_pid, path_buffer, sizeof(path_buffer));

    string exe_path = "Executable path not found.";
    if (ret > 0) {
        exe_path = path_buffer;
    }

    // --- Step 2: Get Process Info ---
    struct proc_taskallinfo task_info;
    ret = proc_pidinfo(target_pid, PROC_PIDTASKALLINFO, 0, &task_info, sizeof(task_info));

    if (ret <= 0) {
        cerr << "Error: Process with PID " << target_pid << " not found." << endl;
        return;
    }

    // Map macOS status (pbsd.pbi_status) to assignment codes
    char status_char = 'S'; // Default to Sleeping
    switch (task_info.pbsd.pbi_status) {
        case 1: status_char = 'R'; break;
        case 2: status_char = 'S'; break;
        case 3: status_char = 'T'; break;
        case 4: status_char = 'Z'; break;
        default: status_char = 'S'; break;
    }

    string status(1, status_char);

    // Foreground check
    pid_t tty_pgid = tcgetpgrp(STDIN_FILENO);
    if (tty_pgid != -1 && tty_pgid == getpgid(target_pid)) {
        status += "+";
    }

    long long virtual_memory = task_info.ptinfo.pti_virtual_size;

    // --- Step 3: Print the Information ---
    cout << "Process Status -- " << status << endl;
    cout << "Memory -- " << virtual_memory / 1024 << " {Virtual Memory}" << endl;
    cout << "Executable Path -- " << exe_path << endl;
}
