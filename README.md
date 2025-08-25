# CPU Scheduling Simulator

A comprehensive C++ implementation of CPU scheduling algorithms including FCFS, SJF, and Round Robin with visual Gantt chart representation and performance metrics.

## Features

### Supported Algorithms
- **FCFS (First Come First Serve)**: Processes are executed in the order they arrive
- **SJF (Shortest Job First)**: Non-preemptive algorithm that selects the process with the shortest burst time
- **Round Robin**: Preemptive algorithm with configurable time quantum

### Metrics Calculated
- **Waiting Time**: Time a process waits in the ready queue
- **Turnaround Time**: Total time from arrival to completion
- **Average Waiting Time**: Mean waiting time across all processes
- **Average Turnaround Time**: Mean turnaround time across all processes

### Visual Features
- **ASCII Gantt Chart**: Visual representation of process execution timeline
- **Detailed Metrics Table**: Clean tabular display of all timing metrics
- **Algorithm Comparison**: Side-by-side comparison of all three algorithms

## Compilation and Execution

### Using Make (Recommended)
```bash
# Compile the program
make

# Run the program
make run

# Clean build artifacts
make clean
```

### Manual Compilation
```bash
# Compile with g++
g++ -std=c++11 -Wall -Wextra -O2 -o cpu_scheduler cpu_scheduler.cpp

# Run the executable
./cpu_scheduler
```

## Usage

1. **Start the Program**: Run the executable to see the main menu
2. **Load Processes**: Choose to either:
   - Input custom processes (Option 1)
   - Load example data (Option 2)
3. **Run Algorithms**: Execute individual algorithms (Options 3-5) or compare all (Option 6)
4. **View Results**: See Gantt charts and performance metrics for each algorithm

### Example Session

```
=== CPU Scheduling Simulator ===
1. Input Custom Processes
2. Load Example Data
3. Run FCFS Algorithm
4. Run SJF Algorithm
5. Run Round Robin Algorithm
6. Compare All Algorithms
7. Exit
Choose an option: 2

Loaded example data:
P1: Burst Time = 10
P2: Burst Time = 5
P3: Burst Time = 8
P4: Burst Time = 3

Choose an option: 6

=== ALGORITHM COMPARISON ===

=== FCFS (First Come First Serve) ===

=== Gantt Chart ===
 -------------------- ---------- ---------------- ------ 
|        P1        |    P2    |       P3       | P4 |
 -------------------- ---------- ---------------- ------ 
0                   10         15               23    26

=== Process Metrics ===
   Process  Burst Time  Waiting Time  Turnaround Time
-------------------------------------------------------
        P1          10             0               10
        P2           5            10               15
        P3           8            15               23
        P4           3            23               26
-------------------------------------------------------
Average Waiting Time: 12.00
Average Turnaround Time: 18.50
```

## Program Structure

### Core Components

- **Process Struct**: Contains process information (ID, burst time, waiting time, etc.)
- **CPUScheduler Class**: Main class handling all scheduling algorithms and calculations
- **Algorithm Functions**: Separate implementations for FCFS, SJF, and Round Robin
- **Display Functions**: Gantt chart visualization and metrics presentation

### Key Functions

- `fcfs()`: First Come First Serve implementation
- `sjf()`: Shortest Job First implementation  
- `roundRobin(int timeQuantum)`: Round Robin with configurable time quantum
- `displayGanttChart()`: ASCII Gantt chart visualization
- `displayMetrics()`: Performance metrics calculation and display

## Example Data

The program includes built-in example data for testing:
- Process 1: Burst Time = 10
- Process 2: Burst Time = 5  
- Process 3: Burst Time = 8
- Process 4: Burst Time = 3

## Educational Value

This simulator is perfect for understanding:
- How different scheduling algorithms affect process execution order
- The impact of algorithm choice on waiting and turnaround times
- Visual representation of CPU scheduling through Gantt charts
- Performance trade-offs between different scheduling approaches

## Future Enhancements

- Priority-based scheduling algorithms
- Preemptive SJF (Shortest Remaining Time First)
- Process arrival times simulation
- Multi-level queue scheduling
- Performance comparison graphs