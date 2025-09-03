// Library headers
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h> // for open() flags
#include <pwd.h>
#include <limits.h>
#include <dirent.h>
#include <grp.h>
#include <iomanip>
#include <ctime>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <signal.h>
#include <libproc.h>
#include <sys/proc_info.h>
#include <termios.h>

// Custom headers
#include "struct.h"
#include "parser.h"
#include "builtins.h"
#include "shellinfo.h"
#include "cd.h"
#include "pwd.h"
#include "echo.h"
#include "ls.h"
#include "search.h"
#include "history.h"
#include "zombie.h"
#include "systemCmd.h"
#include "pinfo.h"