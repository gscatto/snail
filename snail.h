#ifndef SNAIL_H
#define SNAIL_H

#include <stdbool.h>

#define SNAIL_ASSERT(assertion) do { Snail_assert(assertion, #assertion, __FILE__, __LINE__); } while(0)

void Snail_assert(bool result, const char *assertion, const char *file, int line);

void Snail_executeTest(void (*test)());

#ifdef SNAIL_MAIN
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

static int Snail_assertionsCount = 0;
static int Snail_currentFailedAssertionsCount = 0;
static int Snail_failedAssertionsCount = 0;
static int Snail_failedTestsCount = 0;
static int Snail_passedAssertionsCount = 0;
static int Snail_passedTestsCount = 0;
static int Snail_testsCount = 0;

void Snail_errorArgs(const char *file, int line, const char *format, va_list args) {
  fprintf(stderr, "%s:%d: error: ", file, line);
  vfprintf(stderr, format, args);
}

void Snail_addPassedAssertion() {
  Snail_assertionsCount++;
  Snail_passedAssertionsCount++;
}

void Snail_addFailedAssertion(const char* file, int line, const char *format, ...) {
  va_list args;
  Snail_assertionsCount++;
  Snail_failedAssertionsCount++;
  Snail_currentFailedAssertionsCount++;
  va_start(args, format);
  Snail_errorArgs(file, line, format, args);
  va_end(args);
}

void Snail_assert(bool result, const char *assertion, const char *file, int line) {
  if (result)
    Snail_addPassedAssertion();
  else
    Snail_addFailedAssertion(file, line, "assertion \"%s\" failed.\n", assertion);
}

static bool Snail_didTestSuitePass() {
  return Snail_failedAssertionsCount == 0;
}

static void Snail_printTestSuiteResults() {
  printf("%d assertions run: %d passed, %d failed.\n",
	 Snail_assertionsCount, Snail_passedAssertionsCount, Snail_failedAssertionsCount);
  printf("%d tests run: %d passed, %d failed.\n",
	 Snail_testsCount, Snail_passedTestsCount, Snail_failedTestsCount);
}

void Snail_executeTest(void (*test)()) {
  Snail_currentFailedAssertionsCount = 0;
  test();
  if (Snail_currentFailedAssertionsCount == 0)
    Snail_passedTestsCount++;
  else
    Snail_failedTestsCount++;
  Snail_testsCount++;
}

void Snail_executeTestSuite();

int main() {
  Snail_executeTestSuite();
  Snail_printTestSuiteResults();
  return Snail_didTestSuitePass() ? EXIT_SUCCESS : EXIT_FAILURE;
}
#endif

#endif
