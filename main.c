#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_LENGTH 200
#define FILE_NAME "todo.txt"
#define TEMP_FILE_NAME "temp.txt"

// Task structure
typedef struct {
  char description[MAX_LENGTH];
  unsigned int priority;
} Task;

// Menu function
void display_menu() {
  printf("\nTodo List Options:\n");
  printf("1. Add a task\n");
  printf("2. Remove a task\n");
  printf("3. View tasks\n");
  printf("4. Exit\n");
  printf("Choose an option (1-4): ");
}

// Create task and allocate memory
Task *input_task() {
  Task *task = (Task *)malloc(sizeof(Task));
  if (task == NULL) {
    perror("Error allocating memory.\n");
    return NULL;
  }

  // Prompt for input
  printf("Enter a task: ");
  fgets(task->description, MAX_LENGTH, stdin);
  task->description[strcspn(task->description, "\n")] =
      0; // Remove the trailing '\n' that fgets adds

  printf("Enter task priority (1-5): ");
  scanf("%u", &task->priority);
  getchar(); // To remove the trailing '\n' after scanf

  if (task->priority < 1 || task->priority > 5) {
    printf("Invalid priority. Setting to default (5).\n");
    task->priority = 5;
    free(task);
  }

  return task;
}

// Function to add task, argument *fileName for *task
void add_task(const char *fileName, Task *task) {
  FILE *file = fopen(fileName, "a");
  if (file == NULL) {
    perror("Error opening file for appending.\n");
    return;
  }

  fprintf(file, "%s|%u\n", task->description, task->priority);
  fclose(file);
  printf("Task added.\n");
}

// Take the number and priority
void input_task_number_and_priority(unsigned int *task_number,
                                    unsigned int *task_priority) {
  printf("Enter the task number to remove: ");
  scanf("%u", task_number);
  printf("Enter the task priority to remove: ");
  scanf("%u", task_priority);
}

// Function to delete task from *fileName, using task_number
void delete_task(const char *fileName, unsigned int task_number,
                 unsigned int task_priority) {
  FILE *originalFile = fopen(fileName, "r");
  FILE *tempFile = fopen(TEMP_FILE_NAME, "w");
  if (originalFile == NULL || tempFile == NULL) {
    perror("Error opening files.\n");
    return;
  }

  char buffer[MAX_LENGTH];
  unsigned int line_count = 0;

  while (fgets(buffer, sizeof(buffer), originalFile) != NULL) {
    line_count++;
    Task task;
    sscanf(buffer, "%[^|]|%u", task.description, &task.priority);
    if (line_count != task_number || task.priority != task_priority) {
      fputs(buffer, tempFile);
    }
  }

  fclose(originalFile);
  fclose(tempFile);

  remove(fileName);
  rename(TEMP_FILE_NAME, fileName);

  printf("Task removed.\n");
}

// Function to list tasks from todo.txt
void list_tasks(const char *fileName) {
  FILE *file = fopen(fileName, "r");
  if (file == NULL) {
    perror("Error opening file for reading.\n");
    return;
  }

  char buffer[MAX_LENGTH];
  unsigned int task_count = 0;

  printf("\nTodo List:\n");
  while (fgets(buffer, sizeof(buffer), file) != NULL) {
    task_count++;
    Task task;
    sscanf(buffer, "%[^|]|%u", task.description, &task.priority);
    printf("%u. %s (Priority: %u)\n", task_count, task.description,
           task.priority);
  }

  fclose(file);

  if (task_count == 0) {
    printf("No tasks available.\n");
  }
}

int main(void) {
  unsigned int option;

  FILE *file = fopen(FILE_NAME, "a+");
  if (file == NULL) {
    perror("Error creating or opening file.\n");
    return 1;
  }
  fclose(file);
  printf("File ready.\n");

  do {
    display_menu();
    unsigned int result = scanf("%u", &option);
    getchar(); // To remove the trailing '\n' after scanf
    system("clear");

    // Check if keyboard input can be taken
    if (result == EOF) {
      perror("Error reading input or EOF detected.\n");
      break; // break the loop if EOF or an error has occurred
    } else if (result != 1) {
      perror("Invalid input. Please enter a number.\n");
      continue;
    }

    switch (option) {
    case 1: {
      Task *task = input_task();
      if (task != NULL) {
        add_task(FILE_NAME, task);
        free(task);
      }
      break;
    }

    case 2: {
      list_tasks(FILE_NAME);
      unsigned int task_number, task_priority;
      input_task_number_and_priority(&task_number, &task_priority);
      delete_task(FILE_NAME, task_number, task_priority);
      break;
    }

    case 3: {
      list_tasks(FILE_NAME);
      break;
    }

    case 4:
      printf("Exiting.\n");
      break;

    default:
      perror("Invalid option. Please choose a valid option (1-4).\n");
    }
  } while (option != 4);
}
