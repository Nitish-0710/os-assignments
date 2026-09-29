#include <iostream>
#include <vector>
#include <queue>
#include <iomanip>
#include <algorithm>
#include <string>

using namespace std;

struct Process {
    int pid;
    int arrivalTime;
    int burstTime;
    int priority = 0;

    int completionTime = 0;
    int turnaroundTime = 0;
    int waitingTime = 0;
    int responseTime = -1;
    int remainingTime = 0;
};

struct GanttEntry {
    int pid;
    int startTime;
    int endTime;
};

// ------------------------------------------------------------
// Calculate Turnaround Time and Waiting Time
// ------------------------------------------------------------

void calculateMetrics(vector<Process>& processes) {
    for (auto& p : processes) {
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime;
    }
}

// ------------------------------------------------------------
// Display Gantt Chart
// ------------------------------------------------------------

void displayGanttChart(const vector<GanttEntry>& gantt) {
    if (gantt.empty()) {
        return;
    }

    cout << "\nGantt Chart:\n";

    for (const auto& entry : gantt) {
        cout << "| P" << entry.pid << " ";
    }

    cout << "|\n";

    cout << left << setw(5) << gantt[0].startTime;

    for (const auto& entry : gantt) {
        cout << setw(6) << entry.endTime;
    }

    cout << "\n";
}

// ------------------------------------------------------------
// Display Results
// ------------------------------------------------------------

void displayResults(const vector<Process>& processes,
                    const string& algorithm,
                    const vector<GanttEntry>& gantt) {

    double totalWaitingTime = 0;
    double totalTurnaroundTime = 0;
    double totalResponseTime = 0;

    cout << "\n========================================\n";
    cout << "       " << algorithm << " RESULTS\n";
    cout << "========================================\n";

    cout << left
         << setw(8) << "PID"
         << setw(12) << "Arrival"
         << setw(10) << "Burst"
         << setw(14) << "Completion"
         << setw(14) << "Turnaround"
         << setw(12) << "Waiting"
         << setw(12) << "Response"
         << "\n";

    cout << string(82, '-') << "\n";

    for (const auto& p : processes) {
        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
        totalResponseTime += p.responseTime;

        cout << left
             << setw(8) << ("P" + to_string(p.pid))
             << setw(12) << p.arrivalTime
             << setw(10) << p.burstTime
             << setw(14) << p.completionTime
             << setw(14) << p.turnaroundTime
             << setw(12) << p.waitingTime
             << setw(12) << p.responseTime
             << "\n";
    }

    cout << "\nAverage Waiting Time    : "
         << fixed << setprecision(2)
         << totalWaitingTime / processes.size();

    cout << "\nAverage Turnaround Time : "
         << fixed << setprecision(2)
         << totalTurnaroundTime / processes.size();

    cout << "\nAverage Response Time   : "
         << fixed << setprecision(2)
         << totalResponseTime / processes.size()
         << "\n";

    displayGanttChart(gantt);
}

// ------------------------------------------------------------
// FCFS - First Come First Serve
// ------------------------------------------------------------

void fcfs(vector<Process> processes) {

    sort(processes.begin(), processes.end(),
         [](const Process& a, const Process& b) {
             if (a.arrivalTime == b.arrivalTime)
                 return a.pid < b.pid;

             return a.arrivalTime < b.arrivalTime;
         });

    vector<GanttEntry> gantt;

    int currentTime = 0;

    for (auto& p : processes) {

        if (currentTime < p.arrivalTime) {
            currentTime = p.arrivalTime;
        }

        int startTime = currentTime;

        p.responseTime = currentTime - p.arrivalTime;

        currentTime += p.burstTime;

        p.completionTime = currentTime;

        gantt.push_back({
            p.pid,
            startTime,
            currentTime
        });
    }

    calculateMetrics(processes);

    sort(processes.begin(), processes.end(),
         [](const Process& a, const Process& b) {
             return a.pid < b.pid;
         });

    displayResults(processes, "FCFS", gantt);
}

// ------------------------------------------------------------
// SJF - Preemptive (Shortest Remaining Time First)
// ------------------------------------------------------------

void sjfPreemptive(vector<Process> processes) {

    int n = processes.size();
    int completed = 0;
    int currentTime = 0;

    for (auto& p : processes) {
        p.remainingTime = p.burstTime;
    }

    vector<GanttEntry> gantt;

    int previousPid = -1;
    int segmentStart = 0;

    while (completed < n) {

        int selected = -1;

        for (int i = 0; i < n; i++) {

            if (processes[i].arrivalTime <= currentTime &&
                processes[i].remainingTime > 0) {

                if (selected == -1 ||
                    processes[i].remainingTime <
                        processes[selected].remainingTime ||
                    (processes[i].remainingTime ==
                         processes[selected].remainingTime &&
                     processes[i].arrivalTime <
                         processes[selected].arrivalTime) ||
                    (processes[i].remainingTime ==
                         processes[selected].remainingTime &&
                     processes[i].arrivalTime ==
                         processes[selected].arrivalTime &&
                     processes[i].pid < processes[selected].pid)) {

                    selected = i;
                }
            }
        }

        // CPU is idle
        if (selected == -1) {

            if (previousPid != -1) {
                gantt.push_back({
                    previousPid,
                    segmentStart,
                    currentTime
                });

                previousPid = -1;
            }

            currentTime++;
            continue;
        }

        int currentPid = processes[selected].pid;

        // Process changed
        if (currentPid != previousPid) {

            if (previousPid != -1) {
                gantt.push_back({
                    previousPid,
                    segmentStart,
                    currentTime
                });
            }

            previousPid = currentPid;
            segmentStart = currentTime;
        }

        if (processes[selected].responseTime == -1) {
            processes[selected].responseTime =
                currentTime - processes[selected].arrivalTime;
        }

        processes[selected].remainingTime--;

        currentTime++;

        if (processes[selected].remainingTime == 0) {

            processes[selected].completionTime = currentTime;
            completed++;
        }
    }

    if (previousPid != -1) {
        gantt.push_back({
            previousPid,
            segmentStart,
            currentTime
        });
    }

    calculateMetrics(processes);

    sort(processes.begin(), processes.end(),
         [](const Process& a, const Process& b) {
             return a.pid < b.pid;
         });

    displayResults(processes, "SJF (Preemptive)", gantt);
}

// ------------------------------------------------------------
// Priority - Non-Preemptive
// Smaller number = Higher priority
// ------------------------------------------------------------

void priorityNonPreemptive(vector<Process> processes) {

    int n = processes.size();
    int completed = 0;
    int currentTime = 0;

    vector<bool> isCompleted(n, false);
    vector<GanttEntry> gantt;

    cout << "\nEnter priority for each process:\n";
    cout << "(Smaller number = Higher priority)\n";

    for (auto& p : processes) {
        cout << "Priority for P" << p.pid << ": ";
        cin >> p.priority;
    }

    while (completed < n) {

        int selected = -1;

        for (int i = 0; i < n; i++) {

            if (!isCompleted[i] &&
                processes[i].arrivalTime <= currentTime) {

                if (selected == -1 ||
                    processes[i].priority <
                        processes[selected].priority ||
                    (processes[i].priority ==
                         processes[selected].priority &&
                     processes[i].arrivalTime <
                         processes[selected].arrivalTime) ||
                    (processes[i].priority ==
                         processes[selected].priority &&
                     processes[i].arrivalTime ==
                         processes[selected].arrivalTime &&
                     processes[i].pid <
                         processes[selected].pid)) {

                    selected = i;
                }
            }
        }

        // CPU is idle
        if (selected == -1) {
            currentTime++;
            continue;
        }

        int startTime = currentTime;

        processes[selected].responseTime =
            currentTime - processes[selected].arrivalTime;

        currentTime += processes[selected].burstTime;

        processes[selected].completionTime = currentTime;

        isCompleted[selected] = true;
        completed++;

        gantt.push_back({
            processes[selected].pid,
            startTime,
            currentTime
        });
    }

    calculateMetrics(processes);

    sort(processes.begin(), processes.end(),
         [](const Process& a, const Process& b) {
             return a.pid < b.pid;
         });

    displayResults(processes, "Priority (Non-Preemptive)", gantt);
}

// ------------------------------------------------------------
// Round Robin - Preemptive
// ------------------------------------------------------------

void roundRobin(vector<Process> processes, int timeQuantum) {

    int n = processes.size();
    int completed = 0;
    int currentTime = 0;
    int nextProcess = 0;

    for (auto& p : processes) {
        p.remainingTime = p.burstTime;
    }

    sort(processes.begin(), processes.end(),
         [](const Process& a, const Process& b) {

             if (a.arrivalTime == b.arrivalTime)
                 return a.pid < b.pid;

             return a.arrivalTime < b.arrivalTime;
         });

    queue<int> readyQueue;
    vector<GanttEntry> gantt;

    while (completed < n) {

        // Add newly arrived processes
        while (nextProcess < n &&
               processes[nextProcess].arrivalTime <= currentTime) {

            readyQueue.push(nextProcess);
            nextProcess++;
        }

        // CPU idle
        if (readyQueue.empty()) {

            if (nextProcess < n) {
                currentTime = processes[nextProcess].arrivalTime;
                continue;
            }
        }

        if (readyQueue.empty()) {
            continue;
        }

        int index = readyQueue.front();
        readyQueue.pop();

        if (processes[index].responseTime == -1) {

            processes[index].responseTime =
                currentTime - processes[index].arrivalTime;
        }

        int startTime = currentTime;

        int executionTime =
            min(timeQuantum, processes[index].remainingTime);

        currentTime += executionTime;

        processes[index].remainingTime -= executionTime;

        gantt.push_back({
            processes[index].pid,
            startTime,
            currentTime
        });

        // Add processes that arrived during execution
        while (nextProcess < n &&
               processes[nextProcess].arrivalTime <= currentTime) {

            readyQueue.push(nextProcess);
            nextProcess++;
        }

        if (processes[index].remainingTime > 0) {

            readyQueue.push(index);

        } else {

            processes[index].completionTime = currentTime;
            completed++;
        }
    }

    calculateMetrics(processes);

    sort(processes.begin(), processes.end(),
         [](const Process& a, const Process& b) {
             return a.pid < b.pid;
         });

    displayResults(processes, "Round Robin", gantt);
}

// ------------------------------------------------------------
// Input Processes
// ------------------------------------------------------------

vector<Process> inputProcesses() {

    int n;

    cout << "\nEnter number of processes: ";
    cin >> n;

    vector<Process> processes(n);

    cout << "\nEnter process details:\n";

    for (int i = 0; i < n; i++) {

        processes[i].pid = i + 1;

        cout << "\nProcess P" << i + 1 << ":\n";

        cout << "Arrival Time : ";
        cin >> processes[i].arrivalTime;

        cout << "Burst Time   : ";
        cin >> processes[i].burstTime;
    }

    return processes;
}

// ------------------------------------------------------------
// Main Menu
// ------------------------------------------------------------

int main() {

    cout << "========================================\n";
    cout << "       CPU SCHEDULING SIMULATOR\n";
    cout << "========================================\n";

    vector<Process> processes = inputProcesses();

    int choice;

    do {

        cout << "\n\n--------------- MENU ----------------\n";
        cout << "1. FCFS\n";
        cout << "2. SJF (Preemptive)\n";
        cout << "3. Priority (Non-Preemptive)\n";
        cout << "4. Round Robin (Preemptive)\n";
        cout << "5. Exit\n";
        cout << "-------------------------------------\n";

        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {

            case 1:
                fcfs(processes);
                break;

            case 2:
                sjfPreemptive(processes);
                break;

            case 3:
                priorityNonPreemptive(processes);
                break;

            case 4: {

                int timeQuantum;

                cout << "\nEnter Time Quantum: ";
                cin >> timeQuantum;

                if (timeQuantum <= 0) {
                    cout << "Time Quantum must be greater than 0.\n";
                } else {
                    roundRobin(processes, timeQuantum);
                }

                break;
            }

            case 5:
                cout << "\nExiting program...\n";
                break;

            default:
                cout << "\nInvalid choice. Please try again.\n";
        }

    } while (choice != 5);

    return 0;
}