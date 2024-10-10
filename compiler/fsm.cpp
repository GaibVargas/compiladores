#include "fsm.h"

void FiniteStateMachine::reset()
{
  _current_state = _initial_state;
  _lexeme = "";
  _status = FSMStatus::IDLE;
}

FSMStatus FiniteStateMachine::transition(char c)
{
  if (_status == FSMStatus::ERROR)
    return _status;

  if (_transitions.find(_current_state) == _transitions.end())
  {
    _status = FSMStatus::ERROR;
    return _status;
  }
  auto next_state = _transitions.at(_current_state)(c);
  _current_state = next_state;

  if (next_state == "dead")
  {
    _status = FSMStatus::ERROR;
    return _status;
  }

  if (_final_states.find(next_state) != _final_states.end())
  {
    _status = FSMStatus::SUCCESS;
  }
  else
  {
    _status = FSMStatus::RUNNING;
  }

  if (!has_to_move_cursor_back())
    _lexeme += c;

  return _status;
}

bool FiniteStateMachine::has_to_move_cursor_back()
{
  if (_status != FSMStatus::SUCCESS)
    return false;
  if (_final_states.find(_current_state) == _final_states.end())
    return false;
  return _cursor_back_final_states.find(_current_state) != _cursor_back_final_states.end();
}