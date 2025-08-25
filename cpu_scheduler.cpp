#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <iomanip>
#include <string>

struct Process {
    int id;
    int burstTime;
    int priority;
    int waitingTime;
    int turnaroundTime;
    int remainingTime;  // For Round Robin
    int arrivalTime;    // For future extensions
    
    Process(int processId, int bt, int prio = 0) {
        id = processId;
        burstTime = bt;
        priority = prio;
        waitingTime = 0;
        turnaroundTime = 0;
        remainingTime = bt;
        arrivalTime = 0;
    }
};

class CPUScheduler {
private:
    std::vector<Process> processes;
    std::vector<std::pair<int, int>> ganttChart; // (processId, timeSlice)
    
public:
    void addProcess(const Process& p) {
        processes.push_back(p);
    }
    
    void clearProcesses() {
        processes.clear();
        ganttChart.clear();
    }
    
    void resetProcesses() {
        ganttChart.clear();
        for (auto& p : processes) {
            p.waitingTime = 0;
            p.turnaroundTime = 0;
            p.remainingTime = p.burstTime;
        }
    }
    
    // FCFS (First Come First Serve) Algorithm
    void fcfs() {
        resetProcesses();
        std::cout << "\n=== FCFS (First Come First Serve) ===" << std::endl;
        
        int currentTime = 0;
        
        for (auto& process : processes) {
            process.waitingTime = currentTime;
            process.turnaroundTime = process.waitingTime + process.burstTime;
            
            ganttChart.push_back({process.id, process.burstTime});
            currentTime += process.burstTime;
        }
    }
    
    // SJF (Shortest Job First) Non-preemptive Algorithm
    void sjf() {
        resetProcesses();
        std::cout << "\n=== SJF (Shortest Job First) ===" << std::endl;
        
        // Sort processes by burst time
        std::sort(processes.begin(), processes.end(), 
                  [](const Process& a, const Process& b) {
                      return a.burstTime < b.burstTime;
                  });
        
        int currentTime = 0;
        
        for (auto& process : processes) {
            process.waitingTime = currentTime;
            process.turnaroundTime = process.waitingTime + process.burstTime;
            
            ganttChart.push_back({process.id, process.burstTime});
            currentTime += process.burstTime;
        }
    }
    
    // Round Robin Algorithm
    void roundRobin(int timeQuantum) {
        resetProcesses();
        std::cout << "\n=== Round Robin (Time Quantum: " << timeQuantum << ") ===" << std::endl;
        
        std::queue<int> processQueue;
        std::vector<bool> inQueue(processes.size(), false);
        
        // Initialize queue with all processes
        for (int i = 0; i < processes.size(); i++) {
            processQueue.push(i);
            inQueue[i] = true;
        }
        
        int currentTime = 0;
        
        while (!processQueue.empty()) {
            int currentProcessIndex = processQueue.front();
            processQueue.pop();
            inQueue[currentProcessIndex] = false;
            
            Process& currentProcess = processes[currentProcessIndex];
            
            if (currentProcess.remainingTime > 0) {
                int executeTime = std::min(timeQuantum, currentProcess.remainingTime);
                
                ganttChart.push_back({currentProcess.id, executeTime});
                currentProcess.remainingTime -= executeTime;
                currentTime += executeTime;
                
                // If process is not finished, add it back to queue
                if (currentProcess.remainingTime > 0) {
                    processQueue.push(currentProcessIndex);
                    inQueue[currentProcessIndex] = true;
                } else {
                    // Process completed
                    currentProcess.turnaroundTime = currentTime;
                    currentProcess.waitingTime = currentProcess.turnaroundTime - currentProcess.burstTime;
                }
            }
        }
    }
    
    // Calculate and display metrics
    void displayMetrics() {
        std::cout << "\n=== Process Metrics ===" << std::endl;
        std::cout << std::setw(10) << "Process" 
                  << std::setw(12) << "Burst Time" 
                  << std::setw(15) << "Waiting Time" 
                  << std::setw(18) << "Turnaround Time" << std::endl;
        std::cout << std::string(55, '-') << std::endl;
        
        double totalWaitingTime = 0;
        double totalTurnaroundTime = 0;
        
        for (const auto& process : processes) {
            std::cout << std::setw(10) << "P" + std::to_string(process.id)
                      << std::setw(12) << process.burstTime
                      << std::setw(15) << process.waitingTime
                      << std::setw(18) << process.turnaroundTime << std::endl;
            
            totalWaitingTime += process.waitingTime;
            totalTurnaroundTime += process.turnaroundTime;
        }
        
        std::cout << std::string(55, '-') << std::endl;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Average Waiting Time: " << totalWaitingTime / processes.size() << std::endl;
        std::cout << "Average Turnaround Time: " << totalTurnaroundTime / processes.size() << std::endl;
    }
    
    // Display Gantt Chart
    void displayGanttChart() {
        std::cout << "\n=== Gantt Chart ===" << std::endl;
        
        // Top border
        std::cout << " ";
        for (const auto& entry : ganttChart) {
            for (int i = 0; i < entry.second * 2; i++) {
                std::cout << "-";
            }
            std::cout << " ";
        }
        std::cout << std::endl;
        
        // Process names
        std::cout << "|";
        for (const auto& entry : ganttChart) {
            std::string processName = "P" + std::to_string(entry.first);
            int padding = entry.second * 2 - processName.length();
            int leftPad = padding / 2;
            int rightPad = padding - leftPad;
            
            for (int i = 0; i < leftPad; i++) std::cout << " ";
            std::cout << processName;
            for (int i = 0; i < rightPad; i++) std::cout << " ";
            std::cout << "|";
        }
        std::cout << std::endl;
        
        // Bottom border
        std::cout << " ";
        for (const auto& entry : ganttChart) {
            for (int i = 0; i < entry.second * 2; i++) {
                std::cout << "-";
            }
            std::cout << " ";
        }
        std::cout << std::endl;
        
        // Time markers
        std::cout << "0";
        int currentTime = 0;
        for (const auto& entry : ganttChart) {
            currentTime += entry.second;
            std::string timeStr = std::to_string(currentTime);
            int spaces = entry.second * 2 + 1 - timeStr.length();
            for (int i = 0; i < spaces; i++) std::cout << " ";
            std::cout << timeStr;
        }
        std::cout << std::endl;
    }
    
    // Get input for processes
    void inputProcesses() {
        int numProcesses;
        std::cout << "Enter number of processes: ";
        std::cin >> numProcesses;
        
        clearProcesses();
        
        for (int i = 0; i < numProcesses; i++) {
            int burstTime;
            std::cout << "Enter burst time for Process " << (i + 1) << ": ";
            std::cin >> burstTime;
            
            addProcess(Process(i + 1, burstTime));
        }
    }
    
    // Load example data
    void loadExampleData() {
        clearProcesses();
        addProcess(Process(1, 10));
        addProcess(Process(2, 5));
        addProcess(Process(3, 8));
        addProcess(Process(4, 3));
        
        std::cout << "Loaded example data:" << std::endl;
        std::cout << "P1: Burst Time = 10" << std::endl;
        std::cout << "P2: Burst Time = 5" << std::endl;
        std::cout << "P3: Burst Time = 8" << std::endl;
        std::cout << "P4: Burst Time = 3" << std::endl;
    }
};

void displayMenu() {
    std::cout << "\n=== CPU Scheduling Simulator ===" << std::endl;
    std::cout << "1. Input Custom Processes" << std::endl;
    std::cout << "2. Load Example Data" << std::endl;
    std::cout << "3. Run FCFS Algorithm" << std::endl;
    std::cout << "4. Run SJF Algorithm" << std::endl;
    std::cout << "5. Run Round Robin Algorithm" << std::endl;
    std::cout << "6. Compare All Algorithms" << std::endl;
    std::cout << "7. Exit" << std::endl;
    std::cout << "Choose an option: ";
}

void compareAllAlgorithms(CPUScheduler& scheduler) {
    if (scheduler.processes.empty()) {
        std::cout << "No processes loaded. Please input processes first." << std::endl;
        return;
    }
    
    std::cout << "\n=== ALGORITHM COMPARISON ===" << std::endl;
    
    // Store original process order for restoration
    auto originalProcesses = scheduler.processes;
    
    // FCFS
    scheduler.processes = originalProcesses;
    scheduler.fcfs();
    scheduler.displayGanttChart();
    scheduler.displayMetrics();
    
    // SJF
    scheduler.processes = originalProcesses;
    scheduler.sjf();
    scheduler.displayGanttChart();
    scheduler.displayMetrics();
    
    // Round Robin
    scheduler.processes = originalProcesses;
    int timeQuantum;
    std::cout << "\nEnter time quantum for Round Robin: ";
    std::cin >> timeQuantum;
    scheduler.roundRobin(timeQuantum);
    scheduler.displayGanttChart();
    scheduler.displayMetrics();
    
    // Restore original order
    scheduler.processes = originalProcesses;
}

int main() {
    CPUScheduler scheduler;
    int choice;
    
    std::cout << "Welcome to the CPU Scheduling Simulator!" << std::endl;
    std::cout << "This simulator demonstrates FCFS, SJF, and Round Robin algorithms." << std::endl;
    
    while (true) {
        displayMenu();
        std::cin >> choice;
        
        switch (choice) {
            case 1:
                scheduler.inputProcesses();
                break;
                
            case 2:
                scheduler.loadExampleData();
                break;
                
            case 3:
                if (scheduler.processes.empty()) {
                    std::cout << "No processes loaded. Please input processes first." << std::endl;
                    break;
                }
                scheduler.fcfs();
                scheduler.displayGanttChart();
                scheduler.displayMetrics();
                break;
                
            case 4:
                if (scheduler.processes.empty()) {
                    std::cout << "No processes loaded. Please input processes first." << std::endl;
                    break;
                }
                scheduler.sjf();
                scheduler.displayGanttChart();
                scheduler.displayMetrics();
                break;
                
            case 5:
                if (scheduler.processes.empty()) {
                    std::cout << "No processes loaded. Please input processes first." << std::endl;
                    break;
                }
                int timeQuantum;
                std::cout << "Enter time quantum for Round Robin: ";
                std::cin >> timeQuantum;
                scheduler.roundRobin(timeQuantum);
                scheduler.displayGanttChart();
                scheduler.displayMetrics();
                break;
                
            case 6:
                compareAllAlgorithms(scheduler);
                break;
                
            case 7:
                std::cout << "Thank you for using the CPU Scheduling Simulator!" << std::endl;
                return 0;
                
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }
        
        std::cout << "\nPress Enter to continue...";
        std::cin.ignore();
        std::cin.get();
    }
    
    return 0;
}