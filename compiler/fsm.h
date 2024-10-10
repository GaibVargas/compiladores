#ifndef __fsm_h
#define __fsm_h

#include <unordered_map>
#include <string>
#include <functional>
#include <vector>
#include <set>

enum FSMStatus
{
  IDLE,
  RUNNING,
  SUCCESS,
  ERROR
};

class FiniteStateMachine
{
private:
  std::unordered_map<std::string, std::function<std::string(char)>> _transitions;
  std::string _initial_state;
  std::set<std::string> _final_states;
  std::set<std::string> _cursor_back_final_states;
  std::string _current_state;
  std::string _lexeme;
  FSMStatus _status;

public:
  FiniteStateMachine(
      std::unordered_map<std::string, std::function<std::string(char)>> transitions,
      std::string initial_state,
      std::set<std::string> final_states,
      std::set<std::string> cursor_back_final_states = {}) : _transitions(transitions), _initial_state(initial_state), _final_states(final_states), _cursor_back_final_states(cursor_back_final_states), _current_state(initial_state) {}

  FSMStatus get_status()
  {
    return _status;
  }

  std::string get_lexeme()
  {
    return _lexeme;
  }

  void reset();
  FSMStatus transition(char c);
  bool has_to_move_cursor_back();
};

#endif