// OWNER: task "Game state machine". Transition table is documented in GameStateMachine.h.
#include "GameStateMachine.h"

namespace BB {

bool GameStateMachine::handle(GameEvent e)
{
    switch (e) {
    case GameEvent::StartGame:
        if (m_state == GameState::Menu || m_state == GameState::GameOver) {
            m_state = GameState::Ready;
            return true;
        }
        return false;
    case GameEvent::Launch:
        if (m_state == GameState::Ready) {
            m_state = GameState::Playing;
            return true;
        }
        return false;
    case GameEvent::TogglePause:
        if (m_state == GameState::Ready || m_state == GameState::Playing) {
            m_beforePause = m_state;
            m_state = GameState::Paused;
            return true;
        }
        if (m_state == GameState::Paused) {
            m_state = m_beforePause;
            return true;
        }
        return false;
    case GameEvent::BallLost:
        if (m_state == GameState::Playing) {
            m_state = GameState::Ready;
            return true;
        }
        return false;
    case GameEvent::LastBallLost:
        if (m_state == GameState::Playing) {
            m_state = GameState::GameOver;
            return true;
        }
        return false;
    case GameEvent::LevelCleared:
        if (m_state == GameState::Playing) {
            m_state = GameState::LevelCleared;
            return true;
        }
        return false;
    case GameEvent::NextLevel:
        if (m_state == GameState::LevelCleared) {
            m_state = GameState::Ready;
            return true;
        }
        return false;
    case GameEvent::QuitToMenu:
        if (m_state != GameState::Menu) {
            m_state = GameState::Menu;
            return true;
        }
        return false;
    }
    return false;
}

void GameStateMachine::reset()
{
    m_state = GameState::Menu;
    m_beforePause = GameState::Ready;
}

} // namespace BB
