# ⚙️  Non-Blocking Child Process Reaper (Avoiding Zombie Processes)

## 📌 Overview
This C program demonstrates how to handle terminated child processes **without blocking** using the `SIGCHLD` signal. It prevents zombie processes by **reaping** child processes as soon as they exit.

### 🔹 Key Features
✅ Uses **`SIGCHLD`** signal to detect child process termination  
✅ Implements **non-blocking** `waitpid` to clean up exited children  
✅ Prevents **zombie processes**  
✅ Demonstrates **forking multiple child processes**  
✅ Parent process **remains alive** to handle future terminations  

---

## 📜 How It Works
1. The parent process forks **five child processes**.
2. Each child process:
   - Sleeps for **30 seconds**.
   - Exits with a unique **exit status**.
3. The parent process:
   - **Registers** a signal handler for `SIGCHLD`.
   - Uses `waitpid(-1, &status, WNOHANG)` inside the handler to clean up exited children **without blocking**.
   - Stays alive using an **infinite loop**.

---

## 📂 Code Explanation

### 🏗️ **Signal Handler (`sigchld_handler`)**
- **Triggered when a child process exits.**
- Uses `waitpid` with `WNOHANG` to **reap multiple children at once**.
- Prints:
  - Child **PID**
  - **Exit status** (if exited normally)
  - **Termination signal** (if killed by a signal)

### ⚡ **Main Function (`main`)**
- Registers **`SIGCHLD` handler** using `sigaction`.
- Forks **five child processes**.
- Parent process **remains running** to handle future signals.

---

## 🚀 Compilation & Execution

### 🔹 **Step 1: Compile**
```sh
gcc -o no_dead_child no_dead_child.c
```

### 🔹 Step 2: Run
```
./no_dead_child
```

### 🔹 Step 3: Check Running Processes (Optional)
```
ps aux | grep no_dead_child
```

## 🎯 Expected Output
```
Parent PID: 12345
Child PID: 12346 started
Child PID: 12347 started
Child PID: 12348 started
Child PID: 12349 started
Child PID: 12350 started
Child process 12347 exited normally with status 101
Child process 12349 exited normally with status 103
Child process 12348 exited normally with status 102
Child process 12350 exited normally with status 104
Child process 12346 exited normally with status 100
```

## 🔹 Why This Approach?
✅ Prevents zombie processes using  `waitpid`. 

✅ Non-blocking—parent remains responsive.

✅ Handles multiple children simultaneously.

✅ Robust & scalable for real-world applications.


## 🟢  Video to illustarate
[View Video](https://drive.google.com/file/d/1yBQ9sI2yUyXDd00lbBVHue1z1zHFgBVL/view?usp=sharing)

## 🙏 Acknowledgments
This project was developed in C. Contributions and improvements are welcome! 🚀


