#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ELEMENTS 100
#define MAX_DIMENSION 10
#define MAX_PATH_LENGTH 1024

typedef struct {
    const int *array;
    int start;
    int end;
    long long partial_sum;
} SumTask;

void *calculate_partial_sum(void *arg) {
    SumTask *task = (SumTask *)arg;
    task->partial_sum = 0;

    for (int i = task->start; i < task->end; i++) {
        task->partial_sum += task->array[i];
    }

    return NULL;
}

void parallel_sum(void) {
    int array[MAX_ELEMENTS];
    pthread_t threads[MAX_ELEMENTS];
    SumTask tasks[MAX_ELEMENTS];
    int n, thread_count;

    printf("\n--- Parallel Sum of Array Elements ---\n");
    printf("Enter number of array elements (1-%d): ", MAX_ELEMENTS);

    if (scanf("%d", &n) != 1 || n < 1 || n > MAX_ELEMENTS) {
        printf("Invalid array size.\n");
        return;
    }

    printf("Enter %d integer elements:\n", n);

    for (int i = 0; i < n; i++) {
        if (scanf("%d", &array[i]) != 1) {
            printf("Invalid array element.\n");
            return;
        }
    }

    printf("Enter number of threads (1-%d): ", n);

    if (scanf("%d", &thread_count) != 1 ||
        thread_count < 1 || thread_count > n) {
        printf("Invalid thread count.\n");
        return;
    }

    int base_size = n / thread_count;
    int remainder = n % thread_count;
    int start = 0;
    int created = 0;

    for (int i = 0; i < thread_count; i++) {
        int chunk_size = base_size + (i < remainder ? 1 : 0);

        tasks[i].array = array;
        tasks[i].start = start;
        tasks[i].end = start + chunk_size;
        tasks[i].partial_sum = 0;

        start += chunk_size;

        int error = pthread_create(
            &threads[i], NULL, calculate_partial_sum, &tasks[i]
        );

        if (error != 0) {
            fprintf(stderr, "Could not create thread %d (error %d).\n",
                    i + 1, error);
            break;
        }

        created++;
    }

    long long total_sum = 0;
    int join_failed = 0;

    for (int i = 0; i < created; i++) {
        int error = pthread_join(threads[i], NULL);

        if (error != 0) {
            fprintf(stderr, "Could not join thread %d (error %d).\n",
                    i + 1, error);
            join_failed = 1;
            continue;
        }

        printf("Thread %d: elements [%d, %d), partial sum = %lld\n",
               i + 1, tasks[i].start, tasks[i].end,
               tasks[i].partial_sum);

        total_sum += tasks[i].partial_sum;
    }

    if (created == thread_count && !join_failed) {
        printf("Total sum = %lld\n", total_sum);
    } else {
        printf("Total sum not reported because thread execution failed.\n");
    }
}

typedef struct {
    const int (*a)[MAX_DIMENSION];
    const int (*b)[MAX_DIMENSION];
    int (*result)[MAX_DIMENSION];
    int columns;
    int start_row;
    int end_row;
} MatrixTask;

void read_matrix(int matrix[MAX_DIMENSION][MAX_DIMENSION],
                 int rows, int columns, const char *name) {
    printf("Enter elements of Matrix %s (%d x %d):\n",
           name, rows, columns);

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%s[%d][%d]: ", name, i, j);

            if (scanf("%d", &matrix[i][j]) != 1) {
                printf("Invalid matrix element.\n");
                exit(EXIT_FAILURE);
            }
        }
    }
}

void print_matrix(const int matrix[MAX_DIMENSION][MAX_DIMENSION],
                  int rows, int columns) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < columns; j++) {
            printf("%d\t", matrix[i][j]);
        }
        printf("\n");
    }
}

void *add_matrix_rows(void *arg) {
    MatrixTask *task = (MatrixTask *)arg;

    for (int i = task->start_row; i < task->end_row; i++) {
        for (int j = 0; j < task->columns; j++) {
            task->result[i][j] = task->a[i][j] + task->b[i][j];
        }
    }

    return NULL;
}

void matrix_addition(void) {
    int a[MAX_DIMENSION][MAX_DIMENSION] = {0};
    int b[MAX_DIMENSION][MAX_DIMENSION] = {0};
    int result[MAX_DIMENSION][MAX_DIMENSION] = {0};
    pthread_t threads[MAX_DIMENSION];
    MatrixTask tasks[MAX_DIMENSION];

    int rows, columns, thread_count;

    printf("\n--- Matrix Addition Using Threads ---\n");
    printf("Enter number of rows (1-%d): ", MAX_DIMENSION);

    if (scanf("%d", &rows) != 1 ||
        rows < 1 || rows > MAX_DIMENSION) {
        printf("Invalid row count.\n");
        return;
    }

    printf("Enter number of columns (1-%d): ", MAX_DIMENSION);

    if (scanf("%d", &columns) != 1 ||
        columns < 1 || columns > MAX_DIMENSION) {
        printf("Invalid column count.\n");
        return;
    }

    read_matrix(a, rows, columns, "A");
    read_matrix(b, rows, columns, "B");

    printf("Enter number of threads (1-%d): ", rows);

    if (scanf("%d", &thread_count) != 1 ||
        thread_count < 1 || thread_count > rows) {
        printf("Invalid thread count.\n");
        return;
    }

    int base_size = rows / thread_count;
    int remainder = rows % thread_count;
    int start = 0;
    int created = 0;

    for (int i = 0; i < thread_count; i++) {
        int chunk_size = base_size + (i < remainder ? 1 : 0);

        tasks[i].a = a;
        tasks[i].b = b;
        tasks[i].result = result;
        tasks[i].columns = columns;
        tasks[i].start_row = start;
        tasks[i].end_row = start + chunk_size;

        start += chunk_size;

        int error = pthread_create(
            &threads[i], NULL, add_matrix_rows, &tasks[i]
        );

        if (error != 0) {
            fprintf(stderr, "Could not create thread %d (error %d).\n",
                    i + 1, error);
            break;
        }

        created++;
    }

    int join_failed = 0;

    for (int i = 0; i < created; i++) {
        int error = pthread_join(threads[i], NULL);

        if (error != 0) {
            fprintf(stderr, "Could not join thread %d (error %d).\n",
                    i + 1, error);
            join_failed = 1;
        }
    }

    if (created != thread_count || join_failed) {
        printf("Matrix addition could not be completed successfully.\n");
        return;
    }

    printf("\nMatrix A:\n");
    print_matrix(a, rows, columns);

    printf("\nMatrix B:\n");
    print_matrix(b, rows, columns);

    printf("\nResult of Matrix Addition:\n");
    print_matrix(result, rows, columns);
}

typedef struct {
    const int (*a)[MAX_DIMENSION];
    const int (*b)[MAX_DIMENSION];
    long long (*result)[MAX_DIMENSION];
    int rows_a;
    int columns_a;
    int columns_b;
    int start_row;
    int end_row;
} MultiplyTask;

void *multiply_matrix_rows(void *arg) {
    MultiplyTask *task = (MultiplyTask *)arg;

    for (int i = task->start_row; i < task->end_row; i++) {
        for (int j = 0; j < task->columns_b; j++) {
            long long sum = 0;

            for (int k = 0; k < task->columns_a; k++) {
                sum += (long long)task->a[i][k] * task->b[k][j];
            }

            task->result[i][j] = sum;
        }
    }

    return NULL;
}

void matrix_multiplication(void) {
    int a[MAX_DIMENSION][MAX_DIMENSION] = {0};
    int b[MAX_DIMENSION][MAX_DIMENSION] = {0};
    long long result[MAX_DIMENSION][MAX_DIMENSION] = {0};

    pthread_t threads[MAX_DIMENSION];
    MultiplyTask tasks[MAX_DIMENSION];

    int rows_a, columns_a, rows_b, columns_b, thread_count;

    printf("\n--- Matrix Multiplication Using Threads ---\n");

    printf("Enter rows of Matrix A (1-%d): ", MAX_DIMENSION);
    if (scanf("%d", &rows_a) != 1 ||
        rows_a < 1 || rows_a > MAX_DIMENSION) {
        printf("Invalid row count.\n");
        return;
    }

    printf("Enter columns of Matrix A (1-%d): ", MAX_DIMENSION);
    if (scanf("%d", &columns_a) != 1 ||
        columns_a < 1 || columns_a > MAX_DIMENSION) {
        printf("Invalid column count.\n");
        return;
    }

    printf("Enter rows of Matrix B (1-%d): ", MAX_DIMENSION);
    if (scanf("%d", &rows_b) != 1 ||
        rows_b < 1 || rows_b > MAX_DIMENSION) {
        printf("Invalid row count.\n");
        return;
    }

    printf("Enter columns of Matrix B (1-%d): ", MAX_DIMENSION);
    if (scanf("%d", &columns_b) != 1 ||
        columns_b < 1 || columns_b > MAX_DIMENSION) {
        printf("Invalid column count.\n");
        return;
    }

    if (columns_a != rows_b) {
        printf("Multiplication is not possible: columns of A must equal rows of B.\n");
        return;
    }

    read_matrix(a, rows_a, columns_a, "A");
    read_matrix(b, rows_b, columns_b, "B");

    printf("Enter number of threads (1-%d): ", rows_a);

    if (scanf("%d", &thread_count) != 1 ||
        thread_count < 1 || thread_count > rows_a) {
        printf("Invalid thread count.\n");
        return;
    }

    int base_size = rows_a / thread_count;
    int remainder = rows_a % thread_count;
    int start = 0;
    int created = 0;

    for (int i = 0; i < thread_count; i++) {
        int chunk_size = base_size + (i < remainder ? 1 : 0);

        tasks[i].a = a;
        tasks[i].b = b;
        tasks[i].result = result;
        tasks[i].rows_a = rows_a;
        tasks[i].columns_a = columns_a;
        tasks[i].columns_b = columns_b;
        tasks[i].start_row = start;
        tasks[i].end_row = start + chunk_size;

        start += chunk_size;

        int error = pthread_create(
            &threads[i], NULL, multiply_matrix_rows, &tasks[i]
        );

        if (error != 0) {
            fprintf(stderr, "Could not create thread %d (error %d).\n",
                    i + 1, error);
            break;
        }

        created++;
    }

    int join_failed = 0;

    for (int i = 0; i < created; i++) {
        int error = pthread_join(threads[i], NULL);

        if (error != 0) {
            fprintf(stderr, "Could not join thread %d (error %d).\n",
                    i + 1, error);
            join_failed = 1;
        }
    }

    if (created != thread_count || join_failed) {
        printf("Matrix multiplication could not be completed successfully.\n");
        return;
    }

    printf("\nMatrix A:\n");
    print_matrix(a, rows_a, columns_a);

    printf("\nMatrix B:\n");
    print_matrix(b, rows_b, columns_b);

    printf("\nResult of Matrix Multiplication:\n");

    for (int i = 0; i < rows_a; i++) {
        for (int j = 0; j < columns_b; j++) {
            printf("%lld\t", result[i][j]);
        }
        printf("\n");
    }
}

typedef enum {
    COUNT_CHARACTERS,
    COUNT_WORDS,
    COUNT_LINES
} CountType;

typedef struct {
    const char *filename;
    CountType type;
    unsigned long long count;
    int success;
} FileTask;

void *count_file_data(void *arg) {
    FileTask *task = (FileTask *)arg;
    FILE *file = fopen(task->filename, "rb");

    task->count = 0;
    task->success = 0;

    if (file == NULL) {
        return NULL;
    }

    int ch;
    int in_word = 0;
    int last_char = '\0';
    int has_data = 0;

    while ((ch = fgetc(file)) != EOF) {
        has_data = 1;
        last_char = ch;

        if (task->type == COUNT_CHARACTERS) {
            task->count++;
        } else if (task->type == COUNT_LINES) {
            if (ch == '\n') {
                task->count++;
            }
        } else if (task->type == COUNT_WORDS) {
            if (isspace((unsigned char)ch)) {
                in_word = 0;
            } else if (!in_word) {
                task->count++;
                in_word = 1;
            }
        }
    }

    if (ferror(file)) {
        fclose(file);
        return NULL;
    }

    if (task->type == COUNT_LINES &&
        has_data && last_char != '\n') {
        task->count++;
    }

    if (fclose(file) != 0) {
        return NULL;
    }

    task->success = 1;
    return NULL;
}

void parallel_file_processing(void) {
    char filename[MAX_PATH_LENGTH];
    pthread_t threads[3];

    FileTask tasks[3] = {
        {NULL, COUNT_CHARACTERS, 0, 0},
        {NULL, COUNT_WORDS, 0, 0},
        {NULL, COUNT_LINES, 0, 0}
    };

    printf("\n--- Parallel File Processing ---\n");
    printf("Enter file path: ");

    /* Consume the newline left by the previous scanf call. */
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
        /* Discard remaining characters on the input line. */
    }

    if (fgets(filename, sizeof(filename), stdin) == NULL) {
        printf("Could not read file path.\n");
        return;
    }

    filename[strcspn(filename, "\n")] = '\0';

    if (filename[0] == '\0') {
        printf("File path cannot be empty.\n");
        return;
    }

    for (int i = 0; i < 3; i++) {
        tasks[i].filename = filename;
    }

    const char *names[] = {"Character-count", "Word-count", "Line-count"};
    int created = 0;

    for (int i = 0; i < 3; i++) {
        int error = pthread_create(
            &threads[i], NULL, count_file_data, &tasks[i]
        );

        if (error != 0) {
            fprintf(stderr, "Could not create %s thread (error %d).\n",
                    names[i], error);
            break;
        }

        created++;
    }

    int join_failed = 0;

    for (int i = 0; i < created; i++) {
        int error = pthread_join(threads[i], NULL);

        if (error != 0) {
            fprintf(stderr, "Could not join %s thread (error %d).\n",
                    names[i], error);
            join_failed = 1;
        }
    }

    if (created != 3 || join_failed) {
        printf("File processing could not be completed successfully.\n");
        return;
    }

    for (int i = 0; i < 3; i++) {
        if (!tasks[i].success) {
            printf("Could not read the file successfully: %s\n", filename);
            printf("Check that the path exists and the file is readable.\n");
            return;
        }
    }

    printf("\nFile: %s\n", filename);
    printf("Characters (bytes): %llu\n", tasks[0].count);
    printf("Words: %llu\n", tasks[1].count);
    printf("Lines: %llu\n", tasks[2].count);
}

int main(void) {
    int choice;

    do {
        printf("\n========================================\n");
        printf(" MULTITHREADED PROGRAMMING USING PTHREADS\n");
        printf("========================================\n");
        printf("1. Parallel Sum of Array Elements\n");
        printf("2. Matrix Addition Using Threads\n");
        printf("3. Matrix Multiplication Using Threads\n");
        printf("4. Parallel File Processing\n");
        printf("5. Exit\n");
        printf("========================================\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Exiting program.\n");
            return EXIT_FAILURE;
        }

        switch (choice) {
            case 1:
                parallel_sum();
                break;

            case 2:
                matrix_addition();
                break;

            case 3:
                matrix_multiplication();
                break;

            case 4:
                parallel_file_processing();
                break;

            case 5:
                printf("Exiting program.\n");
                break;

            default:
                printf("Invalid choice. Please select 1-5.\n");
        }

    } while (choice != 5);

    return EXIT_SUCCESS;
}
