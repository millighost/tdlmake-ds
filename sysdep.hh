#ifndef tdlmake_ds_sysdep_hh
#define tdlmake_ds_sysdep_hh

#include <string>

/**
 * contains the various parameters gotten from the os.
 */
struct sysdep_conf
{
  /**
   * initialize with the name of the executable.
   */
  sysdep_conf (const std::string &argv0);
  /**
   * path to the installation.
   */
  std::string path_self;
  /**
   * path to the data directory.
   */
  std::string path_data;
  /**
   * path to original tdlmake command.
   */
  std::string path_tdlmake;
  /**
   * path to the datafile.
   */
  std::string path_datafile;
  /**
   * get the path to the logfile.
   */
  std::string path_logfile;
};

#endif
