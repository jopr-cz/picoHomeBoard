cmake_minimum_required(VERSION 3.10)
project(MODBUS_test)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)










# Přidání testovacího frameworku
enable_testing()


# Přidání cíle pro testy
add_executable(homeBoardTest homeBoardTest.cpp ../zaluzie.cpp ../homeBoard.cpp)


# Přidání knihoven nebo závislostí potřebných pro tvůj program
target_link_libraries(homeBoardTest PRIVATE jolib linuxSimulator gtest) 

include_directories(/usr/src/googletest/googletest/include/) # this is so we can #include <gtest/gtest.h>

# Přidání testovacích skriptů do adresáře s testy
file(COPY ../jolib/tools/modpool_tester.sh DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

# Přidání testovacího příkazu
add_test(NAME homeBoardTest COMMAND homeBoardTest)


include(GoogleTest)
gtest_discover_tests(homeBoardTest) #