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

  void reset()
  {
    _current_state = _initial_state;
    _lexeme = "";
    _status = FSMStatus::IDLE;
  }

  FSMStatus get_status()
  {
    return _status;
  }

  std::string get_lexeme()
  {
    return _lexeme;
  }

  void transition(char c)
  {
    if (_status == FSMStatus::ERROR)
      return;

    if (_transitions.find(_current_state) == _transitions.end())
    {
      _status = FSMStatus::ERROR;
      return;
    }
    auto next_state = _transitions.at(_current_state)(c);

    if (next_state == "dead")
    {
      _status = FSMStatus::ERROR;
      return;
    }

    _lexeme += c;
    _current_state = next_state;
    if (_final_states.find(next_state) != _final_states.end())
    {
      _status = FSMStatus::SUCCESS;
    }
    else
    {
      _status = FSMStatus::RUNNING;
    }
  }

  bool has_to_move_cursor_back()
  {
    if (_status != FSMStatus::SUCCESS)
      return false;
    if (_final_states.find(_current_state) == _final_states.end())
      return false;
    return _cursor_back_final_states.find(_current_state) != _cursor_back_final_states.end();
  }
};