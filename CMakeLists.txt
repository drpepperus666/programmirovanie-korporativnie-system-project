cmake_minimum_required(VERSION 3.10)
project(TimeTracker VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Определяем общие исходные файлы бизнес-логики (без main.cpp)
set(LOGIC_SOURCES
    src/TaskFactory.cpp
    src/TrackerManager.cpp
)

# 1. Сборка основного приложения
add_executable(target_exec src/main.cpp ${LOGIC_SOURCES})
target_include_directories(target_exec PUBLIC include)

# 2. Поиск и подключение Google Test библиотеки
find_package(GTest REQUIRED)

# 3. Сборка основного пакета автоматических Unit-тестов (Пункт 2 задания)
add_executable(run_tests tests/test_main.cpp ${LOGIC_SOURCES})
target_include_directories(run_tests PRIVATE include)
target_link_libraries(run_tests GTest::gtest GTest::gtest_main)

# 4. Сборка отдельных мини-программ интеграционных сценариев (Пункты 3-4 задания)
# Предполагается, что файлы сценариев лежат в папке tests/

# Сценарий 1: Успех
add_executable(scenario_success tests/scenario_success.cpp ${LOGIC_SOURCES})
target_include_directories(scenario_success PRIVATE include)

# Сценарий 2: Ошибки
add_executable(scenario_errors tests/scenario_errors.cpp ${LOGIC_SOURCES})
target_include_directories(scenario_errors PRIVATE include)

# Сценарий 3: Чередование (Цепочка Pomodoro)
add_executable(scenario_sequence tests/scenario_sequence.cpp ${LOGIC_SOURCES})
target_include_directories(scenario_sequence PRIVATE include)