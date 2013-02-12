/*
 * replacement program for tdlmake.
 */
#include "pattern.hh"
#include "sysdep.hh"
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <list>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <sstream>

/**
 * data type for option lists.
 */
typedef std::list <std::string> string_list;

/**
 * function to determine the gamma for an image.
 */
static float get_gamma_for
  (const sysdep_conf &conf, const std::string &filename)
{
  lc_path path (filename);
  std::ifstream ifs (conf.path_datafile.c_str (), std::ios::in);
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
/**
 * return the number of arguments for that specific option.
 */
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

/**
 * convert the argc/argv list into a string list.
 */
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

/**
 * get the parameters (filename arguments), ie parameters without options.
 */
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

/**
 * insert the gamma option in front of the string list.
 */
static void insert_gamma_option (float gamma, string_list &args)
{
  {
    std::ostringstream oss;
    oss << gamma;
    args.push_front (oss.str ());
  }
  args.push_front ("-gamma");
}

/**
 * convert the list of strings into a shell command line for
 * use with the system() function.
 */
static std::string build_command_line
  (const std::string &command, const string_list &args)
{
  std::ostringstream oss;
  oss << command;
  string_list::const_iterator it = args.begin ();
  while (it != args.end ()) {
    oss << " " << '"' << *it << '"';
    ++ it;
  }
  return oss.str ();
}

/**
 * execute the command line.
 */
static int execute_tdlmake (const std::string &cmdline)
{
  int ec = system (cmdline.c_str ());
  return ec;
}

int main (int argc, char **argv)
{
  sysdep_conf conf (argv[0]);
#ifdef TEST
  std::cout << "    self: " << conf.path_self << '\n'
            << "    data: " << conf.path_data << '\n'
            << " tdlmake: " << conf.path_tdlmake << '\n'
            << "datafile: " << conf.path_datafile << '\n'
            << " logfile: " << conf.path_logfile << '\n';
#endif
  std::ofstream log (conf.path_logfile.c_str (), std::ios::out | std::ios::app);
  string_list args (get_all_args (argc, argv));
  if (!has_gamma_option (args)) {
    const string_list file_params (get_parameters (args));
    float gamma;
    if (file_params.size () > 1) {
      gamma = get_gamma_for (conf, file_params.front ());
    } else {
      gamma = 1;
    }
    if (gamma != 1) {
      insert_gamma_option (gamma, args);
    }
  }
  const std::string cmdline = build_command_line (conf.path_tdlmake, args);
#if defined (TEST)
  std::cout << "EXEC: " << cmdline << '\n';
  return 0;
#else
  log << "executing: " << cmdline << '\n';
  int ec = execute_tdlmake (cmdline);
  return ec;
#endif
}
