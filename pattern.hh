#ifndef tdlmake_ds_pattern_hh
#define tdlmake_ds_pattern_hh

#include <iostream>
#include <vector>
#include <set>
#include <string>
#include <list>

typedef std::vector <std::string> string_vec;
typedef std::set <int> state_set;

struct pattern_state
{
  /**
   * initialize the path pattern with a string.
   * path components are delimited by slashes.
   * a double slash (//) denotes an arbitrary sequence of path components.
   * asterisk (*) within a component acts as a wildcard.
   * leading empty components or double-slashes are ignored.
   */
  pattern_state (const std::string &pattern);
  /**
   * match a single path component. Asterisks (*) act as wildcards.
   */
  bool match1 (int idx, const std::string &subj) const;
  /**
   * initialize the state.
   * adds the initial position and completes.
   */
  void init_state (state_set &state) const;
  /**
   * shift a path component.
   * return true, iff the beginning of the pattern was shifted.
   */
  bool shift (int pos, const std::string &subj, state_set &state) const;
  /**
   * path components of the pattern.
   */
  const string_vec m_comp;
};

/**
 * a concrete filename converted into a sequence of components.
 * all components are converted to lowercase.
 */
struct lc_path
{
  /**
   * construct a pattern from a filename.
   */
  lc_path (const std::string &filename);
  /**
   * match this against a pattern.
   */
  bool match (const std::string &pattern) const;
  /**
   * lowercase the string.
   */
  std::string lowercase (const std::string &s) const;
  /**
   * locale for filename lowercase conversion.
   */
  const std::locale m_loc;
  /**
   * list of components (things between /) in reverse order.
   */
  const string_vec m_comp;
};

#endif
