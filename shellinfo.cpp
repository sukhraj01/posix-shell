#include "header.h"

using namespace std;

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 1024
#endif

string root;
string shellHome;  
string currWorkDir;
string prevDir = "";  // for cd -

bool rootflag = true;

void initShellHome() {
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) != nullptr) {
        shellHome = cwd;   // the directory from which shell is invoked
        if (rootflag) {
            root = shellHome;
            rootflag = false;
        }
    } else {
        perror("getcwd failed");
        shellHome = "";
    }
    loadHistory(); 
}

string shellInfo() {
    // 1. Username 
    uid_t uid = getuid();               // return user id
    struct passwd *pw = getpwuid(uid);  // returns pointer containing user info 
    string username = pw->pw_name; 

    // 2. System name
    char systemname[HOST_NAME_MAX];
    if (gethostname(systemname, sizeof(systemname)) != 0) { 
       perror("gethostname failed"); // if hostname fails
    }

    // 3. cwd
    char cwd[PATH_MAX];
    string dir;
     if (getcwd(cwd, sizeof(cwd)) != NULL) {
        dir = cwd;
        if (!shellHome.empty() && dir.find(shellHome) == 0) {
            dir.replace(0, shellHome.length(), "~");
        }
    } else {
        dir = "~";
    }

    string shellInfo = username + "@" + systemname + ": " + dir + " > ";
    return shellInfo;

}

/*
In this we need username, system name and cwd and display it like
<Name@UBUNTU:~/test>

struct passwd {
    char *pw_name;   // username
    char *pw_passwd; 
    uid_t pw_uid;    
    gid_t pw_gid;    // numeric group ID
    char *pw_gecos;  // user info (full name, etc.)
    char *pw_dir;    // home directory
    char *pw_shell;  // login shell
};

* we dont use string fior string hostname and cwd becaue the gethostname and cwd uses the fllowing parameters
int gethostname(char *name, size_t len); 
int getcwd(cwd, sizeof(cwd)
*/