# Relay: Systems Engineering Study Guide

This document serves as a comprehensive, beginner-friendly guide to the systems engineering concepts, design decisions, and interview questions related to the Relay reverse proxy project.

---

## Milestone 0: Architecture & Foundation

### Concepts
**What is a Reverse Proxy?**
A proxy is a middleman. A *forward proxy* sits in front of clients, acting on their behalf to fetch resources from the internet. A **reverse proxy** sits in front of *servers*. When a client makes a request to a website, the request actually hits the reverse proxy first. The proxy then decides which backend server should handle the request, forwards it, and returns the response to the client. The client never knows the backend server exists.

**Why do we need a Reverse Proxy?**
- **Load Balancing:** Distributing incoming traffic across multiple backend servers to prevent any single server from becoming overwhelmed.
- **High Availability:** If one backend server crashes, the proxy can route traffic to healthy ones.
- **Security:** Hiding the internal network topology.

### Interview Questions & Answers (Milestone 0)

**Q1: What is the fundamental difference between a Forward Proxy and a Reverse Proxy?**
*Answer:* A forward proxy acts on behalf of the client (e.g., hiding client IPs from the internet), while a reverse proxy acts on behalf of the server (e.g., hiding backend servers from the internet).

**Q2: In a highly concurrent system, what problem does a Thread Pool solve compared to spawning a new thread for every incoming connection?**
*Answer:* Creating and destroying threads has significant OS overhead (context switching, memory allocation for stacks). A Thread Pool reuses a fixed number of threads, avoiding this overhead and preventing the system from running out of memory under heavy load.

**Q3: What is a POSIX socket at the operating system level?**
*Answer:* In Unix-like systems, a socket is treated as a file. The OS provides a "file descriptor" (an integer), and the application reads/writes bytes to this integer just like reading or writing to a file on disk.

**Q4: Why is Round Robin a good starting load-balancing algorithm, and when might it perform poorly?**
*Answer:* Round Robin is easy to implement and distributes requests evenly if all requests take the same amount of time. It performs poorly if some requests are very heavy, causing one server to get bogged down while others are idle (this is where "Least Connections" load balancing is better).

**Q5: What happens to a system's performance if multiple worker threads are constantly fighting over the exact same `std::mutex`?**
*Answer:* This causes "lock contention". Threads spend all their time sleeping and waking up to check the lock rather than doing actual work, completely destroying the benefits of multithreading and causing severe latency.

---

## Milestone 1: TCP Sockets

### Concepts & Workflow
**What is a socket?**
At its core, a socket is a software endpoint that allows two programs to communicate over a network.

**The Core POSIX Socket Lifecycle:**
To act as a server, we must go through a specific sequence of system calls:
1. `socket()`: Asks the OS to create a socket and gives us the file descriptor.
2. `bind()`: Associates this socket with a specific network interface (IP address) and a Port (e.g., `8080`).
3. `listen()`: Tells the OS, "I am a server. Put this socket in a passive mode to listen for incoming connections."
4. `accept()`: Asks the OS to take the next pending connection from the queue. **This call is blocking**—your program will go to sleep here until a client actually connects.

### Interview Questions & Answers (Milestone 1)

**Q1: Why does `accept()` return a brand new file descriptor, rather than just letting us communicate over the original file descriptor we created with `socket()`?**
*Answer:* If `accept()` used the original listening socket, we could only communicate with one client at a time and would have to stop listening for new connections. By returning a *new* file descriptor for the specific client, the original socket can remain open and continue listening for *other* clients concurrently.

**Q2: What does the `bind()` system call actually do under the hood?**
*Answer:* It reserves the specific network interface and port at the OS level so no other application on the machine can claim it.

**Q3: If we forget to call `close()` on the client file descriptor after sending a response, what systemic problem will our proxy eventually encounter?**
*Answer:* This causes a "file descriptor leak". The OS sets a hard limit on how many files/sockets a process can open (often 1024 by default). If we don't close them, our server will quickly hit this limit and crash with a "Too many open files" error.

**Q4: Why do we need to use `htons()` when assigning the port number?**
*Answer:* `htons()` stands for "Host TO Network Short". Different CPUs store integers differently in memory (Little-Endian vs Big-Endian). Network protocols strictly require Big-Endian. This function safely converts your CPU's format to the network format so the OS understands the port correctly.

**Q5: Because `accept()` blocks the loop, what happens if two clients try to connect at the exact same millisecond?**
*Answer:* The OS network stack handles this. It queues up the incoming connections based on the "backlog" number we provided to `listen()`. Once we finish handling the first client and loop back around, `accept()` will instantly pop the second client off the OS queue.
