#include "pattern.hh"
#include <iostream>

namespace {
static string_vec split_path (const std::string &str)
{
  string_vec components;
  std::string::size_type pos = str.find_first_not_of ("/");
  while (pos != str.npos) {
    const std::string::size_type sp = str.find ('/', pos);
    if (sp != str.npos) {
      components.push_back (str.substr (pos, sp - pos));
      pos = sp + 1;
    } else {
      break;
    }
  }
  if (pos != str.npos) {
    components.push_back (str.substr (pos));
  }
  return components;
}

static std::ostream &operator << (std::ostream &os, const state_set &st)
{
  os << "{";
  state_set::const_iterator it = st.begin ();
  if (it != st.end ()) {
    os << *it;
    ++ it;
  }
  while (it != st.end ()) {
    os << ", " << *it;
    ++ it;
  }
  return os << "}";
}
} /* local namespace */

lc_path::lc_path (const std::string &filename)
  : m_loc (std::locale ()),
    m_comp (split_path (this->lowercase (filename)))
{
}

std::string lc_path::lowercase (const std::string &s) const
{
  std::string o;
  std::string::const_iterator it = s.begin ();
  while (it != s.end ()) {
    o += std::tolower (*it, m_loc);
    ++ it;
  }
  return o;
}

bool lc_path::match (const std::string &patterns) const
{
  pattern_state pattern (this->lowercase (patterns));
  state_set state, newstate;
  pattern.init_state (state);
  int pos = m_comp.size () - 1;
  while (pos >= 0 && !state.empty ()) {
    state_set::const_iterator stit = state.begin ();
    while (stit != state.end ()) {
      if (pattern.shift (*stit, m_comp[pos], newstate)) {
        return true;
      }
      ++ stit;
    }
    state.swap (newstate);
    newstate.clear ();
    -- pos;
  }
  return false;
}

pattern_state::pattern_state (const std::string &pattern)
  : m_comp (split_path (pattern))
{
}
void pattern_state::init_state (state_set &state) const
{
  state.clear ();
  state.insert (m_comp.size () - 1);
}
/**
 * simple string comparison with wildcard.
 */
bool pattern_state::match1 (int idx, const std::string &subj) const
{
  const std::string &pattern = m_comp[idx];
  if (pattern.find ('*') != pattern.npos) {
    /*
     * complex case: pattern contains a wildcard.
     */
    state_set state;
    unsigned pos = 0;
    state.insert (0);
    if (pattern[0] == '*') {
      state.insert (1);
    }
    while (pos != subj.size () && !state.empty ()) {
      state_set newstate;
      state_set::const_iterator stit = state.begin ();
      while (stit != state.end () && (unsigned) *stit < pattern.size ()) {
        if (pattern[*stit] == '*') {
          newstate.insert (*stit);
          newstate.insert (*stit + 1);
        } else if (pattern[*stit] == subj[pos]) {
          newstate.insert (*stit + 1);
          if ((unsigned) *stit + 1 < pattern.size ()
            && pattern[*stit + 1] == '*')
            {
              newstate.insert (*stit + 2);
            }
        }
        ++ stit;
      }
      ++ pos;
      newstate.swap (state);
      newstate.clear ();
    }
    return state.find (pattern.size ()) != state.end ();
  } else {
    /*
     * simple case: no wildcard; do normal string match.
     */
    return pattern.size () == subj.size ()
      && pattern == subj;
  }
}

bool pattern_state::shift
  (int ppos, const std::string &subj, state_set &state) const
{
  if (this->match1 (ppos, subj)) {
    if (ppos == 0) {
      return true;
    } else {
      state.insert (ppos - 1);
    }
  } else if (m_comp[ppos].empty ()) {
    state.insert (ppos);
    state.insert (ppos - 1);
  }
  return false;
}

