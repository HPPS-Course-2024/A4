#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <string.h>
#include <time.h>

#define YELLOW "\033[38;2;255;255;85m"
#define RED "\033[31m"
#define RESET "\033[0m"
#define GRAY "\033[38;2;140;140;140m"
#define BLUE "\033[34m"
#define LIGHTGRAY "\033[38;2;211;211;211m"
#define LIGHTBLUE "\033[38;2;173;216;230m"

#define DEBUG_FLAG 0

#if DEBUG_FLAG == 0
#define LOG(level, fmt, ...)                                                   \
  do {                                                                         \
    time_t     rawtime;                                                        \
    struct tm* timeinfo;                                                       \
    char       timestr[20];                                                    \
    FILE*      log_file = fopen("logger.log", "a");                            \
    if (log_file) {                                                            \
      time(&rawtime);                                                          \
      timeinfo = localtime(&rawtime);                                          \
      strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", timeinfo);       \
      fprintf(log_file, "[%s] [%s:%d in %s] %s: " fmt "\n", timestr, __FILE__, \
              __LINE__, __func__, level, ##__VA_ARGS__);                       \
      if (strcmp(level, "ERROR") == 0) {                                       \
        fprintf(stderr,                                                        \
                "[%s%s%s] [%s%s%s:%s%d%s in %s%s%s] %s%s%s: " fmt "\n", GRAY,  \
                timestr, RESET, BLUE, __FILE__, RESET, BLUE, __LINE__, RESET,  \
                RED, __func__, RESET, RED, level, RESET, ##__VA_ARGS__);       \
      }                                                                        \
      fclose(log_file);                                                        \
    }                                                                          \
  } while (0)
#elif DEBUG_FLAG == -1
#define LOG(level, fmt, ...)                                                   \
  do {                                                                         \
  } while (0)
#endif

#define LOG_INFO(fmt, ...) LOG("INFO", fmt, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) LOG("ERROR", fmt, ##__VA_ARGS__)

#endif