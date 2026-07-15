# Relay

> **Relay — A High-Performance Reverse Proxy Built in Modern C++**

Relay is a lightweight, educational reverse proxy built completely from scratch in Modern C++20. It demonstrates fundamental systems engineering concepts without relying on high-level networking frameworks. 

Think of it as a "Mini Nginx" or "Mini Envoy", designed specifically to showcase deeply understood systems architecture.

## Features (Roadmap)
- **POSIX Sockets:** Raw TCP socket management without abstractions like Boost.Asio.
- **HTTP Parsing:** Custom parsing of raw byte streams into structured HTTP/1.1 objects.
- **Reverse Proxying:** Forwarding requests to backend servers and returning responses to clients.
- **Load Balancing:** Round Robin algorithm across multiple backends.
- **Concurrency:** Custom Thread Pool and thread-safe Work Queue for high performance.
- **Health Checks:** Active monitoring of backend availability.
- **Observability:** Custom `/metrics` endpoint to monitor throughput, latency, and queue depth.

## Tech Stack
- **Language:** Modern C++20
- **Networking:** POSIX Sockets
- **Build System:** CMake
- **Testing:** GoogleTest
- **Libraries:** `fmt`, `spdlog`, `nlohmann/json`

## Building and Running

```bash
# Clone the repository
git clone https://github.com/yourusername/relay.git
cd relay

# Build the project
cmake -S . -B build
cmake --build build

# Run the proxy
./build/relay
```

## Architecture Overview
Relay operates using a highly concurrent Thread Pool model. An Accept Thread constantly listens for incoming connections and pushes raw socket file descriptors onto a thread-safe Work Queue. A pool of Worker Threads sleeps until work is available, pops the socket, parses the HTTP request, queries the Backend Manager for a healthy routing destination, and proxies the traffic.

## License
MIT License
