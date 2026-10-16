
#include <stdio.h>
#include <stdbool.h>

#define MAX_PROCESSES 20
#define MAX_RESOURCES 20

typedef struct {
    int processes;
    int resources;
    int allocation[MAX_PROCESSES][MAX_RESOURCES];
    int maximum[MAX_PROCESSES][MAX_RESOURCES];
    int available[MAX_RESOURCES];
} SystemState;

static void calculate_need(const SystemState *s,
                           int need[MAX_PROCESSES][MAX_RESOURCES])
{
    for (int i = 0; i < s->processes; i++) {
        for (int j = 0; j < s->resources; j++) {
            need[i][j] = s->maximum[i][j] - s->allocation[i][j];
        }
    }
}

static void display_matrix(const char *title, int matrix[MAX_PROCESSES][MAX_RESOURCES],
                           int processes, int resources)
{
    printf("\n%s\n", title);
    printf("       ");
    for (int j = 0; j < resources; j++) {
        printf("R%-6d", j);
    }
    printf("\n");

    for (int i = 0; i < processes; i++) {
        printf("P%-6d", i);
        for (int j = 0; j < resources; j++) {
            printf("%-7d", matrix[i][j]);
        }
        printf("\n");
    }
}

static bool read_int(const char *prompt, int *value)
{
    printf("%s", prompt);
    if (scanf("%d", value) != 1) {
        printf("Invalid input. Please enter an integer.\n");
        return false;
    }
    return true;
}

static bool read_system_state(SystemState *s)
{
    if (!read_int("Enter number of processes (1-20): ", &s->processes) ||
        !read_int("Enter number of resource types (1-20): ", &s->resources)) {
        return false;
    }

    if (s->processes < 1 || s->processes > MAX_PROCESSES ||
        s->resources < 1 || s->resources > MAX_RESOURCES) {
        printf("Invalid dimensions. Values must be between 1 and 20.\n");
        return false;
    }

    printf("\nEnter Allocation Matrix (%d x %d):\n",
           s->processes, s->resources);
    for (int i = 0; i < s->processes; i++) {
        printf("P%d: ", i);
        for (int j = 0; j < s->resources; j++) {
            if (scanf("%d", &s->allocation[i][j]) != 1 ||
                s->allocation[i][j] < 0) {
                printf("Allocation values must be non-negative integers.\n");
                return false;
            }
        }
    }

    printf("\nEnter Maximum Matrix (%d x %d):\n",
           s->processes, s->resources);
    for (int i = 0; i < s->processes; i++) {
        printf("P%d: ", i);
        for (int j = 0; j < s->resources; j++) {
            if (scanf("%d", &s->maximum[i][j]) != 1 ||
                s->maximum[i][j] < s->allocation[i][j]) {
                printf("Maximum must be an integer >= Allocation.\n");
                return false;
            }
        }
    }

    printf("\nEnter Available Resources (%d values): ", s->resources);
    for (int j = 0; j < s->resources; j++) {
        if (scanf("%d", &s->available[j]) != 1 ||
            s->available[j] < 0) {
            printf("Available resources must be non-negative integers.\n");
            return false;
        }
    }

    return true;
}

/* Returns true if the state is safe and optionally fills safe_sequence. */
static bool safety_algorithm(const SystemState *s, int *safe_sequence)
{
    int need[MAX_PROCESSES][MAX_RESOURCES];
    int work[MAX_RESOURCES];
    bool finish[MAX_PROCESSES] = {false};
    int completed = 0;

    calculate_need(s, need);

    for (int j = 0; j < s->resources; j++) {
        work[j] = s->available[j];
    }

    printf("\nInitial Work vector: ");
    for (int j = 0; j < s->resources; j++) {
        printf("%d ", work[j]);
    }
    printf("\n");

    while (completed < s->processes) {
        bool found = false;

        for (int i = 0; i < s->processes; i++) {
            if (finish[i]) {
                continue;
            }

            bool can_finish = true;
            for (int j = 0; j < s->resources; j++) {
                if (need[i][j] > work[j]) {
                    can_finish = false;
                    break;
                }
            }

            if (can_finish) {
                printf("P%d can finish; releasing its allocated resources.\n", i);

                for (int j = 0; j < s->resources; j++) {
                    work[j] += s->allocation[i][j];
                }

                finish[i] = true;
                safe_sequence[completed++] = i;
                found = true;

                printf("Work after P%d finishes: ", i);
                for (int j = 0; j < s->resources; j++) {
                    printf("%d ", work[j]);
                }
                printf("\n");
            }
        }

        if (!found) {
            break;
        }
    }

    if (completed == s->processes) {
        printf("\nSystem is in a SAFE state.\nSafe sequence: ");
        for (int i = 0; i < s->processes; i++) {
            printf("P%d%s", safe_sequence[i],
                   i == s->processes - 1 ? "\n" : " -> ");
        }
        return true;
    }

    printf("\nSystem is in an UNSAFE state.\n");
    printf("Processes that could not finish: ");
    for (int i = 0; i < s->processes; i++) {
        if (!finish[i]) {
            printf("P%d ", i);
        }
    }
    printf("\n");
    return false;
}

static void run_safety_algorithm(void)
{
    SystemState s = {0};
    int safe_sequence[MAX_PROCESSES];

    if (!read_system_state(&s)) {
        return;
    }

    int need[MAX_PROCESSES][MAX_RESOURCES];
    calculate_need(&s, need);

    display_matrix("Allocation Matrix:", s.allocation,
                   s.processes, s.resources);
    display_matrix("Maximum Matrix:", s.maximum,
                   s.processes, s.resources);
    display_matrix("Need Matrix:", need, s.processes, s.resources);

    printf("\nAvailable Vector: ");
    for (int j = 0; j < s.resources; j++) {
        printf("%d ", s.available[j]);
    }
    printf("\n");

    safety_algorithm(&s, safe_sequence);
}

static void run_resource_request_algorithm(void)
{
    SystemState s = {0};
    int safe_sequence[MAX_PROCESSES];
    int process;
    int request[MAX_RESOURCES];

    if (!read_system_state(&s)) {
        return;
    }

    if (!read_int("\nEnter requesting process number: ", &process)) {
        return;
    }

    if (process < 0 || process >= s.processes) {
        printf("Invalid process number.\n");
        return;
    }

    printf("Enter request vector for P%d: ", process);
    for (int j = 0; j < s.resources; j++) {
        if (scanf("%d", &request[j]) != 1 || request[j] < 0) {
            printf("Request values must be non-negative integers.\n");
            return;
        }
    }

    int need[MAX_PROCESSES][MAX_RESOURCES];
    calculate_need(&s, need);

    for (int j = 0; j < s.resources; j++) {
        if (request[j] > need[process][j]) {
            printf("Request denied: exceeds the process's remaining need.\n");
            return;
        }
    }

    for (int j = 0; j < s.resources; j++) {
        if (request[j] > s.available[j]) {
            printf("Request cannot be granted now: insufficient available resources.\n");
            return;
        }
    }

    /* Save the original state before tentative allocation. */
    SystemState original = s;

    for (int j = 0; j < s.resources; j++) {
        s.available[j] -= request[j];
        s.allocation[process][j] += request[j];
    }

    printf("\nRequest tentatively allocated to P%d.\n", process);

    if (safety_algorithm(&s, safe_sequence)) {
        printf("Request GRANTED. The system remains safe.\n");
        printf("Updated Available Vector: ");
        for (int j = 0; j < s.resources; j++) {
            printf("%d ", s.available[j]);
        }
        printf("\n");
    } else {
        s = original;
        printf("Request DENIED. The tentative allocation was rolled back.\n");
        printf("Original Available Vector restored: ");
        for (int j = 0; j < s.resources; j++) {
            printf("%d ", s.available[j]);
        }
        printf("\n");
    }
}

int main(void)
{
    int choice;

    while (1) {
        printf("\n========================================\n");
        printf("       BANKER'S ALGORITHM SIMULATOR\n");
        printf("========================================\n");
        printf("1. Safety Algorithm\n");
        printf("2. Resource Request Algorithm\n");
        printf("0. Exit\n");

        if (!read_int("Enter your choice: ", &choice)) {
            return 1;
        }

        switch (choice) {
            case 1:
                run_safety_algorithm();
                break;
            case 2:
                run_resource_request_algorithm();
                break;
            case 0:
                printf("Exiting Banker's Algorithm Simulator.\n");
                return 0;
            default:
                printf("Invalid choice. Select 1, 2, or 0.\n");
        }
    }
}

