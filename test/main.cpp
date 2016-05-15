//macro CATCH_CONFIG_MAIN will create a main for CATCH
//define this macro only once
#define CATCH_CONFIG_COLOUR_ANSI
#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

int main(int argc, const char** const argv) {
  Catch::Session session;
  int returnCode = session.applyCommandLine( argc, argv);
  if (returnCode != 0) // a command line error
    return returnCode;
  return session.run();
}




