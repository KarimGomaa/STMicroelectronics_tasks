# 📝 Real-Time Signal-Based Logger

## 📌 Overview
This project implements a **dynamically configurable logger** in C that allows adjusting log levels **in real-time** using **real-time signals (SIGRTMIN)**. 

### 🔹 Features:
✅ Provides **four logging levels**: `ERROR`, `WARNING`, `INFO`, and `DEBUG`  
✅ Supports **dynamic log level control** using real-time signals  
✅ Uses a **separate controller (`set_log_level`)** to send signals  
✅ Displays **PID** for easy signal control  
✅ No **threads** required, purely signal-based  

---

## 🚀 How It Works

### 🏗️ **Logger (`logger.c`)**
- Starts with a **default log level** (`INFO`).
- Listens for **real-time signals** to adjust the log level dynamically.
- Prints messages based on the **current log level**.

### 🎛 **Log Level Control (`set_log_level.c`)**
- Sends **real-time signals (SIGRTMIN + X)** to the `logger` process.
- Allows the user to **increase or decrease the log level dynamically**.

---

## 🔧 Compilation & Execution

### **1️⃣ Compile Both Programs**
```sh
gcc -o logger logger.c
gcc -o set_log_level set_log_level.c
```

### 2️⃣ Run the Logger
```
./logger
```

Expected Output:
```
Logger started. PID: 12345
- Use SIGRTMIN to Disable logger.
- Use SIGRTMIN+1 to set logger to error.
- Use SIGRTMIN+2 to set logger to warning.
- Use SIGRTMIN+3 to set logger to info.
- Use SIGRTMIN+4 to set logger to debug.
----------------------------------------------------
[ERROR] This is an ERROR message.
[WARNING] This is a WARNING message.
[INFO] This is an INFO message.
----------------------------------------------------
```

### 3️⃣ Find the Logger's PID
If you missed it in the output, use:
```
ps aux | grep logger
```

### 4️⃣ Change the Log Level
Use set_log_level with the PID:
```
./set_log_level <PID> disable   # Disable logging
./set_log_level <PID> error     # Set log level to ERROR
./set_log_level <PID> warning   # Set log level to WARNING
./set_log_level <PID> info      # Set log level to INFO
./set_log_level <PID> debug     # Set log level to DEBUG
```

Alternatively, send signals manually:
```
kill -SIGRTMIN <PID>    # Disable logging
kill -SIGRTMIN+1 <PID>  # Set log level to ERROR
kill -SIGRTMIN+2 <PID>  # Set log level to WARNING
kill -SIGRTMIN+3 <PID>  # Set log level to INFO
kill -SIGRTMIN+4 <PID>  # Set log level to DEBUG

```

## 📜 Expected Output
### 🔹 Default Log Level (INFO)
```
[ERROR] This is an ERROR message.
[WARNING] This is a WARNING message.
[INFO] This is an INFO message.
----------------------------------------------------
```
### 🔹 After Running ./set_log_level <PID> debug
```
Log level changed to: DEBUG
[ERROR] This is an ERROR message.
[WARNING] This is a WARNING message.
[INFO] This is an INFO message.
[DEBUG] This is a DEBUG message.
----------------------------------------------------
```
### 🔹 After Running ./set_log_level <PID> error
```
Log level changed to: ERROR
[ERROR] This is an ERROR message.
----------------------------------------------------
```

### 🔹 After Running ./set_log_level <PID> disable
```
Log level changed to: DISABLED
```
(No messages will be logged now.)

---

## 🎯 Why Use This Approach?
✅ Efficient – Uses real-time signals (SIGRTMIN) for fast log level updates

✅ Lightweight – No threads, just a signal handler

✅ Dynamic – Adjusts log levels without restarting the logger

✅ Practical – Can be used in system monitoring and debugging

## 🟢  Video to illustarate
[View Video](https://drive.google.com/file/d/1V2EZ2xUChEmKPFUlqSKhaqu-nNgjqW9D/view?usp=sharing)

## 🙏 Acknowledgments
This project was developed in C. Contributions and improvements are welcome! 🚀


