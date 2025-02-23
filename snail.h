#ifndef SNAIL_H
#define SNAIL_H

#include <stdbool.h>

#define SNAIL_ASSERT(assertion) do { Snail_assert(assertion, #assertion, __FILE__, __LINE__); } while(0)

void Snail_assert(bool result, const char *assertion, const char *file, int line);
void Snail_error(const char *file, int line, const char *format, ...);

void Snail_executeTest(void (*test)());
void Snail_skipTest(void (*test)());

void Snail_pushSetUpHook(void (*hook)(void));
void Snail_popSetUpHook();
void Snail_pushTearDownHook(void (*hook)(void));
void Snail_popTearDownHook();

#ifdef SNAIL_MAIN
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

static void (*Snail_setUpHook)(void) = NULL;

void Snail_pushSetUpHook(void (*hook)(void)) {
  Snail_setUpHook = hook;
}

void Snail_popSetUpHook() {
  Snail_setUpHook = NULL;
}

void Snail_runSetUpHook() {
  if (Snail_setUpHook)
    Snail_setUpHook();
}

static void (*Snail_tearDownHook)(void) = NULL;

void Snail_pushTearDownHook(void (*hook)(void)) {
  Snail_tearDownHook = hook;
}

void Snail_popTearDownHook() {
  Snail_tearDownHook = NULL;
}

void Snail_runTearDownHook() {
  if (Snail_tearDownHook)
    Snail_tearDownHook();
}

static int Snail_assertionsCount = 0;
static int Snail_currentFailedAssertionsCount = 0;
static int Snail_failedAssertionsCount = 0;
static int Snail_failedTestsCount = 0;
static int Snail_passedAssertionsCount = 0;
static int Snail_passedTestsCount = 0;
static int Snail_skippedTestsCount = 0;
static int Snail_testsCount = 0;

void Snail_errorArgs(const char *file, int line, const char *format, va_list args) {
  fprintf(stderr, "%s:%d: error: ", file, line);
  vfprintf(stderr, format, args);
}

void Snail_error(const char *file, int line, const char *format, ...) {
  va_list args;
  va_start(args, format);
  Snail_errorArgs(file, line, format, args);
  va_end(args);
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
  printf("%d tests: %d passed, %d failed, %d skipped.\n",
	 Snail_testsCount, Snail_passedTestsCount, Snail_failedTestsCount, Snail_skippedTestsCount);
}

void Snail_executeTest(void (*test)()) {
  Snail_currentFailedAssertionsCount = 0;
  Snail_runSetUpHook();
  test();
  Snail_runTearDownHook();
  if (Snail_currentFailedAssertionsCount == 0)
    Snail_passedTestsCount++;
  else
    Snail_failedTestsCount++;
  Snail_testsCount++;
}

void Snail_skipTest(void (*test)()) {
  Snail_skippedTestsCount++;
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
