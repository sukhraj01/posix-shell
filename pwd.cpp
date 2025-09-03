#include "header.h"

using namespace std;

void pwd(const command& cmd) {
    if (cmd.args.size() > 0) {
        cerr << "pwd: too many arguments\n";
        return;
    }
    char cwd[PATH_MAX];
    string dir;
     if (getcwd(cwd, sizeof(cwd)) != NULL) {
        dir = cwd;
    } else {
        dir = "~";
    }
    cout << dir << endl;
}