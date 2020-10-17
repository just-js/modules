extern int log(const char* pattern, int n);
int foo (int n) {
  log("%i.", n);
  return 99;
}
