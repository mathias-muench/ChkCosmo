#ifndef CU_H
#define CU_H
#include <stdio.h>

static int _cu_tests_run = 0;
static int _cu_tests_pass = 0;

#define cu_test(test) do { \
  int _success = test(); \
  _cu_tests_run++; \
  _success && _cu_tests_pass++; \
  const char *_result_string = _success ? "OK" : "FAIL"; \
  printf("TEST: %s: %s\n", #test, _result_string); \
} while(0)

#define cu_result() (_cu_tests_run == _cu_tests_pass)

#endif
