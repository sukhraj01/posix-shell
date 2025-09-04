# Advanced Operating Systems
## Assignment 2: POSIX Shell Implementation

Submitted By: Sukhraj Singh | 2025202009

---

### 📌 Task
Implement a shell that supports:
- Semi-colon (`;`) separated commands (use `strtok` for tokenizing).
- Background execution using `&`, printing the PID of created process.
- Modular design in **C++**, managing system/user-defined executables.
- Foreground & background process handling, input/output redirection, and pipes.

---

### 🔑 Features Implemented
- **Custom Prompt:** `<username@system:current_dir>`  
- **Built-ins:** `cd`, `pwd`, `echo`  
- **ls:** Supports `-a` and `-l` flags  
- **System Commands:** Run in foreground/background with PID display  
- **pinfo:** Show process information (PID, status, memory, executable path)  
- **search:** Recursive file/folder search  
- **Redirection:** `<`, `>`, `>>` supported  
- **Pipelines:** Multiple pipes supported (`cmd1 | cmd2 | cmd3`)  
- **Redirection + Pipeline:** Works together  
- **Autocomplete (TAB):** Suggests commands/files in current directory  
- **History:** Stores up to 20 commands, persists across sessions  

---

### ⚠️ Disclaimer (macOS vs Linux)
This shell is **developed and tested on macOS**.  
Since macOS does not provide `/proc/<pid>` like Linux, the **pinfo** command is implemented differently:
- **On Linux:** Reads process info from `/proc`
- **On macOS:** Uses system calls like `ps`

👉 If you try running the macOS version of `pinfo` on Linux, results will differ.

---

### 📂 Directory Structure
```
<roll_number>_Assignment2/
├── README.md          # Project description (this file)
├── makefile           # Build instructions
├── header.h           # Common declarations
├── shell.cpp          # Main shell loop
├── cd.cpp             # Implementation of cd
├── ls.cpp             # Implementation of ls
├── pinfo.cpp          # Implementation of pinfo
├── history.cpp        # Command history tracking
├── autocomplete.cpp   # TAB-based autocomplete
└── other files...
```

---

### ⚡ Compilation & Run
```bash
make        # compile using makefile
./shell     # start the custom shell
```

---

## 📝 Example Usage
```bash
<Name@MacBook:~> ls -a -l
<Name@MacBook:~> cd test
<Name@MacBook:~/test> echo hello > out.txt
<Name@MacBook:~/test> cat < out.txt | wc -l
<Name@MacBook:~/test> gedit &
<Name@MacBook:~/test> pinfo
<Name@MacBook:~/test> search file.txt
<Name@MacBook:~/test> history
```


---

### ✅ Notes
- Written entirely in **C++** (no `system()` or `popen()`).  
- Handles random spaces/tabs in commands.  
- Persistent history across sessions.  
- Errors handled gracefully with clear messages.  
- Modular design for maintainability.  
