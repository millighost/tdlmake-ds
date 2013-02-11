#include "sysdep.hh"

#if defined (WIN32)
#  include <windows.h>
#else
#  include <unistd.h>
#endif

namespace {

#if defined (WIN32)
static const char log_filename[] = "v:/tmp/log.txt";
#else
static const char log_filename[] = "log.txt";
#endif

/**
 * get the path directory of the executable.
 */
static std::string get_path_self (const std::string &argv0)
{
  std::string::size_type spos = argv0.rfind ('/');
  if (spos != argv0.npos) {
    return argv0.substr (0, spos + 1);
  }
#ifdef WIN32
  /*
   * windows try the backslash as a delimiter.
   */
  spos = argv0.rfind ('\\');
  if (spos != argv0.npos) {
    return argv0.substr (0, spos + 1);
  }
  /*
   * get the name of the executable.
   */
  {
    char buffer[1000];
    int size = GetModuleFileName (0, buffer, sizeof buffer);
    if (size > 0) {
      std::string exec_name (buffer);
      return exec_name.substr (0, exec_name.find ('\\') + 1);
    }
  }
#else
  /*
   * not on windows.
   */
  {
    char buffer[1000];
    ssize_t link_size = readlink ("/proc/self/exe", buffer, sizeof buffer);
    std::string exec_name (buffer, link_size);
    return exec_name.substr (0, exec_name.find ('/') + 1);
  }
#endif
  return ".";
}

/**
 * search for a replacement tdlmake.
 * self should be the path of the program itself.
 */
std::string get_path_tdlmake (const std::string &self)
{
  return self + "tdlmake-org.exe";
}

} /* local namespace */

#include <iostream>
sysdep_conf::sysdep_conf (const std::string &argv0)
  : path_self (get_path_self (argv0)),
    path_tdlmake (get_path_tdlmake (path_self)),
    path_datafile (path_self + "gamma.txt"),
    path_logfile (log_filename)
{
}
