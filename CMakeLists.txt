cmake_minimum_required(VERSION 3.21)
project(BrickBreaker VERSION 0.1 LANGUAGES CXX)

# OWNER: integrator (Person A). Read-only for task agents (AGENTS.md R8).

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Core Gui Qml Quick QuickTest Test Sql)
qt_standard_project_setup()

# ---------------------------------------------------------------------------
# Engine + QML module (static library, so tests import the real module)
# ---------------------------------------------------------------------------
set(BB_ENGINE_SOURCES
    src/engine/Constants.h
    src/engine/Types.h
    src/engine/Rng.h
    src/engine/Collision.h          src/engine/Collision.cpp
    src/engine/PaddleLogic.h        src/engine/PaddleLogic.cpp
    src/engine/BallPhysics.h        src/engine/BallPhysics.cpp
    src/engine/BrickGrid.h          src/engine/BrickGrid.cpp
    src/engine/Levels.h             src/engine/Levels.cpp
    src/engine/CapsuleSystem.h      src/engine/CapsuleSystem.cpp
    src/engine/PowerUps.h           src/engine/PowerUps.cpp
    src/engine/WeaponSystem.h       src/engine/WeaponSystem.cpp
    src/engine/PaceController.h     src/engine/PaceController.cpp
    src/engine/GameStateMachine.h   src/engine/GameStateMachine.cpp
    src/engine/HighScoreTable.h     src/engine/HighScoreTable.cpp
    src/engine/ScoreStore.h
    src/engine/SqliteScoreStore.h   src/engine/SqliteScoreStore.cpp
    src/engine/Models.h             src/engine/Models.cpp
    src/engine/GameEngine.h
    src/engine/GameEngine_loop.cpp
    src/engine/GameEngine.cpp
    src/engine/GameEngine_balls.cpp
    src/engine/GameEngine_powerups.cpp
)

set(BB_QML_FILES
    qml/Theme.qml
    qml/Main.qml
    qml/AppRoot.qml
    qml/MenuScreen.qml
    qml/LevelSelectScreen.qml
    qml/OptionsScreen.qml
    qml/HelpScreen.qml
    qml/GameScreen.qml
    qml/Hud.qml
    qml/PauseButton.qml
    qml/InputHint.qml
    qml/Playfield.qml
    qml/Board.qml
    qml/Brick.qml
    qml/Paddle.qml
    qml/Ball.qml
    qml/Capsule.qml
    qml/Projectile.qml
    qml/FxLayer.qml
    qml/PauseOverlay.qml
    qml/LevelBanner.qml
    qml/InitialsEntry.qml
    qml/GameOverOverlay.qml
)

set_source_files_properties(qml/Theme.qml PROPERTIES QT_QML_SINGLETON_TYPE TRUE)
foreach(f IN LISTS BB_QML_FILES)
    get_filename_component(_name "${f}" NAME)
    set_source_files_properties("${f}" PROPERTIES QT_RESOURCE_ALIAS "${_name}")
endforeach()

qt_add_library(bbcore STATIC)
qt_add_qml_module(bbcore
    URI BrickBreaker
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/qml/BrickBreaker
    SOURCES ${BB_ENGINE_SOURCES}
    QML_FILES ${BB_QML_FILES}
)
target_include_directories(bbcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src/engine)
target_link_libraries(bbcore PUBLIC Qt6::Core Qt6::Gui Qt6::Qml Qt6::Quick Qt6::Sql)

# Optional bundled fonts (Silkscreen, OFL). Added by the integration task; the build works without them.
file(GLOB BB_FONT_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/fonts/*.ttf")
if(BB_FONT_FILES)
    qt_add_resources(bbcore "bb_fonts" PREFIX "/" BASE "${CMAKE_CURRENT_SOURCE_DIR}" FILES ${BB_FONT_FILES})
endif()

# ---------------------------------------------------------------------------
# Application
# ---------------------------------------------------------------------------
qt_add_executable(brickbreaker src/app/main.cpp)
target_link_libraries(brickbreaker PRIVATE bbcoreplugin Qt6::Gui Qt6::Quick)
set_target_properties(brickbreaker PROPERTIES WIN32_EXECUTABLE TRUE MACOSX_BUNDLE TRUE)

# ---------------------------------------------------------------------------
# Tests (headless; QT_QPA_PLATFORM=offscreen is set per test)
# ---------------------------------------------------------------------------
enable_testing()
add_subdirectory(tests)
