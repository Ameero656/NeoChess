cmake_minimum_required(VERSION 3.10)
project(ChessProject_CPP VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define an executable
add_executable(ChessGame
    src/main.cpp
    src/core/ChessTypes.h # Headers usually don't need to be listed if included correctly
    src/core/Position.cpp
    src/core/Move.cpp
    src/core/Piece.cpp
    src/core/Pawn.cpp
    src/core/Rook.cpp
    src/core/Knight.cpp
    src/core/Bishop.cpp
    src/core/Queen.cpp
    src/core/King.cpp
    src/core/Board.cpp
    src/core/Game.cpp
    src/player/Player.cpp
    src/player/HumanPlayer.cpp
    src/player/AIPlayer.cpp
    src/ai/EvaluationEngine.cpp
    src/ui/TextDisplay.cpp
)

# Add include directories
target_include_directories(ChessGame PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/src)

# Optional: Compiler flags
# if(CMAKE_COMPILER_IS_GNUXX OR CMAKE_COMPILER_IS_CLANGXX)
#     target_compile_options(ChessGame PRIVATE -Wall -Wextra -pedantic -g)
# elseif(MSVC)
#     target_compile_options(ChessGame PRIVATE /W4 /Zi)
# endif()

install(TARGETS ChessGame DESTINATION bin)

# Optional: Enable CTest for testing
# enable_testing()
# add_test(NAME MyTestCOMMAND ChessGame --some-test-options) # Example