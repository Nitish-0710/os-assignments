#define _POSIX_C_SOURCE 200809L

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define TEXT_SIZE 256
#define DATA_SIZE 4096
#define FIFO_BASIC "/tmp/os_assignment5_basic_fifo"
#define FIFO_REQUEST "/tmp/os_assignment5_request_fifo"
#define FIFO_RESPONSE "/tmp/os_assignment5_response_fifo"

typedef struct {
  long type;
  char text[TEXT_SIZE];
} Message;

typedef struct {
  int ready;
  int first;
  int second;
  char operation;
  int result;
  int success;
} SharedArithmetic;

typedef struct {
  size_t size;
  char data[DATA_SIZE];
} SharedFileBuffer;

/* ---------- General utility functions ---------- */

static int read_line(const char *prompt, char *buffer, size_t size) {
  printf("%s", prompt);
  fflush(stdout);

  if (fgets(buffer, (int)size, stdin) == NULL) {
    return 0;
  }

  size_t length = strlen(buffer);

  if (length > 0 && buffer[length - 1] == '\n') {
    buffer[length - 1] = '\0';
  } else {
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
  }

  return 1;
}

static int read_integer(const char *prompt, int *value) {
  char buffer[TEXT_SIZE];
  char *end;
  long number;

  if (!read_line(prompt, buffer, sizeof(buffer))) {
    return 0;
  }

  errno = 0;
  number = strtol(buffer, &end, 10);

  while (isspace((unsigned char)*end)) {
    end++;
  }

  if (buffer[0] == '\0' || *end != '\0' || errno == ERANGE ||
      number < INT_MIN || number > INT_MAX) {
    printf("Invalid integer. Please try again.\n");
    return -1;
  }

  *value = (int)number;
  return 1;
}

static int read_path(const char *prompt, char *path, size_t size) {
  if (!read_line(prompt, path, size)) {
    return 0;
  }

  if (path[0] == '\0') {
    printf("Path cannot be empty.\n");
    return -1;
  }

  return 1;
}

static int write_all(int fd, const void *data, size_t size) {
  const char *ptr = data;

  while (size > 0) {
    ssize_t written = write(fd, ptr, size);

    if (written < 0) {
      if (errno == EINTR) {
        continue;
      }
      return -1;
    }

    if (written == 0) {
      return -1;
    }

    ptr += written;
    size -= (size_t)written;
  }

  return 0;
}

static int read_exact(int fd, void *data, size_t size) {
  char *ptr = data;

  while (size > 0) {
    ssize_t received = read(fd, ptr, size);

    if (received < 0) {
      if (errno == EINTR) {
        continue;
      }
      return -1;
    }

    if (received == 0) {
      return -1;
    }

    ptr += received;
    size -= (size_t)received;
  }

  return 0;
}

static void wait_for_child(pid_t pid) {
  int status;
  pid_t result;

  do {
    result = waitpid(pid, &status, 0);
  } while (result < 0 && errno == EINTR);

  if (result < 0) {
    perror("waitpid");
  } else if (WIFEXITED(status) && WEXITSTATUS(status) != 0) {
    printf("Child process reported an error.\n");
  } else if (WIFSIGNALED(status)) {
    printf("Child process terminated by signal %d.\n", WTERMSIG(status));
  }
}

/* ---------- 1. Anonymous pipe communication ---------- */

static void anonymous_pipe(void) {
  int fd[2];
  pid_t pid;
  char message[] = "Hello from the parent process!";
  char buffer[TEXT_SIZE];
  ssize_t count;

  printf("\n--- Anonymous Pipe Communication ---\n");

  if (pipe(fd) == -1) {
    perror("pipe");
    return;
  }

  pid = fork();

  if (pid < 0) {
    perror("fork");
    close(fd[0]);
    close(fd[1]);
    return;
  }

  if (pid == 0) {
    close(fd[1]);

    count = read(fd[0], buffer, sizeof(buffer) - 1);
    if (count < 0) {
      perror("read");
      close(fd[0]);
      _exit(EXIT_FAILURE);
    }

    buffer[count] = '\0';
    printf("Child received: %s\n", buffer);
    fflush(stdout);

    close(fd[0]);
    _exit(EXIT_SUCCESS);
  }

  close(fd[0]);

  if (write_all(fd[1], message, strlen(message)) == -1) {
    perror("write");
  } else {
    printf("Parent sent: %s\n", message);
  }

  close(fd[1]);
  wait_for_child(pid);
}

/* ---------- 2. Bidirectional communication using two pipes ---------- */

static void bidirectional_pipes(void) {
  int parent_to_child[2];
  int child_to_parent[2];
  pid_t pid;
  char message[] = "Hello child!";
  char reply[] = "Hello parent!";
  char buffer[TEXT_SIZE];
  ssize_t count;

  printf("\n--- Bidirectional Communication Using Two Pipes ---\n");

  if (pipe(parent_to_child) == -1 || pipe(child_to_parent) == -1) {
    perror("pipe");
    return;
  }

  pid = fork();

  if (pid < 0) {
    perror("fork");
    return;
  }

  if (pid == 0) {
    close(parent_to_child[1]);
    close(child_to_parent[0]);

    count = read(parent_to_child[0], buffer, sizeof(buffer) - 1);
    if (count < 0) {
      perror("child read");
      _exit(EXIT_FAILURE);
    }

    buffer[count] = '\0';
    printf("Child received: %s\n", buffer);
    fflush(stdout);

    if (write_all(child_to_parent[1], reply, strlen(reply)) == -1) {
      perror("child write");
      _exit(EXIT_FAILURE);
    }

    close(parent_to_child[0]);
    close(child_to_parent[1]);
    _exit(EXIT_SUCCESS);
  }

  close(parent_to_child[0]);
  close(child_to_parent[1]);

  if (write_all(parent_to_child[1], message, strlen(message)) == -1) {
    perror("parent write");
  } else {
    printf("Parent sent: %s\n", message);
  }

  close(parent_to_child[1]);

  count = read(child_to_parent[0], buffer, sizeof(buffer) - 1);

  if (count < 0) {
    perror("parent read");
  } else {
    buffer[count] = '\0';
    printf("Parent received: %s\n", buffer);
  }

  close(child_to_parent[0]);
  wait_for_child(pid);
}

/* ---------- 3. File transfer using pipes ---------- */

static void pipe_file_transfer(void) {
  int fd[2];
  pid_t pid;
  char source[TEXT_SIZE];
  char destination[TEXT_SIZE];
  char buffer[DATA_SIZE];
  int input_fd, output_fd;
  ssize_t count;
  int failed = 0;

  printf("\n--- File Transfer Using Pipes ---\n");

  if (read_path("Source file path: ", source, sizeof(source)) != 1 ||
      read_path("Destination file path: ", destination, sizeof(destination)) !=
          1) {
    return;
  }

  if (strcmp(source, destination) == 0) {
    printf("Source and destination must be different.\n");
    return;
  }

  input_fd = open(source, O_RDONLY);
  if (input_fd < 0) {
    perror("open source");
    return;
  }

  if (pipe(fd) == -1) {
    perror("pipe");
    close(input_fd);
    return;
  }

  pid = fork();

  if (pid < 0) {
    perror("fork");
    close(input_fd);
    close(fd[0]);
    close(fd[1]);
    return;
  }

  if (pid == 0) {
    close(fd[1]);
    close(input_fd);

    output_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (output_fd < 0) {
      perror("open destination");
      close(fd[0]);
      _exit(EXIT_FAILURE);
    }

    while ((count = read(fd[0], buffer, sizeof(buffer))) > 0) {
      if (write_all(output_fd, buffer, (size_t)count) == -1) {
        perror("write destination");
        close(output_fd);
        close(fd[0]);
        _exit(EXIT_FAILURE);
      }
    }

    if (count < 0) {
      perror("read pipe");
      failed = 1;
    }

    close(output_fd);
    close(fd[0]);
    _exit(failed ? EXIT_FAILURE : EXIT_SUCCESS);
  }

  close(fd[0]);

  while ((count = read(input_fd, buffer, sizeof(buffer))) > 0) {
    if (write_all(fd[1], buffer, (size_t)count) == -1) {
      perror("write pipe");
      failed = 1;
      break;
    }
  }

  if (count < 0) {
    perror("read source");
    failed = 1;
  }

  close(input_fd);
  close(fd[1]);
  wait_for_child(pid);

  if (!failed) {
    printf("File transfer through the pipe completed.\n");
    printf("Destination: %s\n", destination);
  }
}

/* ---------- 4. Named pipe (FIFO) ---------- */

static void named_fifo(void) {
  pid_t pid;
  int fd;
  char message[] = "Message sent through a named FIFO.";
  char buffer[TEXT_SIZE];
  ssize_t count;

  printf("\n--- Named Pipe (FIFO) ---\n");

  unlink(FIFO_BASIC);

  if (mkfifo(FIFO_BASIC, 0600) == -1) {
    perror("mkfifo");
    return;
  }

  pid = fork();

  if (pid < 0) {
    perror("fork");
    unlink(FIFO_BASIC);
    return;
  }

  if (pid == 0) {
    fd = open(FIFO_BASIC, O_WRONLY);
    if (fd < 0) {
      perror("FIFO open writer");
      _exit(EXIT_FAILURE);
    }

    if (write_all(fd, message, strlen(message)) == -1) {
      perror("FIFO write");
      close(fd);
      _exit(EXIT_FAILURE);
    }

    printf("Writer sent: %s\n", message);
    fflush(stdout);
    close(fd);
    _exit(EXIT_SUCCESS);
  }

  fd = open(FIFO_BASIC, O_RDONLY);

  if (fd < 0) {
    perror("FIFO open reader");
    wait_for_child(pid);
    unlink(FIFO_BASIC);
    return;
  }

  count = read(fd, buffer, sizeof(buffer) - 1);

  if (count < 0) {
    perror("FIFO read");
  } else {
    buffer[count] = '\0';
    printf("Reader received: %s\n", buffer);
  }

  close(fd);
  wait_for_child(pid);
  unlink(FIFO_BASIC);
}

/* ---------- 5. Client-server communication using FIFO ---------- */

static void fifo_client_server(void) {
  pid_t pid;
  int request_fd, response_fd;
  char request[TEXT_SIZE];
  char response[TEXT_SIZE];
  ssize_t count;

  printf("\n--- Client-Server Communication Using FIFO ---\n");

  if (read_line("Enter a message for the server: ", request, sizeof(request)) !=
      1) {
    return;
  }

  unlink(FIFO_REQUEST);
  unlink(FIFO_RESPONSE);

  if (mkfifo(FIFO_REQUEST, 0600) == -1 || mkfifo(FIFO_RESPONSE, 0600) == -1) {
    perror("mkfifo");
    unlink(FIFO_REQUEST);
    unlink(FIFO_RESPONSE);
    return;
  }

  pid = fork();

  if (pid < 0) {
    perror("fork");
    unlink(FIFO_REQUEST);
    unlink(FIFO_RESPONSE);
    return;
  }

  if (pid == 0) {
    char received[TEXT_SIZE];

    request_fd = open(FIFO_REQUEST, O_RDONLY);
    if (request_fd < 0) {
      perror("server open request FIFO");
      _exit(EXIT_FAILURE);
    }

    count = read(request_fd, received, sizeof(received) - 1);
    close(request_fd);

    if (count < 0) {
      perror("server read");
      _exit(EXIT_FAILURE);
    }

    received[count] = '\0';

    response_fd = open(FIFO_RESPONSE, O_WRONLY);
    if (response_fd < 0) {
      perror("server open response FIFO");
      _exit(EXIT_FAILURE);
    }

    snprintf(response, sizeof(response), "Server processed: %.237s", received);

    if (write_all(response_fd, response, strlen(response)) == -1) {
      perror("server write");
      close(response_fd);
      _exit(EXIT_FAILURE);
    }

    close(response_fd);
    _exit(EXIT_SUCCESS);
  }

  request_fd = open(FIFO_REQUEST, O_WRONLY);

  if (request_fd < 0) {
    perror("client open request FIFO");
    wait_for_child(pid);
    unlink(FIFO_REQUEST);
    unlink(FIFO_RESPONSE);
    return;
  }

  if (write_all(request_fd, request, strlen(request)) == -1) {
    perror("client write");
  }

  close(request_fd);

  response_fd = open(FIFO_RESPONSE, O_RDONLY);

  if (response_fd < 0) {
    perror("client open response FIFO");
  } else {
    count = read(response_fd, response, sizeof(response) - 1);

    if (count < 0) {
      perror("client read");
    } else {
      response[count] = '\0';
      printf("Client received: %s\n", response);
    }

    close(response_fd);
  }

  wait_for_child(pid);
  unlink(FIFO_REQUEST);
  unlink(FIFO_RESPONSE);
}

/* ---------- 6. FIFO-based chat application ---------- */

static void fifo_chat(void) {
  pid_t pid;
  int to_server, to_client;
  char message[TEXT_SIZE];
  char response[TEXT_SIZE];
  ssize_t count;

  printf("\n--- FIFO-Based Chat Application ---\n");
  printf("This demonstration uses a client and a replying server.\n");

  if (read_line("Client: ", message, sizeof(message)) != 1) {
    return;
  }

  unlink(FIFO_REQUEST);
  unlink(FIFO_RESPONSE);

  if (mkfifo(FIFO_REQUEST, 0600) == -1 || mkfifo(FIFO_RESPONSE, 0600) == -1) {
    perror("mkfifo");
    unlink(FIFO_REQUEST);
    unlink(FIFO_RESPONSE);
    return;
  }

  pid = fork();

  if (pid < 0) {
    perror("fork");
    unlink(FIFO_REQUEST);
    unlink(FIFO_RESPONSE);
    return;
  }

  if (pid == 0) {
    char incoming[TEXT_SIZE];

    to_server = open(FIFO_REQUEST, O_RDONLY);
    if (to_server < 0) {
      perror("chat server open");
      _exit(EXIT_FAILURE);
    }

    count = read(to_server, incoming, sizeof(incoming) - 1);
    close(to_server);

    if (count < 0) {
      perror("chat server read");
      _exit(EXIT_FAILURE);
    }

    incoming[count] = '\0';
    printf("Server received: %s\n", incoming);
    fflush(stdout);

    snprintf(response, sizeof(response), "Acknowledged: %.241s", incoming);

    to_client = open(FIFO_RESPONSE, O_WRONLY);
    if (to_client < 0) {
      perror("chat server response open");
      _exit(EXIT_FAILURE);
    }

    if (write_all(to_client, response, strlen(response)) == -1) {
      perror("chat server write");
      close(to_client);
      _exit(EXIT_FAILURE);
    }

    close(to_client);
    _exit(EXIT_SUCCESS);
  }

  to_server = open(FIFO_REQUEST, O_WRONLY);

  if (to_server < 0) {
    perror("chat client open");
    wait_for_child(pid);
    unlink(FIFO_REQUEST);
    unlink(FIFO_RESPONSE);
    return;
  }

  if (write_all(to_server, message, strlen(message)) == -1) {
    perror("chat client write");
  }

  close(to_server);

  to_client = open(FIFO_RESPONSE, O_RDONLY);

  if (to_client < 0) {
    perror("chat client response open");
  } else {
    count = read(to_client, response, sizeof(response) - 1);

    if (count < 0) {
      perror("chat client read");
    } else {
      response[count] = '\0';
      printf("Server: %s\n", response);
    }

    close(to_client);
  }

  wait_for_child(pid);
  unlink(FIFO_REQUEST);
  unlink(FIFO_RESPONSE);
}

/* ---------- 7. Producer-consumer using message queues ---------- */

static void producer_consumer_queue(void) {
  int queue_id;
  pid_t pid;
  Message message;

  printf("\n--- Producer-Consumer Using Message Queues ---\n");

  queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
  if (queue_id == -1) {
    perror("msgget");
    return;
  }

  pid = fork();

  if (pid < 0) {
    perror("fork");
    msgctl(queue_id, IPC_RMID, NULL);
    return;
  }

  if (pid == 0) {
    if (msgrcv(queue_id, &message, sizeof(message.text), 1, 0) == -1) {
      perror("consumer msgrcv");
      _exit(EXIT_FAILURE);
    }

    printf("Consumer received: %s\n", message.text);
    fflush(stdout);
    _exit(EXIT_SUCCESS);
  }

  message.type = 1;
  snprintf(message.text, sizeof(message.text),
           "Item produced by the producer.");

  if (msgsnd(queue_id, &message, strlen(message.text) + 1, 0) == -1) {
    perror("producer msgsnd");
  } else {
    printf("Producer sent: %s\n", message.text);
  }

  wait_for_child(pid);

  if (msgctl(queue_id, IPC_RMID, NULL) == -1) {
    perror("msgctl");
  }
}

/* ---------- 8. System V message queue communication ---------- */

static void message_queue_communication(void) {
  int queue_id;
  pid_t pid;
  Message message;

  printf("\n--- System V Message Queue Communication ---\n");

  queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0600);
  if (queue_id == -1) {
    perror("msgget");
    return;
  }

  pid = fork();

  if (pid < 0) {
    perror("fork");
    msgctl(queue_id, IPC_RMID, NULL);
    return;
  }

  if (pid == 0) {
    if (msgrcv(queue_id, &message, sizeof(message.text), 1, 0) == -1) {
      perror("child msgrcv");
      _exit(EXIT_FAILURE);
    }

    printf("Child received: %s\n", message.text);
    fflush(stdout);

    message.type = 2;
    snprintf(message.text, sizeof(message.text), "Reply from child process.");

    if (msgsnd(queue_id, &message, strlen(message.text) + 1, 0) == -1) {
      perror("child msgsnd");
      _exit(EXIT_FAILURE);
    }

    _exit(EXIT_SUCCESS);
  }

  message.type = 1;
  snprintf(message.text, sizeof(message.text), "Request from parent process.");

  if (msgsnd(queue_id, &message, strlen(message.text) + 1, 0) == -1) {
    perror("parent msgsnd");
  } else {
    printf("Parent sent: %s\n", message.text);
  }

  if (msgrcv(queue_id, &message, sizeof(message.text), 2, 0) == -1) {
    perror("parent msgrcv");
  } else {
    printf("Parent received: %s\n", message.text);
  }

  wait_for_child(pid);

  if (msgctl(queue_id, IPC_RMID, NULL) == -1) {
    perror("msgctl");
  }
}

/* ---------- 9. Shared data communication ---------- */

static void shared_memory_communication(void) {
  int shmid;
  int sync_pipe[2];
  pid_t pid;
  char *shared;
  char signal_byte = 'R';

  printf("\n--- Shared Memory Communication ---\n");

  shmid = shmget(IPC_PRIVATE, TEXT_SIZE, IPC_CREAT | 0600);
  if (shmid == -1) {
    perror("shmget");
    return;
  }

  shared = shmat(shmid, NULL, 0);
  if (shared == (void *)-1) {
    perror("shmat");
    shmctl(shmid, IPC_RMID, NULL);
    return;
  }

  if (pipe(sync_pipe) == -1) {
    perror("pipe");
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);
    return;
  }

  pid = fork();

  if (pid < 0) {
    perror("fork");
    close(sync_pipe[0]);
    close(sync_pipe[1]);
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);
    return;
  }

  if (pid == 0) {
    close(sync_pipe[1]);

    if (read_exact(sync_pipe[0], &signal_byte, 1) == -1) {
      perror("child synchronization");
      shmdt(shared);
      close(sync_pipe[0]);
      _exit(EXIT_FAILURE);
    }

    printf("Child read from shared memory: %s\n", shared);
    fflush(stdout);

    close(sync_pipe[0]);
    shmdt(shared);
    _exit(EXIT_SUCCESS);
  }

  close(sync_pipe[0]);

  snprintf(shared, TEXT_SIZE, "Data written by parent into shared memory.");

  printf("Parent wrote: %s\n", shared);

  if (write_all(sync_pipe[1], &signal_byte, 1) == -1) {
    perror("parent synchronization");
  }

  close(sync_pipe[1]);
  wait_for_child(pid);

  shmdt(shared);

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
  }
}

/* ---------- 10. Arithmetic using shared memory ---------- */

static void shared_memory_arithmetic(void) {
  int shmid;
  int sync_pipe[2];
  pid_t pid;
  SharedArithmetic *shared;
  char signal_byte = 'C';
  int first, second;
  char operation[TEXT_SIZE];

  printf("\n--- Shared Memory Arithmetic Operations ---\n");

  if (read_integer("Enter first integer: ", &first) != 1 ||
      read_integer("Enter second integer: ", &second) != 1 ||
      !read_line("Operation (+, -, *, /): ", operation, sizeof(operation))) {
    return;
  }

  if (strlen(operation) != 1 || strchr("+-*/", operation[0]) == NULL) {
    printf("Invalid operation.\n");
    return;
  }

  if (operation[0] == '/' && second == 0) {
    printf("Division by zero is not allowed.\n");
    return;
  }

  shmid = shmget(IPC_PRIVATE, sizeof(*shared), IPC_CREAT | 0600);

  if (shmid == -1) {
    perror("shmget");
    return;
  }

  shared = shmat(shmid, NULL, 0);

  if (shared == (void *)-1) {
    perror("shmat");
    shmctl(shmid, IPC_RMID, NULL);
    return;
  }

  if (pipe(sync_pipe) == -1) {
    perror("pipe");
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);
    return;
  }

  shared->first = first;
  shared->second = second;
  shared->operation = operation[0];
  shared->ready = 0;
  shared->success = 0;

  pid = fork();

  if (pid < 0) {
    perror("fork");
    close(sync_pipe[0]);
    close(sync_pipe[1]);
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);
    return;
  }

  if (pid == 0) {
    close(sync_pipe[1]);

    if (read_exact(sync_pipe[0], &signal_byte, 1) == -1) {
      perror("server synchronization");
      shmdt(shared);
      close(sync_pipe[0]);
      _exit(EXIT_FAILURE);
    }

    switch (shared->operation) {
    case '+':
      shared->result = shared->first + shared->second;
      shared->success = 1;
      break;
    case '-':
      shared->result = shared->first - shared->second;
      shared->success = 1;
      break;
    case '*':
      shared->result = shared->first * shared->second;
      shared->success = 1;
      break;
    case '/':
      shared->result = shared->first / shared->second;
      shared->success = 1;
      break;
    }

    shared->ready = 1;
    printf("Server completed the arithmetic operation.\n");
    fflush(stdout);

    close(sync_pipe[0]);
    shmdt(shared);
    _exit(EXIT_SUCCESS);
  }

  close(sync_pipe[0]);

  printf("Client placed operands in shared memory.\n");

  if (write_all(sync_pipe[1], &signal_byte, 1) == -1) {
    perror("client synchronization");
  }

  close(sync_pipe[1]);
  wait_for_child(pid);

  if (shared->ready && shared->success) {
    printf("Result: %d %c %d = %d\n", shared->first, shared->operation,
           shared->second, shared->result);
  } else {
    printf("The server could not calculate the result.\n");
  }

  shmdt(shared);

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
  }
}

/* ---------- 11. Shared-memory file copy ---------- */

static void shared_memory_file_copy(void) {
  int shmid;
  int data_ready[2];
  int buffer_consumed[2];
  pid_t pid;
  SharedFileBuffer *shared;
  char source[TEXT_SIZE];
  char destination[TEXT_SIZE];
  char signal_byte;
  int input_fd, output_fd;
  ssize_t count;
  int failed = 0;

  printf("\n--- Shared Memory File Copy ---\n");

  if (read_path("Source file path: ", source, sizeof(source)) != 1 ||
      read_path("Destination file path: ", destination, sizeof(destination)) !=
          1) {
    return;
  }

  if (strcmp(source, destination) == 0) {
    printf("Source and destination must be different.\n");
    return;
  }

  input_fd = open(source, O_RDONLY);
  if (input_fd < 0) {
    perror("open source");
    return;
  }

  shmid = shmget(IPC_PRIVATE, sizeof(*shared), IPC_CREAT | 0600);

  if (shmid == -1) {
    perror("shmget");
    close(input_fd);
    return;
  }

  shared = shmat(shmid, NULL, 0);

  if (shared == (void *)-1) {
    perror("shmat");
    close(input_fd);
    shmctl(shmid, IPC_RMID, NULL);
    return;
  }

  if (pipe(data_ready) == -1 || pipe(buffer_consumed) == -1) {
    perror("pipe");
    close(input_fd);
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);
    return;
  }

  pid = fork();

  if (pid < 0) {
    perror("fork");
    close(input_fd);
    close(data_ready[0]);
    close(data_ready[1]);
    close(buffer_consumed[0]);
    close(buffer_consumed[1]);
    shmdt(shared);
    shmctl(shmid, IPC_RMID, NULL);
    return;
  }

  if (pid == 0) {
    close(data_ready[1]);
    close(buffer_consumed[0]);
    close(input_fd);

    output_fd = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    if (output_fd < 0) {
      perror("open destination");
      close(data_ready[0]);
      close(buffer_consumed[1]);
      shmdt(shared);
      _exit(EXIT_FAILURE);
    }

    for (;;) {
      if (read_exact(data_ready[0], &signal_byte, 1) == -1) {
        perror("child data signal");
        failed = 1;
        break;
      }

      if (shared->size == 0) {
        break;
      }

      if (write_all(output_fd, shared->data, shared->size) == -1) {
        perror("write destination");
        failed = 1;
        break;
      }

      signal_byte = 'A';

      if (write_all(buffer_consumed[1], &signal_byte, 1) == -1) {
        perror("child acknowledgement");
        failed = 1;
        break;
      }
    }

    close(output_fd);
    close(data_ready[0]);
    close(buffer_consumed[1]);
    shmdt(shared);
    _exit(failed ? EXIT_FAILURE : EXIT_SUCCESS);
  }

  close(data_ready[0]);
  close(buffer_consumed[1]);

  while ((count = read(input_fd, shared->data, DATA_SIZE)) > 0) {
    shared->size = (size_t)count;
    signal_byte = 'D';

    if (write_all(data_ready[1], &signal_byte, 1) == -1 ||
        read_exact(buffer_consumed[0], &signal_byte, 1) == -1) {
      perror("shared-memory synchronization");
      failed = 1;
      break;
    }
  }

  if (count < 0) {
    perror("read source");
    failed = 1;
  }

  if (!failed) {
    shared->size = 0;
    signal_byte = 'D';

    if (write_all(data_ready[1], &signal_byte, 1) == -1) {
      perror("send end signal");
      failed = 1;
    }
  }

  close(input_fd);
  close(data_ready[1]);
  close(buffer_consumed[0]);
  wait_for_child(pid);

  shmdt(shared);

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
  }

  if (!failed) {
    printf("Shared-memory file copy completed.\n");
    printf("Destination: %s\n", destination);
  }
}

/* ---------- Main menu ---------- */

static void display_menu(void) {
  printf("\n");
  printf("====================================================\n");
  printf("       INTER-PROCESS COMMUNICATION (IPC)\n");
  printf("====================================================\n");
  printf(" 1. Anonymous Pipe Communication\n");
  printf(" 2. Bidirectional Communication Using Two Pipes\n");
  printf(" 3. File Transfer Using Pipes\n");
  printf(" 4. Named Pipes (FIFO)\n");
  printf(" 5. Client-Server Communication Using FIFO\n");
  printf(" 6. FIFO-Based Chat Application\n");
  printf(" 7. Producer-Consumer Using Message Queues\n");
  printf(" 8. System V Message Queue Communication\n");
  printf(" 9. Shared Data Communication\n");
  printf("10. Arithmetic Using Shared Memory\n");
  printf("11. Shared-Memory File Copy\n");
  printf(" 0. Exit\n");
  printf("====================================================\n");
}

int main(void) {
  int choice;
  int result;

  for (;;) {
    display_menu();

    result = read_integer("Enter your choice: ", &choice);

    if (result == 0) {
      printf("\nInput closed. Exiting.\n");
      break;
    }

    if (result < 0) {
      continue;
    }

    switch (choice) {
    case 1:
      anonymous_pipe();
      break;
    case 2:
      bidirectional_pipes();
      break;
    case 3:
      pipe_file_transfer();
      break;
    case 4:
      named_fifo();
      break;
    case 5:
      fifo_client_server();
      break;
    case 6:
      fifo_chat();
      break;
    case 7:
      producer_consumer_queue();
      break;
    case 8:
      message_queue_communication();
      break;
    case 9:
      shared_memory_communication();
      break;
    case 10:
      shared_memory_arithmetic();
      break;
    case 11:
      shared_memory_file_copy();
      break;
    case 0:
      printf("Exiting Assignment 5.\n");
      return EXIT_SUCCESS;
    default:
      printf("Invalid choice. Select a number from 0 to 11.\n");
    }
  }

  return EXIT_SUCCESS;
}
