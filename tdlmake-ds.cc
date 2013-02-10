/*
 * replacement program for tdlmake.
 */
#include "pattern.hh"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cstring>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <sstream>

#if defined (WIN32)
#  include <windows.h>
#endif

#if defined (WIN32)
static const char log_filename[] = "v:/tmp/log.txt";
static const char org_filename[] = "w:/dsprog45/bin/tdlmake-org.exe";
static const char tmp_filename[] = "v:/tmp/";
static const char dat_filename[] = "v:/tmp/gamma.txt";
#else
static const char log_filename[] = "log.txt";
static const char org_filename[] = "tdlmake";
static const char tmp_filename[] = "./";
static const char dat_filename[] = "gamma.txt";
#endif

/*
 * set path=%path%;v:/mingw/bin
 * g++ tdltest.cc -static-libstdc++ -static-libgcc
 * tdlmake called like this:
 * w:/dsprog45/bin/tdlmake-org.exe "-mode" "periodic"\
 *   "C:/users/rassahah/Application Data/DAZ 3D/Studio4/temp/d28.tif"\
 *   "C:/users/rassahah/Application Data/DAZ 3D/Studio4/temp/d29_d28.tdl"
 */

typedef std::list <std::string> string_list;

/**
 * function to determine the gamma for an image.
 */
static float get_gamma_for (const std::string &filename)
{
  std::cout << "get_gamma_for:" << filename << '\n';
  lc_path path (filename);
  std::ifstream ifs (dat_filename, std::ios::in);
  char buffer[1000];
  while (ifs.getline (buffer, sizeof buffer)) {
    std::string line (buffer, ifs.gcount () - 1);
    std::istringstream iss (line);
    float gamma;
    if (iss >> gamma >> std::ws && (size_t) iss.tellg () < line.size ()) {
      std::string path_pattern (line.substr (iss.tellg ()));
      std::string::size_type last_char
        = path_pattern.find_last_not_of ("\t\r\n ");
      if (last_char != path_pattern.npos) {
        path_pattern.resize (last_char + 1);
        std::cout << "match against '" << path_pattern << "'\n";
        if (path.match (path_pattern)) {
          return gamma;
        }
      }
    }
  }
  return 1.0;
}

/**
 * return true, iff the options already hold some gamma parameter.
 */
static bool has_gamma_option (const string_list &args)
{
  string_list::const_iterator begin = args.begin ();
  return std::find (args.begin (), args.end (), std::string ("-gamma"))
    != args.end ()
  || std::find (args.begin (), args.end (), std::string ("-rgbagamma"))
    != args.end ();
}

struct argument_count_rec {
  const char *arg;
  int count;
};
static const argument_count_rec argument_counts[] = {
  { "-skymap", 1 },
  { "-fov", 1 },
  { "-smode", 1 },
  { "-tmode", 1 },
  { "-mode", 1 },
  { "-filter", 1 },
  { "-window", 1 },
  { "-quality", 1 },
  { "-sfilterwidth", 1 },
  { "-tfilterwidth", 1 },
  { "-filterwidth", 1 },
  { "-blur", 1 },
  { "-scale", 1 },
  { "-preview", 1 },
  { "-preview8", 1 },
  { "-gamma", 1 },
  { "-rgbagamma", 4 },
  { "-colorspace", 1 },
  { "-bakeres", 1 },
  { "-pixelaspectratio", 1 },
  { "-imageaspectratio", 1 },
};
static int get_number_of_arguments (const std::string &opt)
{
  unsigned counti = 0;
  while (counti != sizeof argument_counts / sizeof argument_counts[0]) {
    if (opt.compare (argument_counts[counti].arg) == 0) {
      return argument_counts[counti].count;
    } else {
      ++ counti;
    }
  }
  return 0;
}

static string_list get_all_args (int argc, char **argv)
{
  string_list args;
  int opti = 1;
  while (opti < argc) {
    args.push_back (argv[opti]);
    ++ opti;
  }
  return args;
}

static string_list get_parameters (const string_list &args)
{
  string_list params;
  string_list::const_iterator it = args.begin ();
  while (it != args.end ()) {
    if (it->size () > 0 && (*it)[0] == '-') {
      std::advance (it, get_number_of_arguments (*it) + 1);
    } else {
      params.push_back (*it);
      ++ it;
    }
  }
  return params;
}

static void insert_gamma_option (float gamma, string_list &args)
{
  {
    std::ostringstream oss;
    oss << gamma;
    args.push_front (oss.str ());
  }
  args.push_front ("-gamma");
}

static std::string build_command_line (const string_list &args)
{
  std::ostringstream oss;
  oss << org_filename;
  string_list::const_iterator it = args.begin ();
  while (it != args.end ()) {
    oss << " " << '"' << *it << '"';
    ++ it;
  }
  return oss.str ();
}

static int execute_tdlmake (const std::string &cmdline)
{
  int ec = system (cmdline.c_str ());
  return ec;
}

int main (int argc, char **argv)
{
  /*
   * command to create and execute.
   */
  std::ofstream log (log_filename, std::ios::out | std::ios::app);
  string_list args (get_all_args (argc, argv));
  if (!has_gamma_option (args)) {
    log << "no gamma option present.\n";
    const string_list file_params (get_parameters (args));
    float gamma;
    if (file_params.size () > 1) {
      gamma = get_gamma_for (file_params.front ());
    } else {
      gamma = 1;
    }
    if (gamma != 1) {
      insert_gamma_option (gamma, args);
    }
  }
  const std::string cmdline = build_command_line (args);
  log << "command: " << cmdline << '\n';
#if defined (TEST)
  std::cout << "EXEC: " << cmdline << '\n';
  return 0;
#else
  int ec = execute_tdlmake (cmdline);
  return ec;
#endif
}
