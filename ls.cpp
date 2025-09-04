#include "header.h"
using namespace std;

void printPermissions(mode_t mode) {
    cout << ((S_ISDIR(mode)) ? 'd' : '-');
    cout << ((mode & S_IRUSR) ? 'r' : '-');
    cout << ((mode & S_IWUSR) ? 'w' : '-');
    cout << ((mode & S_IXUSR) ? 'x' : '-');
    cout << ((mode & S_IRGRP) ? 'r' : '-');
    cout << ((mode & S_IWGRP) ? 'w' : '-');
    cout << ((mode & S_IXGRP) ? 'x' : '-');
    cout << ((mode & S_IROTH) ? 'r' : '-');
    cout << ((mode & S_IWOTH) ? 'w' : '-');
    cout << ((mode & S_IXOTH) ? 'x' : '-');
}

void printLongListing(const string &path, const struct stat &info) {
    cout << ((S_ISDIR(info.st_mode)) ? 'd' : '-');
    cout << ((info.st_mode & S_IRUSR) ? 'r' : '-');
    cout << ((info.st_mode & S_IWUSR) ? 'w' : '-');
    cout << ((info.st_mode & S_IXUSR) ? 'x' : '-');
    cout << ((info.st_mode & S_IRGRP) ? 'r' : '-');
    cout << ((info.st_mode & S_IWGRP) ? 'w' : '-');
    cout << ((info.st_mode & S_IXGRP) ? 'x' : '-');
    cout << ((info.st_mode & S_IROTH) ? 'r' : '-');
    cout << ((info.st_mode & S_IWOTH) ? 'w' : '-');
    cout << ((info.st_mode & S_IXOTH) ? 'x' : '-');
    cout << " " << info.st_nlink;

    struct passwd *pw = getpwuid(info.st_uid); // get user info
    struct group  *gr = getgrgid(info.st_gid); // get group info
    cout << " " << (pw ? pw->pw_name : to_string(info.st_uid));
    cout << " " << (gr ? gr->gr_name : to_string(info.st_gid));

    cout << " " << info.st_size;

    char timebuf[80]; // last modification time
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", localtime(&info.st_mtime));
    cout << " " << timebuf;

    cout << " " << path << endl;
}

void listFile(const string &filePath, bool longList) {
    struct stat pathStat;
    if (stat(filePath.c_str(), &pathStat) == -1) {
        perror(("ls: cannot access " + filePath).c_str());
        return;
    }

    if (longList) {
        printLongListing(filePath, pathStat);
    } else {
        cout << filePath << endl;
    }
}

void listDirectory(const string &dirPath, bool listAll, bool longList) {
    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        perror(("ls: cannot open directory " + dirPath).c_str());
        return;
    }

    struct dirent *entry;
    vector<string> names;

    while ((entry = readdir(dir)) != nullptr) {
        string name = entry->d_name;
        if (!listAll && name[0] == '.')
            continue;
        names.push_back(name);
    }
    closedir(dir);

    sort(names.begin(), names.end());

    for (const auto &name : names) {
        string fullPath = dirPath + "/" + name;

        if (longList) {
            struct stat fileStat;
            if (stat(fullPath.c_str(), &fileStat) == -1) {
                perror(("ls: cannot access " + fullPath).c_str());
                continue;
            }
            printLongListing(name, fileStat);
        } else {
            cout << name << endl;
        }
    }
}

void ls(const command &cmd) {
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

        dup2(fd_out, STDOUT_FILENO);
        close(fd_out);
    }
    
    // START OF ORIGINAL LS LOGIC
    bool listAll = false;
    bool longList = false;
    vector<string> targets;

    for (const auto &arg : cmd.args) {
        if (arg == "-a") listAll = true;
        else if (arg == "-l") longList = true;
        else if (arg == "-la" || arg == "-al") listAll = longList = true;
        else if (!arg.empty() && arg[0] == '~') {
            const char* home = getenv("HOME");
            if (home == NULL) {
            cerr << "ls: HOME not set\n";
            continue;
        }
        // Replace ~ with actual home directory
        string expanded = string(home) + arg.substr(1);
        targets.push_back(expanded);
        }
        else targets.push_back(arg);
    }

    if (targets.empty()) {
        targets.push_back(".");
    }

    for (size_t i = 0; i < targets.size(); i++) {
        struct stat pathStat;
        if (stat(targets[i].c_str(), &pathStat) == -1) {
            perror(("ls: cannot access " + targets[i]).c_str());
            continue;
        }

        if (S_ISDIR(pathStat.st_mode)) {
            if (targets.size() > 1) {
                cout << targets[i] << ":" << endl;
            }
            listDirectory(targets[i], listAll, longList);
            if (i != targets.size() - 1) cout << endl;
        } else {
            listFile(targets[i], longList);
        }
    }
    // END OF ORIGINAL LS LOGIC

    if (!cmd.output_file.empty()) {
        dup2(original_stdout, STDOUT_FILENO);
    }
    close(original_stdout);
}