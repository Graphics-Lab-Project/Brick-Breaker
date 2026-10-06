#pragma once
// OWNER: Phase 0 (interface + data members FROZEN). Implementation: task "Game state machine" -> GameStateMachine.cpp
#include "Types.h"

namespace BB {

enum class GameEvent { StartGame, Launch, TogglePause, BallLost, LastBallLost, LevelCleared, NextLevel, QuitToMenu };

// Valid transitions (everything else is rejected and returns false):
//  StartGame    : Menu | GameOver        -> Ready
//  Launch       : Ready                  -> Playing
//  TogglePause  : Ready | Playing        -> Paused (remembers which)
//                 Paused                 -> the remembered state
//  BallLost     : Playing                -> Ready     (lives remain)
//  LastBallLost : Playing                -> GameOver
//  LevelCleared : Playing                -> LevelCleared
//  NextLevel    : LevelCleared           -> Ready
//  QuitToMenu   : any state except Menu  -> Menu
class GameStateMachine {
public:
    GameState state() const { return m_state; }
    // Returns true if the event caused a transition.
    bool handle(GameEvent e);
    void reset();   // -> Menu
    // Phase 0 helper for the engine/tests: set the state without validation.
    void forceState(GameState s) { m_state = s; }

private:
    GameState m_state = GameState::Menu;
    GameState m_beforePause = GameState::Ready;
};

} // namespace BB
