#include <iostream>
#include <vector>
#include <algorithm>
#include <climits>
#include <queue>
using namespace std;

struct Process
{
    int id;
    int burstTime;
    int arrivalTime;
    int priority;
    int remainingTime;
    int startTime;
    int completionTime;
    int waitingTime;
    int turnaroundTime;
    int responseTime;
};

bool compareArrivalTime(const Process &a, const Process &b)
{
    return a.arrivalTime < b.arrivalTime;
}

bool comparePriority(const Process &a, const Process &b)
{
    return a.priority > b.priority;
}

void FCFS(vector<Process> &processes)
{
    sort(processes.begin(), processes.end(), compareArrivalTime);

    int currentTime = 0;
    for (int i = 0; i < processes.size(); ++i)
    {
        currentTime = max(currentTime, processes[i].arrivalTime);
        processes[i].startTime = currentTime;
        processes[i].completionTime = currentTime + processes[i].burstTime;
        processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
        processes[i].responseTime = processes[i].startTime - processes[i].arrivalTime;
        currentTime = processes[i].completionTime;
    }
}

void SJF_non_preemptive(vector<Process> &processes)
{
    sort(processes.begin(), processes.end(), compareArrivalTime);

    int currentTime = 0;
    for (int i = 0; i < processes.size(); ++i)
    {
        currentTime = max(currentTime, processes[i].arrivalTime);
        int shortestJobIndex = i;
        for (int j = i + 1; j < processes.size() && processes[j].arrivalTime <= currentTime; ++j)
        {
            if (processes[j].burstTime < processes[shortestJobIndex].burstTime)
                shortestJobIndex = j;
        }
        swap(processes[i], processes[shortestJobIndex]);
        processes[i].startTime = currentTime;
        processes[i].completionTime = currentTime + processes[i].burstTime;
        processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
        processes[i].responseTime = processes[i].startTime - processes[i].arrivalTime;
        currentTime = processes[i].completionTime;
    }
}

void priorityScheduling(vector<Process> &processes)
{
    sort(processes.begin(), processes.end(), compareArrivalTime);

    int currentTime = 0;
    int completed = 0;
    while (completed < processes.size())
    {
        int highestPriorityIndex = -1;
        int highestPriority = INT_MAX;
        for (int i = 0; i < processes.size(); ++i)
        {
            if (processes[i].arrivalTime <= currentTime && processes[i].remainingTime > 0 && processes[i].priority < highestPriority)
            {
                highestPriority = processes[i].priority;
                highestPriorityIndex = i;
            }
        }
        if (highestPriorityIndex == -1)
        {
            ++currentTime;
            continue;
        }
        processes[highestPriorityIndex].startTime = currentTime;
        processes[highestPriorityIndex].completionTime = currentTime + 1;
        --processes[highestPriorityIndex].remainingTime;
        if (processes[highestPriorityIndex].remainingTime == 0)
            ++completed;
        currentTime = processes[highestPriorityIndex].completionTime;
    }
    for (int i = 0; i < processes.size(); ++i)
    {
        processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime;
        processes[i].waitingTime = processes[i].turnaroundTime - processes[i].burstTime;
        processes[i].responseTime = processes[i].startTime - processes[i].arrivalTime;
    }
}

struct CompareRemainingTime
{
    bool operator()(const Process &a, const Process &b) const
    {
        return a.remainingTime > b.remainingTime;
    }
};

void SJF_preemptive(vector<Process> &processes)
{
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b)
         { return a.arrivalTime < b.arrivalTime; });

    int currentTime = 0;
    int completed = 0;
    priority_queue<Process, vector<Process>, CompareRemainingTime> pq;

    while (completed < processes.size() || !pq.empty())
    {
        while (!pq.empty() && pq.top().remainingTime == 0)
        {
            Process completedProcess = pq.top();
            pq.pop();
            completedProcess.completionTime = currentTime;
            completedProcess.turnaroundTime = completedProcess.completionTime - completedProcess.arrivalTime;
            completedProcess.waitingTime = completedProcess.turnaroundTime - completedProcess.burstTime;
            completedProcess.responseTime = completedProcess.startTime - completedProcess.arrivalTime;
            ++completed;
            processes[completedProcess.id - 1] = completedProcess;
        }

        while (completed < processes.size() && processes[completed].arrivalTime <= currentTime)
        {
            pq.push(processes[completed]);
            ++completed;
        }

        if (!pq.empty())
        {
            Process currentProcess = pq.top();
            pq.pop();
            if (currentProcess.startTime == -1)
            {
                currentProcess.startTime = currentTime;
            }
            currentProcess.remainingTime--;
            currentTime++;
            pq.push(currentProcess);
        }
        else
        {
            currentTime = processes[completed].arrivalTime;
        }
    }
}

void roundRobin(vector<Process> &processes, int quantum)
{
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b)
         { return a.arrivalTime < b.arrivalTime; });

    int n = processes.size();
    vector<int> remainingTime(n);
    vector<int> startTime(n, -1);
    vector<int> completed(n, 0);
    queue<int> readyQueue;

    for (int i = 0; i < n; ++i)
    {
        remainingTime[i] = processes[i].burstTime;
    }

    int currentTime = 0;
    int completedCount = 0;
    int idx = 0;

    while (completedCount < n)
    {
        while (idx < n && processes[idx].arrivalTime <= currentTime)
        {
            readyQueue.push(idx);
            idx++;
        }

        if (readyQueue.empty())
        {
            currentTime++;
            continue;
        }

        int currentProcess = readyQueue.front();
        readyQueue.pop();
        if (startTime[currentProcess] == -1)
        {
            startTime[currentProcess] = currentTime;
        }

        int executeTime = min(quantum, remainingTime[currentProcess]);
        remainingTime[currentProcess] -= executeTime;
        currentTime += executeTime;

        if (remainingTime[currentProcess] == 0)
        {
            completed[currentProcess] = 1;
            processes[currentProcess].completionTime = currentTime;
            processes[currentProcess].turnaroundTime = processes[currentProcess].completionTime - processes[currentProcess].arrivalTime;
            processes[currentProcess].waitingTime = processes[currentProcess].turnaroundTime - processes[currentProcess].burstTime;
            processes[currentProcess].responseTime = startTime[currentProcess] - processes[currentProcess].arrivalTime;
            completedCount++;
        }
        else
        {
            readyQueue.push(currentProcess);
        }
    }
}

void SRTF(vector<Process> &processes)
{
    sort(processes.begin(), processes.end(), [](const Process &a, const Process &b)
         { return a.arrivalTime < b.arrivalTime; });

    int n = processes.size();
    int currentTime = 0;
    int completed = 0;
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    vector<int> startTime(n, -1);

    while (completed < n)
    {

        while (completed < n && processes[completed].arrivalTime <= currentTime)
        {
            pq.push({processes[completed].burstTime, completed});
            completed++;
        }

        if (!pq.empty())
        {
            int id = pq.top().second;
            pq.pop();

            if (startTime[id] == -1)
            {
                startTime[id] = currentTime;
            }

            processes[id].remainingTime--;

            if (processes[id].remainingTime == 0)
            {
                processes[id].completionTime = currentTime + 1;
                processes[id].turnaroundTime = processes[id].completionTime - processes[id].arrivalTime;
                processes[id].waitingTime = processes[id].turnaroundTime - processes[id].burstTime;
                processes[id].responseTime = startTime[id] - processes[id].arrivalTime;
                completed++;
            }
            else
            {
                pq.push({processes[id].remainingTime, id});
            }
        }

        currentTime++;
    }
}

int main()
{
    int n;
    cout << "Enter the number of processes: ";
    cin >> n;

    vector<Process> processes(n);

    for (int i = 0; i < n; ++i)
    {
        cout << "Enter arrival time for process " << i + 1 << ": ";
        cin >> processes[i].arrivalTime;
        cout << "Enter burst time for process " << i + 1 << ": ";
        cin >> processes[i].burstTime;
        processes[i].id = i + 1;
        processes[i].remainingTime = processes[i].burstTime;
        processes[i].startTime = -1;
    }

    int choice;
    cout << "Select scheduling algorithm:\n";
    cout << "1. FCFS\n";
    cout << "2. SJF (Non-preemptive)\n";
    cout << "3. Priority Scheduling\n";
    cout << "4. SJF (Preemptive)\n";
    cout << "5. Round Robin\n";
    cout << "6. SRTF\n";
    cout << "Enter your choice: ";
    cin >> choice;

    switch (choice)
    {
    case 1:
        FCFS(processes);
        break;
    case 2:
        SJF_non_preemptive(processes);
        break;
    case 3:
        for (int i = 0; i < n; ++i)
        {
            cout << "Enter priority for process " << i + 1 << ": ";
            cin >> processes[i].priority;
        }
        priorityScheduling(processes);
        break;
    case 4:
        SJF_preemptive(processes);
        break;
    case 5:
    {
        int quantum;
        cout << "Enter time quantum: ";
        cin >> quantum;
        roundRobin(processes, quantum);
        break;
    }
    case 6:
        SRTF(processes);
        break;
    default:
        cout << "Invalid choice\n";
        return 1;
    }

    double totalWaitingTime = 0, totalTurnaroundTime = 0, totalResponseTime = 0;
    for (int i = 0; i < n; ++i)
    {
        totalWaitingTime += processes[i].waitingTime;
        totalTurnaroundTime += processes[i].turnaroundTime;
        totalResponseTime += processes[i].responseTime;
    }

    cout << "Average Waiting Time: " << totalWaitingTime / n << endl;
    cout << "Average Turnaround Time: " << totalTurnaroundTime / n << endl;
    cout << "Average Response Time: " << totalResponseTime / n << endl;

    return 0;
}