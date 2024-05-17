#include <iostream>
#include <csignal>

sig_atomic_t sigflag = 0;

void sighandler(int s)
{
  std::cerr << "Caught signal " << s << ".\n"; // this is undefined behaviour
  sigflag = 1; // something like that
}

int main()
{
  std::signal(SIGINT, sighandler);

  // ... your program here ...

  // example: baby's first loop (Ctrl-D to end)
  char c;
  while (std::cin >> c)
  {
    if (sigflag != 0) { std::cerr << "Signal!\n"; sigflag = 0; }
  }
}
