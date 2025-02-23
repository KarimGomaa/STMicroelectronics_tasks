# 👕 Clothing Shop Simulation 🛍️  

This project simulates a **multi-process clothing shop** where customers can **place orders**, an inventory manager can **update stock**, and a shop process **manages everything** in real-time. The system uses **Inter-Process Communication (IPC)** via **FIFO (named pipes)** and **shared memory with semaphores** to ensure synchronization.  

---

## 📌 Table of Contents
- [🛠️ System Overview](#-system-overview)  
- [🔗 Inter-Process Communication](#-inter-process-communication)  
- [⚙️ Features](#️-features)  
- [🚀 How to Run](#-how-to-run)  
- [📂 Program Structure](#-program-structure)  
- [🔍 Error Handling](#-error-handling)  
- [✨ Future Improvements](#-future-improvements)  

---

## 🛠️ System Overview  

The Clothing Shop Simulation consists of **three main processes**:  

1. **🛍️ Shop Process (`shop.c`)**  
   - Manages the inventory using **shared memory**.  
   - Handles customer requests for inventory.  
   - Processes customer orders.  
   - Logs all customer orders in a file (`orders.log`).  
   - Notifies customers of their order status.  
   
2. **👤 Customer Process (`customer.c`)**  
   - Requests inventory information from the shop.  
   - Places orders for available items.  
   - Receives confirmation messages from the shop.  
   - Handles invalid input (e.g., non-existent items or excessive quantity).  

3. **📦 Inventory Manager (`inventory_manager.c`)**  
   - Updates stock levels and prices.  
   - Adds new items to the inventory.  
   - Removes items when their stock and price are set to `0`.  
   - Saves inventory updates to `inventory.txt` (persistent storage).  
   - Notifies the shop and customers about inventory updates in **real-time**.  

---

## 🔗 Inter-Process Communication  

The processes communicate using **FIFO (Named Pipes)** and **Shared Memory with Semaphores**:  

| **FIFO Name** | **Purpose** | **Used By** |
|--------------|------------|-------------|
| `/tmp/shop_fifo` | Customer sends requests to the shop | Customer → Shop |
| `/tmp/inventory_fifo` | Shop sends inventory data to the customer | Shop → Customer |
| `/tmp/confirm_fifo` | Shop sends order confirmation to the customer | Shop → Customer |
| `/tmp/inventory_update_fifo` | Inventory manager sends stock updates to the shop and customers | Inventory Manager → Shop & Customers |

Additionally, the **shop and inventory manager use**:  
✅ **Shared Memory (SHM)** → Stores the inventory in memory.  
✅ **Semaphores (SEM)** → Prevents simultaneous writes to the inventory.

---

## ⚙️ Features  

✅ **Multi-Process Design** – Shop, customers, and inventory manager run independently.  
✅ **Real-Time Inventory Updates** – Customers and the shop get updates instantly.  
✅ **Order Logging** – All orders are stored in `orders.log` with timestamps.  
✅ **Persistent Inventory** – Inventory is saved in `inventory.txt` and loaded on restart.  
✅ **Stock Management** – Admin can add, remove, and update items.  
✅ **Error Handling** – Handles missing stock, invalid input, and IPC failures.  
✅ **Automatic Item Removal** – Items with `0` stock and `0` price are deleted.  

---

## 🚀 How to Run  

### **1️⃣ Compile the Programs**
```bash
gcc shop.c -o shop -lrt -pthread
gcc customer.c -o customer -lrt -pthread
gcc inventory_manager.c -o inventory_manager -lrt -pthread
```

### **2️⃣ Ensure Order Log & Inventory Files Exist**
```bash
touch orders.log
touch inventory.txt
```

### **3️⃣ Create FIFOs (Named Pipes)**
```bash
rm -f /tmp/shop_fifo /tmp/inventory_fifo /tmp/confirm_fifo /tmp/inventory_update_fifo
mkfifo /tmp/shop_fifo
mkfifo /tmp/inventory_fifo
mkfifo /tmp/confirm_fifo
mkfifo /tmp/inventory_update_fifo
```


### **4️⃣ Start the Shop Process**
```bash
./shop
```


### **5️⃣ Start the Customer Process**
```bash
./customer
```


### **6️⃣ Start the Inventory Manager**
```bash
./inventory_manager
```

✅ Now, you can interact with the system!


## 📂 Program Structure
```bash
/clothing-shop/
│── shop.c                 # Shop process (Manages inventory & processes orders)
│── customer.c             # Customer process (Places orders & views inventory)
│── inventory_manager.c    # Inventory Manager (Updates stock & saves inventory)
│── inventory.txt          # Persistent inventory storage
│── design.puml            # uml design for the shop
│── README.md              # Documentation
```


## 🔍 Error Handling

#### The system includes robust error handling for:

- ❌ Invalid Input – If a customer enters an invalid item name or excessive quantity, an error message is displayed.
- ❌ FIFO Issues – If a FIFO cannot be opened, an error message is logged, and the system retries.
- ❌ Shared Memory Failures – If shared memory cannot be accessed, the system prints an error and exits.
- ❌ Semaphore Deadlocks – Ensures locks are properly acquired and released to prevent deadlocks.
- ❌ File Handling Errors – If `orders.log` or `inventory.txt` cannot be opened, an error is printed.

##  🏪 Clothing shop Simulation uml Desing
![Image](https://github.com/user-attachments/assets/dd1c3c2b-5ac9-4c66-a983-f6c7d17ec654)

## 📹 Clothing Shop Simulation Video
[View Video]()

## ✨ Future Improvements
- 🚀 Multi-Customer Support – Enable multiple customers to order simultaneously using threads.
- 📊 Admin Dashboard – A real-time UI for monitoring inventory and sales.
- 🌐 Web Interface – Create a REST API for managing inventory and processing orders online.
- 🔄 Automatic Inventory Restocking – Automatically restock items when they reach a low threshold.
- 📜 Better Logging System – Store logs in a database for analytics.


## 🎯 Conclusion
This Clothing Shop Simulation is a powerful multi-process system that efficiently manages inventory, processes customer orders, and updates stock in real time. The system uses FIFO pipes, shared memory, and semaphores to ensure proper synchronization between processes.

- 👕 Customers can browse and order items.
- 🛒 The shop handles requests and logs all orders.
- 📦 Admins can update inventory, add/remove items, and track stock.
- 💡 This project is a great example of Inter-Process Communication (IPC) in Linux and can be extended further!

##
Thank you for exploring My Clothing Shop project simulation. If you have any questions or feedback, feel free to reach out or contribute to the repository. 🔥 Happy coding! 🚀

