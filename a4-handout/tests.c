#include "comparators.h"
#include "log.h"
#include "printers.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RED "\033[31m"
#define GREEN "\033[32m"
#define RESET "\033[0m"
#define BOLD "\033[1m"

#define CHECKMARK "\u2714"
#define CROSSMARK "\u2718"

#define MAX_COMMAND_LENGTH 256

void print_header() {
  printf("\n%s%-3s %-25s | %-14s %-14s %s%s\n", BOLD, "No", "Description",
         "Expected", "Got", "Result", RESET);
}

typedef int (*Comparator)(void* expected, void* got);
typedef void (*Printer)(void* value, char* buffer, size_t buffer_size);

typedef struct {
  int passed;
  int failed;
} TestResult;

int assert_test(uint8_t test_case, const char* description, void* expected,
                void* got, Comparator compare, Printer print_expected,
                Printer print_got, TestResult* result) {
  const char* result_icon;
  const char* result_color;
  int         succeeded = compare(expected, got);

  if (succeeded) {
    result_icon  = CHECKMARK;
    result_color = GREEN;
    result->passed++;
  } else {
    result_icon  = CROSSMARK;
    result_color = RED;
    result->failed++;
  }

  char expected_str[32];
  char got_str[32];

  print_expected(expected, expected_str, sizeof(expected_str));
  print_got(got, got_str, sizeof(got_str));

  char parenthesized_description[40];
  snprintf(parenthesized_description, sizeof(parenthesized_description), "(%s)",
           description);

  printf("%-3u %-25s | %-14s %-14s %s%s%s\n", test_case,
         parenthesized_description, expected_str, got_str, result_color,
         result_icon, RESET);
  return !succeeded;
}

int test_id_query_naive(int* counter, TestResult* result) {
  // Run ./id_query_naive data/10_samples.tsv
  // and compare the output with the expected output
  const char* input_file      = "data/10_samples.tsv";
  const char* expected_output = "expected_outputs/10_samples.txt";
  char        command[MAX_COMMAND_LENGTH];
  char        output_line[1024];
  char        expected_line[1024];

  int r = snprintf(command, sizeof(command), "./id_query_naive %s", input_file);
  if (r < 0) {
    LOG_ERROR("Failed to create command string");
    return 1;
  }

  FILE* fp_output = popen(command, "r");
  if (!fp_output) {
    LOG_ERROR("Failed to open pipe to command: %s", command);
    pclose(fp_output);
    result->failed++;
    return 1;
  }

  FILE* fp_expected = fopen(expected_output, "r");
  if (!fp_expected) {
    LOG_ERROR("Failed to open file: %s", expected_output);
    pclose(fp_output);
    result->failed++;
    return 1;
  }

  int failed = 0;

  while (fgets(output_line, sizeof(output_line), fp_output) &&
         fgets(expected_line, sizeof(expected_line), fp_expected)) {
    failed |= assert_test((*counter)++, "Output line", expected_line,
                          output_line, (Comparator)strcmp, (Printer)snprintf,
                          (Printer)snprintf, result);
  }

  if (failed) {
    result->failed++;
    return 1;
  }
  result->passed++;
  pclose(fp_output);
  fclose(fp_expected);

  return 0;
}

int main() {
  struct {
    int (*func_ptr)(int* c, TestResult* r);
    const char* name;
  } test_cases[] = {{&test_id_query_naive, "test_id_query_naive"}};

  int        test_counter = 1;
  TestResult result       = {0, 0};

  for (int i = 0; i < (int)(sizeof(test_cases) / sizeof(test_cases[0])); i++) {
    print_header();
    printf("%s:\n", test_cases[i].name);

    int   res    = test_cases[i].func_ptr(&test_counter, &result);
    char* color  = res == 0 ? GREEN : RED;
    char* result = res == 0 ? "PASSED" : "FAILED";

    printf("%s %s%s%s:\n\n", test_cases[i].name, color, result, RESET);

    char msg_buffer[100];
    snprintf(msg_buffer, sizeof(msg_buffer), "Test failed for %s",
             test_cases[i].name);
    if (res != 0) {
      LOG_ERROR("%s", msg_buffer);
    }
  }
  printf("Summary: ");
  printf("%s%d tests passed%s, ", GREEN, result.passed, RESET);
  printf("%s%d tests failed%s\n\n", RED, result.failed, RESET);
}