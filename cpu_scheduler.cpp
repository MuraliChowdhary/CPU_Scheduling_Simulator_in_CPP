// =================================================================================
// IMPORTANT COMPILATION INSTRUCTIONS:
// This code uses C++11 features (like std::mutex and std::atomic).
// You MUST compile it with a C++11 standard flag or newer.
//
// For GCC/Clang (Linux, macOS, MinGW):
//   g++ -std=c++11 your_file.cpp -o output_name
//
// For Visual Studio:
//   Project Properties -> C/C++ -> Language -> C++ Language Standard -> C++11 or newer
// =================================================================================

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
#include <iomanip>
#include <string>
#include <memory>
#include <map>
#include <limits> // Required for clearing input buffer

// Try to include C++11 threading if available
#if __cplusplus >= 201103L || (defined(_MSC_VER) && _MSC_VER >= 1900)
    #include <thread>
    #include <mutex>
    #include <atomic>
    #include <chrono>
    #define HAS_CPP11_THREADING 1
#else
    #define HAS_CPP11_THREADING 0
#endif

// Enhanced Process structure for real-time and multi-core systems
struct Process {
    int id;
    int burstTime;
    int priority;           // 0 = highest priority, 255 = lowest
    int deadline;           // Real-time deadline (for EDF)
    int waitingTime;
    int turnaroundTime;
    int remainingTime;
    int coreId;             // Which core is executing this process
    bool isRealTime;        // Real-time vs regular process
    double powerConsumption; // Power consumed by this process

    Process(int processId, int bt, int prio = 128, int dl = 0, bool rt = false) {
        id = processId;
        burstTime = bt;
        priority = prio;
        deadline = dl;
        waitingTime = 0;
        turnaroundTime = 0;
        remainingTime = bt;
        coreId = -1;
        isRealTime = rt;
        powerConsumption = bt * 0.1; // Simple power model
    }
};

// Power and Performance Metrics
struct SystemMetrics {
    double totalPowerConsumption = 0.0;
    double averagePowerPerCore = 0.0;
    int deadlineMisses = 0;
    int totalProcesses = 0;
    double throughput = 0.0;

    void calculateMetrics(int numCores, int totalTime) {
        if (numCores > 0) {
            averagePowerPerCore = totalPowerConsumption / numCores;
        }
        if (totalTime > 0) {
            throughput = static_cast<double>(totalProcesses) / totalTime;
        }
    }
};

class EnhancedCPUScheduler {
private:
    std::vector<Process> processes;
    std::vector<std::vector<std::pair<int, int>>> ganttCharts;
    int numCores;
    SystemMetrics metrics;

#if HAS_CPP11_THREADING
    std::mutex schedulerMutex;
    std::atomic<bool> schedulingActive{false};
#endif

public:
    EnhancedCPUScheduler(int cores = 4) : numCores(cores) {
        ganttCharts.resize(numCores);
    }

    // Explicitly delete copy constructor and assignment operator to prevent errors
    // due to non-copyable members like std::mutex and std::atomic.
    EnhancedCPUScheduler(const EnhancedCPUScheduler&) = delete;
    EnhancedCPUScheduler& operator=(const EnhancedCPUScheduler&) = delete;

    void addProcess(const Process& p) {
#if HAS_CPP11_THREADING
        std::lock_guard<std::mutex> lock(schedulerMutex);
#endif
        processes.push_back(p);
    }

    void clearProcesses() {
#if HAS_CPP11_THREADING
        std::lock_guard<std::mutex> lock(schedulerMutex);
#endif
        processes.clear();
        for (auto& chart : ganttCharts) {
            chart.clear();
        }
        metrics = SystemMetrics();
    }
    
    // Method to reconfigure the scheduler without using the deleted assignment operator
    void reconfigure(int newNumCores) {
        clearProcesses(); // Clear all existing data
        numCores = newNumCores;
        ganttCharts.resize(numCores);
    }

    // Resets the state of processes for a new simulation run
    void resetProcessesState() {
        for (auto& chart : ganttCharts) {
            chart.clear();
        }
        for (auto& p : processes) {
            p.waitingTime = 0;
            p.turnaroundTime = 0;
            p.remainingTime = p.burstTime;
            p.coreId = -1;
        }
        metrics = SystemMetrics();
    }

    bool isEmpty() const {
        return processes.empty();
    }

    // Multi-Core FCFS with Round-Robin Load Balancing
    void multiCoreFCFS() {
        resetProcessesState();
        std::cout << "\n=== Multi-Core FCFS (Cores: " << numCores << ") ===" << std::endl;

        std::vector<int> coreTime(numCores, 0);
        
        // Distribute processes to cores based on which core will be free first
        for (auto& proc : processes) {
            // Find the core with the minimum current workload
            int bestCore = std::min_element(coreTime.begin(), coreTime.end()) - coreTime.begin();
            
            proc.coreId = bestCore;
            proc.waitingTime = coreTime[bestCore];
            proc.turnaroundTime = proc.waitingTime + proc.burstTime;

            ganttCharts[bestCore].push_back({proc.id, proc.burstTime});
            coreTime[bestCore] += proc.burstTime;
            metrics.totalPowerConsumption += proc.powerConsumption;
        }
        
        int totalTime = coreTime.empty() ? 0 : *std::max_element(coreTime.begin(), coreTime.end());
        metrics.totalProcesses = processes.size();
        metrics.calculateMetrics(numCores, totalTime);
    }

    // Priority-based Scheduling (Non-preemptive)
    void priorityScheduling() {
        resetProcessesState();
        std::cout << "\n=== Priority-Based Scheduling (Cores: " << numCores << ") ===" << std::endl;

        std::vector<Process> sortedProcesses = processes;
        std::sort(sortedProcesses.begin(), sortedProcesses.end(),
                  [](const Process& a, const Process& b) { return a.priority < b.priority; });

        std::vector<int> coreTime(numCores, 0);

        for (auto& sorted_proc : sortedProcesses) {
            int bestCore = std::min_element(coreTime.begin(), coreTime.end()) - coreTime.begin();
            
            auto it = std::find_if(processes.begin(), processes.end(), 
                                   [&](const Process& p){ return p.id == sorted_proc.id; });

            if(it != processes.end()) {
                it->coreId = bestCore;
                it->waitingTime = coreTime[bestCore];
                it->turnaroundTime = coreTime[bestCore] + it->burstTime;
                
                ganttCharts[bestCore].push_back({it->id, it->burstTime});
                coreTime[bestCore] += it->burstTime;
                metrics.totalPowerConsumption += it->powerConsumption;
            }
        }

        int totalTime = coreTime.empty() ? 0 : *std::max_element(coreTime.begin(), coreTime.end());
        metrics.totalProcesses = processes.size();
        metrics.calculateMetrics(numCores, totalTime);
    }

    // Earliest Deadline First (EDF) Scheduling
    void edfScheduling() {
        resetProcessesState();
        std::cout << "\n=== EDF (Earliest Deadline First) Real-Time Scheduling ===" << std::endl;
        
        std::vector<Process> sortedProcesses = processes;
        std::sort(sortedProcesses.begin(), sortedProcesses.end(),
                  [](const Process& a, const Process& b) { return a.deadline < b.deadline; });

        std::vector<int> coreTime(numCores, 0);

        for (auto& sorted_proc : sortedProcesses) {
            int bestCore = std::min_element(coreTime.begin(), coreTime.end()) - coreTime.begin();
            
            auto it = std::find_if(processes.begin(), processes.end(), 
                                   [&](const Process& p){ return p.id == sorted_proc.id; });

            if(it != processes.end()){
                it->coreId = bestCore;
                it->waitingTime = coreTime[bestCore];
                it->turnaroundTime = coreTime[bestCore] + it->burstTime;
                
                if (it->deadline > 0 && it->turnaroundTime > it->deadline) {
                    metrics.deadlineMisses++;
                    std::cout << "⚠️  Process P" << it->id << " missed deadline!" << std::endl;
                }
                
                ganttCharts[bestCore].push_back({it->id, it->burstTime});
                coreTime[bestCore] += it->burstTime;
                metrics.totalPowerConsumption += it->powerConsumption;
            }
        }

        int totalTime = coreTime.empty() ? 0 : *std::max_element(coreTime.begin(), coreTime.end());
        metrics.totalProcesses = processes.size();
        metrics.calculateMetrics(numCores, totalTime);
    }

    // Multi-Core Round Robin Scheduling
    void multiCoreRoundRobin(int timeQuantum) {
        resetProcessesState();
        std::cout << "\n=== Multi-Core Round Robin (Quantum: " << timeQuantum << ", Cores: " << numCores << ") ===" << std::endl;

        std::vector<std::queue<Process*>> coreQueues(numCores);
        std::vector<int> coreTime(numCores, 0);
        int processesFinished = 0;

        for (size_t i = 0; i < processes.size(); i++) {
            coreQueues[i % numCores].push(&processes[i]);
        }
        
        bool active = true;
        while(active) {
            active = false;
            for(int core = 0; core < numCores; ++core) {
                if(!coreQueues[core].empty()) {
                    active = true;
                    Process* proc = coreQueues[core].front();
                    coreQueues[core].pop();

                    int executeTime = std::min(timeQuantum, proc->remainingTime);
                    ganttCharts[core].push_back({proc->id, executeTime});
                    proc->remainingTime -= executeTime;
                    coreTime[core] += executeTime;

                    if (proc->remainingTime > 0) {
                        coreQueues[core].push(proc);
                    } else {
                        proc->coreId = core;
                        proc->turnaroundTime = coreTime[core];
                        proc->waitingTime = proc->turnaroundTime - proc->burstTime;
                        metrics.totalPowerConsumption += proc->powerConsumption;
                    }
                }
            }
        }
        
        int totalTime = coreTime.empty() ? 0 : *std::max_element(coreTime.begin(), coreTime.end());
        metrics.totalProcesses = processes.size();
        metrics.calculateMetrics(numCores, totalTime);
    }
    
    // Display all metrics
    void displayAllResults() {
        displayEnhancedMetrics();
        displayMultiCoreGanttChart();
    }
    
    // Display enhanced metrics
    void displayEnhancedMetrics() {
        std::cout << "\n--- Process Performance ---" << std::endl;
        std::cout << std::left << std::setw(10) << "Process"
                  << std::setw(8) << "Core"
                  << std::setw(12) << "Burst"
                  << std::setw(10) << "Priority"
                  << std::setw(12) << "Deadline"
                  << std::setw(15) << "Waiting Time"
                  << std::setw(18) << "Turnaround Time"
                  << std::setw(12) << "Power (W)" << std::endl;
        std::cout << std::string(100, '-') << std::endl;

        double totalWaitingTime = 0;
        double totalTurnaroundTime = 0;
        
        for (const auto& process : processes) {
            std::cout << std::left << std::setw(10) << ("P" + std::to_string(process.id))
                      << std::setw(8) << process.coreId
                      << std::setw(12) << process.burstTime
                      << std::setw(10) << process.priority
                      << std::setw(12) << (process.deadline > 0 ? std::to_string(process.deadline) : "N/A")
                      << std::setw(15) << process.waitingTime
                      << std::setw(18) << process.turnaroundTime
                      << std::fixed << std::setprecision(2) << std::setw(12) << process.powerConsumption << std::endl;

            totalWaitingTime += process.waitingTime;
            totalTurnaroundTime += process.turnaroundTime;
        }
        std::cout << std::string(100, '-') << std::endl;

        std::cout << "\n--- System Performance ---" << std::endl;
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "💻 Number of Cores: " << numCores << std::endl;
        std::cout << "⚡ Total Power Consumption: " << metrics.totalPowerConsumption << " W" << std::endl;
        std::cout << "🔋 Average Power per Core: " << metrics.averagePowerPerCore << " W" << std::endl;
        std::cout << "📊 Throughput: " << metrics.throughput << " processes/time unit" << std::endl;
        if (!processes.empty()) {
            std::cout << "⏱️  Average Waiting Time: " << totalWaitingTime / processes.size() << std::endl;
            std::cout << "🔄 Average Turnaround Time: " << totalTurnaroundTime / processes.size() << std::endl;
        }

        if (metrics.deadlineMisses > 0) {
            std::cout << "❌ Deadline Misses: " << metrics.deadlineMisses << " ("
                      << (100.0 * metrics.deadlineMisses / processes.size()) << "%)" << std::endl;
        } else {
            std::cout << "✅ All Real-time Deadlines Met!" << std::endl;
        }
    }

    // Display multi-core Gantt charts
    void displayMultiCoreGanttChart() {
        std::cout << "\n=== MULTI-CORE GANTT CHART ===" << std::endl;
        for (int core = 0; core < numCores; core++) {
            if (ganttCharts[core].empty()) continue;

            std::cout << "\nCore " << core << ":" << std::endl;
            
            std::string topBorder = " ", midLayer = "|", bottomBorder = " ", timeMarkers = "0";

            int currentTime = 0;
            for (const auto& entry : ganttCharts[core]) {
                int width = entry.second * 3 + 2;
                std::string pName = "P" + std::to_string(entry.first);
                int padding = width - pName.length();

                topBorder += std::string(width, '-') + " ";
                bottomBorder += std::string(width, '-') + " ";
                midLayer += std::string(padding / 2, ' ') + pName + std::string(padding - (padding / 2), ' ') + "|";
                
                currentTime += entry.second;
                std::string timeStr = std::to_string(currentTime);
                timeMarkers += std::string(width + 1 - timeStr.length(), ' ') + timeStr;
            }

            std::cout << topBorder << std::endl;
            std::cout << midLayer << std::endl;
            std::cout << bottomBorder << std::endl;
            std::cout << timeMarkers << std::endl;
        }
    }

    void loadEnhancedExampleData() {
        clearProcesses();
        addProcess(Process(1, 10, 100, 25, false));
        addProcess(Process(2, 5, 50, 15, true));
        addProcess(Process(3, 8, 150, 20, false));
        addProcess(Process(4, 3, 25, 10, true));
        addProcess(Process(5, 12, 75, 30, false));
        addProcess(Process(6, 6, 10, 18, true));
        std::cout << "\nLoaded enhanced example data." << std::endl;
    }

    void inputEnhancedProcesses() {
        int numProcesses;
        std::cout << "Enter number of processes: ";
        std::cin >> numProcesses;
        
        clearProcesses();
        
        for (int i = 0; i < numProcesses; i++) {
            int burstTime, priority, deadline;
            char isRealTime;

            std::cout << "\nProcess " << (i + 1) << ":" << std::endl;
            std::cout << "Enter burst time: ";
            std::cin >> burstTime;
            std::cout << "Enter priority (0-255, lower is higher): ";
            std::cin >> priority;
            std::cout << "Enter deadline (0 for none): ";
            std::cin >> deadline;
            std::cout << "Is real-time process? (y/n): ";
            std::cin >> isRealTime;

            addProcess(Process(i + 1, burstTime, priority, deadline, (isRealTime == 'y' || isRealTime == 'Y')));
        }
    }
};

void displayEnhancedMenu() {
    std::cout << "\n╔════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║    ENHANCED CPU SCHEDULING SIMULATOR       ║" << std::endl;
    std::cout << "╠════════════════════════════════════════════════╣" << std::endl;
    std::cout << "║ 1. Input Custom Processes                      ║" << std::endl;
    std::cout << "║ 2. Load Enhanced Example Data                  ║" << std::endl;
    std::cout << "║ 3. Run Multi-Core FCFS Algorithm             ║" << std::endl;
    std::cout << "║ 4. Run Priority-Based Scheduling             ║" << std::endl;
    std::cout << "║ 5. Run EDF (Earliest Deadline First)         ║" << std::endl;
    std::cout << "║ 6. Run Multi-Core Round Robin Algorithm      ║" << std::endl;
    std::cout << "║ 7. Compare All Algorithms                      ║" << std::endl;
    std::cout << "║ 8. Configure System (Number of Cores)        ║" << std::endl;
    std::cout << "║ 9. Exit                                        ║" << std::endl;
    std::cout << "╚════════════════════════════════════════════════╝" << std::endl;
    std::cout << "Choose an option: ";
}

void runAndDisplay(EnhancedCPUScheduler& scheduler, int algo, int timeQuantum = 0) {
    if (scheduler.isEmpty()) {
        std::cout << "\nNo processes loaded. Please use option 1 or 2 first." << std::endl;
        return;
    }

    switch (algo) {
        case 3: scheduler.multiCoreFCFS(); break;
        case 4: scheduler.priorityScheduling(); break;
        case 5: scheduler.edfScheduling(); break;
        case 6: scheduler.multiCoreRoundRobin(timeQuantum); break;
    }
    scheduler.displayAllResults();
}

int main() {
    EnhancedCPUScheduler scheduler(4); // Default to 4 cores
    int choice;

    std::cout << "🎯 Welcome to the Enhanced CPU Scheduling Simulator!" << std::endl;

    while (true) {
        displayEnhancedMenu();
        std::cin >> choice;

        if(std::cin.fail()){
            std::cout << "Invalid input. Please enter a number." << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: scheduler.inputEnhancedProcesses(); break;
            case 2: scheduler.loadEnhancedExampleData(); break;
            case 3:
            case 4:
            case 5: runAndDisplay(scheduler, choice); break;
            case 6: {
                if (scheduler.isEmpty()) {
                     std::cout << "\nNo processes loaded. Please use option 1 or 2 first." << std::endl;
                } else {
                    int tq;
                    std::cout << "Enter time quantum for Round Robin: ";
                    std::cin >> tq;
                    runAndDisplay(scheduler, choice, tq);
                }
                break;
            }
            case 7: 
                runAndDisplay(scheduler, 3);
                runAndDisplay(scheduler, 4);
                runAndDisplay(scheduler, 5);
                {
                    int tq;
                    std::cout << "\nEnter time quantum for Round Robin comparison: ";
                    std::cin >> tq;
                    runAndDisplay(scheduler, 6, tq);
                }
                break;
            case 8: {
                int numCores;
                std::cout << "Enter new number of CPU cores (1-16): ";
                std::cin >> numCores;
                if (numCores >= 1 && numCores <= 16) {
                    scheduler.reconfigure(numCores);
                    std::cout << "\nSystem reconfigured with " << numCores << " cores." << std::endl;
                } else {
                    std::cout << "\nInvalid number of cores. Configuration unchanged." << std::endl;
                }
                break;
            }
            case 9:
                std::cout << "🎯 Thank you for using the simulator!" << std::endl;
                return 0;
            default:
                std::cout << "Invalid choice. Please try again." << std::endl;
        }

        if (choice != 9) {
            std::cout << "\nPress Enter to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::cin.get();
        }
    }

    return 0;
}