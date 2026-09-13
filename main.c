#include <stdio.h>
#include <string.h>

#define MIN_PROCESS 3 
#define MAX_PROCESS 10 
#define ID_LENGTH 20 

typedef struct { 
    char id[ID_LENGTH]; 
    int arrivalTime; 
    int burstTime; 
    int startTime; 
    int completionTime; 
    int waitingTime; 
    int turnaroundTime;
    int originalIndex;
    int completed;
} Process; 

int isDuplicateID(Process processes[], int count, char id[]) { 
    for (int i = 0; i < count; i++) { 
        if (strcmp(processes[i].id, id) == 0) return 1; 
    } 
    return 0; 
} 

int isDuplicateArrival(Process processes[], int count, int arrivalTime) { 
    for (int i = 0; i < count; i++) { 
        if (processes[i].arrivalTime == arrivalTime) return 1; 
    } 
    return 0; 
} 

void sortByArrivalTime(Process processes[], int n) { 
    Process temp; 
    for (int i = 0; i < n - 1; i++) { 
        for (int j = 0; j < n - i - 1; j++) { 
            if (processes[j].arrivalTime > processes[j + 1].arrivalTime || 
               (processes[j].arrivalTime == processes[j + 1].arrivalTime && processes[j].originalIndex > processes[j + 1].originalIndex)) { 
                temp = processes[j]; 
                processes[j] = processes[j + 1]; 
                processes[j + 1] = temp; 
            } 
        } 
    } 
} 

void calculateFCFS(Process processes[], int n) { 
    sortByArrivalTime(processes, n);
    int currentTime = 0; 
    for (int i = 0; i < n; i++) { 
        if (currentTime < processes[i].arrivalTime) { 
            currentTime = processes[i].arrivalTime; 
        } 
        processes[i].startTime = currentTime; 
        processes[i].completionTime = processes[i].startTime + processes[i].burstTime; 
        processes[i].waitingTime = processes[i].startTime - processes[i].arrivalTime; 
        processes[i].turnaroundTime = processes[i].completionTime - processes[i].arrivalTime; 
        currentTime = processes[i].completionTime; 
    } 
} 

void calculateSJF(Process processes[], int n) {
    int currentTime = 0;
    int completedCount = 0;

    for (int i = 0; i < n; i++) {
        processes[i].completed = 0;
    }

    Process execOrder[MAX_PROCESS];
    int execIdx = 0;

    while (completedCount < n) {
        int idx = -1;
        int minBT = 99999;

        for (int i = 0; i < n; i++) {
            if (!processes[i].completed && processes[i].arrivalTime <= currentTime) {
                if (processes[i].burstTime < minBT) {
                    minBT = processes[i].burstTime;
                    idx = i;
                } else if (processes[i].burstTime == minBT) {
                    if (idx == -1 || processes[i].arrivalTime < processes[idx].arrivalTime || 
                       (processes[i].arrivalTime == processes[idx].arrivalTime && processes[i].originalIndex < processes[idx].originalIndex)) {
                        idx = i;
                    }
                }
            }
        }

        if (idx == -1) {
            int nextAT = 99999;
            for (int i = 0; i < n; i++) {
                if (!processes[i].completed && processes[i].arrivalTime < nextAT) {
                    nextAT = processes[i].arrivalTime;
                }
            }
            currentTime = nextAT;
        } else {
            if (currentTime < processes[idx].arrivalTime) {
                currentTime = processes[idx].arrivalTime;
            }
            processes[idx].startTime = currentTime;
            processes[idx].completionTime = processes[idx].startTime + processes[idx].burstTime;
            processes[idx].waitingTime = processes[idx].startTime - processes[idx].arrivalTime;
            processes[idx].turnaroundTime = processes[idx].completionTime - processes[idx].arrivalTime;
            processes[idx].completed = 1;

            currentTime = processes[idx].completionTime;
            execOrder[execIdx++] = processes[idx];
            completedCount++;
        }
    }

    for (int i = 0; i < n; i++) {
        processes[i] = execOrder[i];
    }
}

void displayReadyQueue(Process processes[], int n) { 
    int maxAT = processes[0].arrivalTime; 
    for (int i = 1; i < n; i++) { 
        if (processes[i].arrivalTime > maxAT) maxAT = processes[i].arrivalTime; 
    } 
    printf("\n============================================================\n"); 
    printf(" 1st STEP - RQ (READY QUEUE)\n"); 
    printf("============================================================\n\n"); 
    printf("%-5s %-10s %-10s\n", "T", "PID", "BT"); 
    printf("---------------------------\n"); 
    
    Process tempArr[MAX_PROCESS];
    for(int i=0; i<n; i++) tempArr[i] = processes[i];
    sortByArrivalTime(tempArr, n);

    for (int t = 0; t <= maxAT; t++) { 
        int foundIdx = -1; 
        for (int i = 0; i < n; i++) { 
            if (tempArr[i].arrivalTime == t) { foundIdx = i; break; } 
        } 
        if (foundIdx != -1) { 
            printf("%-5d %-10s %-10d\n", t, tempArr[foundIdx].id, tempArr[foundIdx].burstTime); 
        } else { 
            printf("%-5d %-10s %-10s\n", t, "-", "-"); 
        } 
    } 
} 

void displayGanttChart(Process processes[], int n) { 
    int totalBT = 0; 
    for (int i = 0; i < n; i++) totalBT += processes[i].burstTime; 
    printf("\n============================================================\n"); 
    printf(" 2nd STEP - GANTT CHART\n"); 
    printf("============================================================\n\n"); 
    
    printf("|"); 
    for (int i = 0; i < n; i++) printf("  %-5s |", processes[i].id); 
    printf("\n------------------------------------------------------------\n"); 
    
    printf("%d", processes[0].startTime); 
    for (int i = 0; i < n; i++) printf("%10d", processes[i].completionTime); 
    printf("\n\n"); 
    
    int lastET = processes[n - 1].completionTime; 
    printf("Checking: Total BT given = %d | Last ET = %d | Match: %s\n", totalBT, lastET, (totalBT == lastET) ? "YES" : "NO"); 
} 

void displayWaitingTime(Process processes[], int n) { 
    float totalWT = 0; 
    printf("\n============================================================\n"); 
    printf(" 3rd STEP - WT (WAITING TIME)\n"); 
    printf("============================================================\n"); 
    printf("Formula: WT = ST - AT\n\n"); 
    for (int i = 0; i < n; i++) { 
        printf("%-5s = %d - %d = %d ms\n", processes[i].id, processes[i].startTime, processes[i].arrivalTime, processes[i].waitingTime); 
        totalWT += processes[i].waitingTime; 
    } 
    printf("\nAWT (Average Waiting Time):\n"); 
    for (int i = 0; i < n; i++) printf("%d%s", processes[i].waitingTime, (i == n - 1) ? "" : " + "); 
    printf(" = %.0f / %d = %.2f ms\n", totalWT, n, totalWT / n); 
} 

void displayTurnaroundTime(Process processes[], int n) { 
    float totalTAT = 0; 
    printf("\n============================================================\n"); 
    printf(" 4th STEP - TAT (TURNAROUND TIME)\n"); 
    printf("============================================================\n"); 
    printf("Options: ET - AT  OR  WT + BT\n\n"); 
    printf("%-8s %-16s %-16s %-8s\n", "PID", "ET - AT", "WT + BT", "TAT"); 
    printf("----------------------------------------------------\n"); 
    for (int i = 0; i < n; i++) { 
        int option1 = processes[i].completionTime - processes[i].arrivalTime; 
        int option2 = processes[i].waitingTime + processes[i].burstTime; 
        printf("%-8s %d - %d = %-8d %d + %d = %-8d %-8d\n", 
               processes[i].id, processes[i].completionTime, processes[i].arrivalTime, option1, 
               processes[i].waitingTime, processes[i].burstTime, option2, processes[i].turnaroundTime); 
        totalTAT += processes[i].turnaroundTime; 
    } 
    printf("\nATAT (Average Turnaround Time):\n"); 
    for (int i = 0; i < n; i++) printf("%d%s", processes[i].turnaroundTime, (i == n - 1) ? "" : " + "); 
    printf(" = %.0f / %d = %.2f ms\n", totalTAT, n, totalTAT / n); 
} 

int main() { 
    Process processes[MAX_PROCESS]; 
    int n, choice; 
    
    printf("\n============================================================\n"); 
    printf("              CPU SCHEDULING SIMULATOR (MP2)\n"); 
    printf("============================================================\n"); 
    
    do { 
        printf("Enter the no. of process: "); 
        scanf("%d", &n); 
        if (n < MIN_PROCESS || n > MAX_PROCESS) { 
            printf("\nERROR: Please enter between 3 and 10 processes.\n\n"); 
        } 
    } while (n < MIN_PROCESS || n > MAX_PROCESS); 
    
    for (int i = 0; i < n; i++) { 
        processes[i].originalIndex = i;
        do { 
            printf("Enter process ID for Process%d: ", i + 1); 
            scanf("%19s", processes[i].id); 
            if (isDuplicateID(processes, i, processes[i].id)) printf("ERROR: Process ID already exists!\n"); 
        } while (isDuplicateID(processes, i, processes[i].id)); 
    } 

    for (int i = 0; i < n; i++) { 
        do { 
            printf("Enter waiting time for %s: ", processes[i].id); 
            scanf("%d", &processes[i].arrivalTime); 
            if (processes[i].arrivalTime < 0) { 
                printf("ERROR: Waiting time cannot be negative!\n"); 
            } else if (isDuplicateArrival(processes, i, processes[i].arrivalTime)) { 
                printf("ERROR: Waiting time must be unique!\n"); 
            } 
        } while (processes[i].arrivalTime < 0 || isDuplicateArrival(processes, i, processes[i].arrivalTime)); 
    } 

    for (int i = 0; i < n; i++) { 
        do { 
            printf("Enter burst time for %s: ", processes[i].id); 
            scanf("%d", &processes[i].burstTime); 
            if (processes[i].burstTime <= 0) printf("ERROR: Burst time must be greater than 0!\n"); 
        } while (processes[i].burstTime <= 0); 
    } 

    do {
        printf("Select the CPU scheduling algorithm 1 - FCFS and 2 - SJF : ");
        scanf("%d", &choice);
        if (choice != 1 && choice != 2) printf("ERROR: Invalid selection! Please enter 1 or 2.\n");
    } while (choice != 1 && choice != 2);

    if (choice == 1) {
        calculateFCFS(processes, n);
    } else {
        calculateSJF(processes, n);
    }

    float total_wt = 0, total_tat = 0;
    for(int i = 0; i < n; i++) {
        total_wt += processes[i].waitingTime;
        total_tat += processes[i].turnaroundTime;
    }
    printf("Average waiting: %.1f\n", total_wt / n);
    printf("Average turn-around time: %.1f\n", total_tat / n);

    displayReadyQueue(processes, n); 
    displayGanttChart(processes, n); 
    displayWaitingTime(processes, n); 
    displayTurnaroundTime(processes, n); 
    
    return 0; 
}