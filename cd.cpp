#include "header.h"

using namespace std;

void cd(const command& cmd) {
    if (cmd.args.size() > 1) {
        cerr << "cd: too many arguments\n";
        return;
    }
    string currDir;
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        currDir = cwd;
    }
    string destDir;
    if (cmd.args.size() == 0) {
        destDir = root;
    } else if (cmd.args[0] == ".") { // current directory
        return; 
    } else if (cmd.args[0] == "..") {
        destDir = "..";           // parent directory
    } else if (cmd.args[0] == "-") {
        if (prevDir.empty()) {
        cerr << "cd: OLDPWD not set\n";
        return;
        }
        destDir = prevDir;   // go to previous directory
    } else if (cmd.args[0] == "~") {
        const char* home = getenv("HOME");       // get user's home dir
        if (home == NULL) {
            cerr << "cd: HOME not set\n";
            return;
        }
        destDir = home;
    } 
    else {
        destDir = cmd.args[0];    // specific path
    }

    if (chdir(destDir.c_str()) != 0) {
        perror("cd failed");
    } else {
        prevDir = currDir;
    }
}