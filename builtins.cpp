#include "header.h"

using namespace std;

bool isBuiltIn(const command &cmd) {
    string name = cmd.name;
    return (name == "cd" || name == "pwd" || name == "echo"
            || name == "ls" || name == "pinfo" || name == "history"
            || name == "search");
}


void executeBuiltIn(const command &cmd) {
    if (cmd.name == "cd") {
        cd(cmd);
    } else if (cmd.name == "pwd") {
        pwd(cmd);
    } else if (cmd.name == "echo") {
        echo(cmd);
    } else if (cmd.name == "ls") {
        ls(cmd);
    } else if (cmd.name == "search") {
        search(cmd);
    } else if(cmd.name == "history") {
        history(cmd);
    } else if (cmd.name == "pinfo") {
        pinfo(cmd);
    }
    else {
        cout << "Unsupported built-in command\n";
    }
}
