#include "header.h"

using namespace std;

bool recurse(const string &dir, const string &target) {
    DIR *dp = opendir(dir.c_str());
    if (!dp) return false;

    struct dirent *entry;
    while ((entry = readdir(dp)) != nullptr) {
        string name = entry->d_name;

        if (name == "." || name == "..") continue;

        if (name == target) { // match condition
            closedir(dp);
            return true;
        }

        if (entry->d_type == DT_DIR) {  // dfs for directory 
            string subdir = dir + "/" + name;
            if (recurse(subdir, target)) {
                closedir(dp);
                return true;
            }
        }
    }
    closedir(dp);
    return false;
}

void search(const command &cmd) {
    if (cmd.args.empty()) {
        cerr << "search: missing operand\n";
        return;
    }

    string target = cmd.args[0];

    // get current directory
    char cwd[PATH_MAX];
    if (!getcwd(cwd, sizeof(cwd))) {
        perror("getcwd");
        return;
    }

    bool found = recurse(cwd, target);
    cout << (found ? "True" : "False") << "\n";
}
