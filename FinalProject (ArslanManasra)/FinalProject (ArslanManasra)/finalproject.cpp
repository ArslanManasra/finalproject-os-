#include <iostream>
#include <fstream>
#include <vector>
#include <queue>

using namespace std;

struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int endTime = 0;
    int TAT = 0;
    int waitingTime = 0;
    int originalBurstTime;
};

void readFile(const string& filename, vector<Process>& processes) {
    ifstream file(filename);
    if (!file) {
        cout << "Unable to open file\n";
        exit(1);
    }

    int n, arrival, burst;
    file >> n;
    for (int i = 0; i < n; i++) {
        file >> arrival >> burst;
        processes.push_back({ i + 1, arrival, burst, 0, 0, 0, burst });
    }
    file.close();
}

void FCFS(vector<Process>& processes, int contextSwitch) {
    int currentTime = 0;
    for (auto& p : processes) {
        if (currentTime < p.arrivalTime) {
            currentTime = p.arrivalTime;
        }
        p.waitingTime = currentTime - p.arrivalTime;
        currentTime += p.burstTime + contextSwitch;
        p.endTime = currentTime - contextSwitch;
        p.TAT = p.endTime - p.arrivalTime;
    }
}

void SRT(vector<Process>& processes, int contextSwitch) {
    auto comp = [](Process* a, Process* b) { return a->burstTime > b->burstTime; };
    priority_queue<Process*, vector<Process*>, decltype(comp)> pq(comp);

    int currentTime = 0, idx = 0;
    while (!pq.empty() || idx < processes.size()) {
        while (idx < processes.size() && processes[idx].arrivalTime <= currentTime) {
            pq.push(&processes[idx++]);
        }

        if (!pq.empty()) {
            Process* current = pq.top();
            pq.pop();
            currentTime += 1;
            current->burstTime--;

            while (idx < processes.size() && processes[idx].arrivalTime <= currentTime) {
                pq.push(&processes[idx++]);
            }

            if (current->burstTime > 0) {
                pq.push(current);
            }
            else {
                current->endTime = currentTime;
                current->TAT = currentTime - current->arrivalTime;
                current->waitingTime = current->TAT - current->originalBurstTime;
            }

            currentTime += contextSwitch;
        }
        else {
            if (idx < processes.size()) {
                currentTime = processes[idx].arrivalTime;
            }
        }
    }
}

void RR(vector<Process>& processes, int quantum, int contextSwitch) {
    queue<Process*> q;
    int currentTime = 0, idx = 0;
    while (!q.empty() || idx < processes.size()) {
        while (idx < processes.size() && processes[idx].arrivalTime <= currentTime) {
            q.push(&processes[idx++]);
        }

        if (!q.empty()) {
            Process* current = q.front();
            q.pop();

            int timeSlice = min(quantum, current->burstTime);
            current->burstTime -= timeSlice;
            currentTime += timeSlice;

            while (idx < processes.size() && processes[idx].arrivalTime <= currentTime) {
                q.push(&processes[idx++]);
            }

            if (current->burstTime > 0) {
                q.push(current);
            }
            else {
                current->endTime = currentTime;
                current->TAT = currentTime - current->arrivalTime;
                current->waitingTime = current->TAT - current->originalBurstTime;
            }

            currentTime += contextSwitch;
        }
        else {
            if (idx < processes.size()) {
                currentTime = processes[idx].arrivalTime;
            }
        }
    }
}

void printResults(const vector<Process>& processes) {
    double totalTAT = 0, totalWaiting = 0;
    cout << "Process\tArrival\tBurst\tEnd\tWaiting\tTAT\n";
    for (const auto& p : processes) {
        cout << p.id << "\t" << p.arrivalTime << "\t" << p.originalBurstTime << "\t"
            << p.endTime << "\t" << p.waitingTime << "\t" << p.TAT << endl;
        totalTAT += p.TAT;
        totalWaiting += p.waitingTime;
    }

    cout << "Average Waiting Time: " << totalWaiting / processes.size() << endl;
    cout << "Average Turnaround Time: " << totalTAT / processes.size() << endl;
}

int main() {
    string filename = "Arslan.txt";
    vector<Process> processes;
    readFile(filename, processes);

    int contextSwitch = 0;
    int quantum = 3;
    int choice;
    cout << "Select your Choice:\n0.FCFS\n1.SRT\n2.RR\n3.Exit" << endl;
    cin >> choice;

    if (choice == 0) {
        cout << "This is the FSCS algorithm for your values:" << endl;
        FCFS(processes, contextSwitch);
        printResults(processes);
    }
    else if (choice == 1) {
        cout << "This is the SRT algorithm for your values:" << endl;
        SRT(processes, contextSwitch);
        printResults(processes);
    }
    else if (choice == 2) {
        cout << "This is the FSCS algorithm for your values:" << endl;
        RR(processes, quantum, contextSwitch);
        printResults(processes);
    }
    else if (choice == 3) {
        cout << "BYE BYE" << endl;
    }
    else {
        cout << "--------------------------------------------------------" << endl;
        cout << "Invalid Choice" << endl;
    }

    return 0;
}