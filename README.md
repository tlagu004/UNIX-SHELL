# UNIX Shell

My project is a custom UNIX Shell interface implemented in C. The program mimics a standard command-line interpreter (like bash) by accepting user commands and executing them in separate processes.

<!-- ## Key Features
* **Process Management**: Uses `fork()`, `execvp()`, and `wait()` to run commands.
* **Background Execution**: Supports the `&` suffix to run processes concurrently.
* **History Feature**: Use `!!` to re-run the most recent command.
* **I/O Redirection**: 
    * `>`: Redirects output to a file (e.g., `ls > out.txt`).
    * `<`: Redirects input from a file (e.g., `sort < in.txt`).
* **Pipes**: Supports IPC between two commands using `|` (e.g., `ls -l | less`). -->


## Features Implemented in this Shell

### Core Functionality
- **Process Creation and Execution**
- **Command Parsing**
- **History Feature**
- **Input/Output Redirection**
- **Pipes**

### Additional Features
- **Job Control**
- **Aliases**
- **Command Completion**
- **Signal Handling**



## 🚀 How to Run

 **Compile**: Use the provided Makefile or gcc.
```bash
gcc shell.c -o osh
```
 **Execute**:
```bash
./osh
```
**Exit**: Type `exit` to terminate the shell.

## 📺 Demo Video
[Watch the Demo Video](https://youtu.be/ISblj3dPUvM)