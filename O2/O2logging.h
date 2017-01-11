#include <stdio.h>
#ifndef _O2_LOG_H
#define _O2_LOG_H

typedef enum { PASS, INFO, WARN, FAIL } report_level_t;

extern FILE *DEFAULT_REPORT_STREAM;
extern int REPORT_W_TIMESTAMPS;
extern int REPORT_W_COLORS;

void report(report_level_t level, const char *format, ...)
    __attribute__((format(printf, 2, 3)));
void reportf(report_level_t level, int timestamped, int colored,
             const char *format, ...) __attribute__((format(printf, 4, 5)));
void freport(FILE *stream, report_level_t level, const char *format, ...)
    __attribute__((format(printf, 3, 4)));
void freportf(FILE *stream, report_level_t level, int timestamped, int colored,
              const char *format, ...) __attribute__((format(printf, 5, 6)));

#endif
