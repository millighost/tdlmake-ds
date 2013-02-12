#include "sysdep.hh"

#if defined (WIN32)
#  include <windows.h>
#else
#  include <unistd.h>
#endif

namespace {

static const char log_filename[] = "log-tdlmake.txt";

/**
 * retrieve the user app data folder.
 */
static std::string get_path_data ()
{
  std::string folder_name;
#if defined (WIN32)
  HKEY hkey;
  if (RegOpenKeyEx (HKEY_CURRENT_USER,
      "Software\\Microsoft\\Windows\\CurrentVersion"
      "\\Explorer\\Shell Folders\\", 0, KEY_READ, &hkey) != 0) {
    return std::string ();
  }
  {
    char buffer[1000];
    DWORD buffer_size = sizeof buffer;
    DWORD type;
    if (RegQueryValueEx (hkey, "AppData", 0, &type,
        (BYTE *) &buffer[0], &buffer_size) == 0)
      {
        folder_name.assign (buffer, buffer_size);
        folder_name += '\\';
      }
  }
  RegCloseKey (hkey);
#else
  folder_name.assign ("/");
#endif
  return folder_name;
}

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

/**
 * get the configuration filename.
 */
static std::string get_datafile (const std::string &datadir)
{
  return datadir + "gamma.txt";
}

} /* local namespace */

#include <iostream>

sysdep_conf::sysdep_conf (const std::string &argv0)
  : path_self (get_path_self (argv0)),
    path_data (get_path_data ()),
    path_tdlmake (get_path_tdlmake (path_self)),
    path_datafile (get_datafile (path_data)),
    path_logfile (path_data + log_filename)
{
}
