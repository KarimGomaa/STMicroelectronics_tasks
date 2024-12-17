# ⚙️ Real-Time Signal Handling in C

This project demonstrates how to handle **real-time signals** (`SIGRTMIN` and `SIGRTMIN+1`) in a C program. It uses custom signal handlers to terminate the process in different ways:  
- ✅ Termination without a core dump.  
- 💾 Termination with a core dump.



## ✨ Features

- 🛠 **Signal Handling**: Implements handlers for `SIGRTMIN` and `SIGRTMIN+1`.
- ❌ **Process Termination**:
  - Graceful termination with `SIGINT` when receiving signal `SIGRTMIN`.
  - Core dump generation with `SIGABRT` when receiving signal `SIGRTMIN+1`.
- 📡 **Real-Time Signal Control**: Demonstrates usage of real-time signals for process control.



## 📋 Prerequisites

- 🐧 A Linux-based operating system.
- 🧰 GCC compiler for C programs.



## 🛠 Installation

1. Clone this repository or copy the code:
   ```bash
   git clone https://github.com/KarimGomaa/STMicroelectronics_tasks
   cd Tasks/Realtime_signal_task


2. Compile the program using gcc:

```
gcc -o signal_handler signal_handler.c
```



## 🚀 Usage

1. Run the program:
```
./signal_handler
```
The program will display:

```
You can send from another terminal using kill command
kill -34 <PID> to terminate
kill -35 <PID> to terminate and generate core dump
```
2. Send signals from another terminal:

    - ✅ To terminate the process with SIGINT:

    ```
    kill -34 <PID>
    ```

    - 💾 To terminate the process and generate a core dump:

    ```
    kill -35 <PID>
    ```
    Replace `<PID>` with the process ID of the running program, which can be obtained using ps     or from the program's output.

3. Core Dump Setup:
If the core dump file is not generated, follow these steps:

    - 📂 Allow unlimited core dump size:
    ```
    ulimit -c unlimited
    ```
    
    - ⚙️ Disable the apport service (Ubuntu/Debian):
    ```
    sudo service apport stop
    ```
This ensures the core dump file is created in the working directory.


## 📝 Code Explanation

1. 🔧 Signal Handlers:

    - handler1: Handles signal SIGRTMIN and terminates the process with SIGINT.
    - handler2: Handles signal SIGRTMIN+1, provides core dump instructions, and terminates the process with SIGABRT.

2. ▶️ Main Function:

    - Registers the handlers for SIGRTMIN and SIGRTMIN+1 using the signal() function.
    - Continuously waits for signals using an infinite loop.
    

##  Example Output

#### 🟢 Case 1: Sending Signal SIGRTMIN (kill -34 <PID>)
```
You send Real time signal 0 process terminate
```

#### 🔴 Case 2: Sending Signal SIGRTMIN+1 (kill -35 <PID>)
```
if the core dump is not generated do this 
ulimit -c unlimited 
sudo service apport stop ( 3shan ytl3lk f nfs mkan al file) 
You send Real time signal 1 process terminate with core dump file
```
## See the illustration Video in the link below:
    - [View Video](https://drive.google.com/file/d/1YA4C6f7KpSSTrAPUZl2BUG02NUEegSnK/view?usp=sharing)

## 🙏 Acknowledgments
This project was developed in C. Contributions and improvements are welcome! 🚀




