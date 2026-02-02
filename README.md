# 🇺🇸 English Version

## Elo Maluco Solver | A* Search & Multithreading
This repository contains a high-performance solution for the **"Elo Maluco"** puzzle (a classic 80s logic toy), developed for the *Algorithm Analysis and Design (PAA)* course at **UNIFEI**.

### 🧠 Technical Overview
* **Algorithm:** A* (A-Star) Search.
* **Heuristic:** Specialized heuristic based on Manhattan Distance, piece height patterns, and color distribution.
* **Parallelism:** Multithreaded implementation for parallel state-space exploration.
* **Synchronization:** Thread-safe global score updates using `std::mutex` and atomic-like operations.

### 📊 Performance & Scalability
The solver demonstrates near-linear scalability and high efficiency:
* **Complex Instances (100+ moves):** Solved in ~4 seconds.
* **Throughput:** Processes an average of 111,000 iterations per execution.
* **Robustness:** Uses stochastic generation (`std::mt19937`) to avoid local minima during the search process.

### 🛠️ Stack & Libraries
* **Language:** Modern C++11 (Object-Oriented Programming).
* **Concurrency:** `<thread>`, `<mutex>`, `<condition_variable>`.
* **Graphics & Math:** OpenGL, GLUT, and GLM (for visualization and vector math).
* **Data Parsing:** TinyXML2 for game state input/output.
* **Build System:** CMake and Makefile.

### 📂 Project Structure
| Path | Description |
| :--- | :--- |
| `/src` | Core logic, solver implementation, and thread management. |
| `/data` | XML files containing initial game states for testing. |
| `/docs` | Technical reports and documentation in PDF format. |
| `CMakeLists.txt` | Compiler flags and dependency management. |

### 🚀 Getting Started
Ensure dependencies (OpenGL, GLUT, GLM) are installed. 

**Build the project using CMake:**
```bash
mkdir build && cd build
cmake ..
make
