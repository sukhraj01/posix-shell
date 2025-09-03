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
    printPermissions(info.st_mode);
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

    // Just file
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
    bool listAll = false;
    bool longList = false;
    vector<string> targets;

    // Parse args
    for (const auto &arg : cmd.args) {
        if (arg == "-a") listAll = true;
        else if (arg == "-l") longList = true;
        else if (arg == "-la" || arg == "-al") listAll = longList = true;
        else if (!arg.empty() && arg[0] == '~') {
            targets.push_back(root + arg.substr(1));  
        }
        else targets.push_back(arg);
    }

    if (targets.empty()) {
        targets.push_back("."); // default = current dir
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
}
