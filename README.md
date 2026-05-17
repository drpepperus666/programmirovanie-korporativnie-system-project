# Проект Тайм-Трекер

Консольное приложение для учёта рабочего времени на языке C++.

## Использованные паттерны

1. **Singleton (Одиночка)** — класс `TrackerManager`.
2. **Factory Method (Фабричный метод)** — класс `TaskFactory`.
3. **State (Состояние)** — управление логикой таймера (`Running`, `Paused`, `Stopped`).

## Требования

- [CMake](https://cmake.org/) 3.10+
- Компилятор C++17 (**MSYS2 UCRT64**: `g++`, `ninja`, `cmake`)
- Google Test для модульных тестов

Установка зависимостей в **MSYS2 UCRT64** (терминал `ucrt64`):

```bash
pacman -S --needed mingw-w64-ucrt64-toolchain mingw-w64-ucrt64-cmake mingw-w64-ucrt64-ninja mingw-w64-ucrt64-gtest
```

---

## Быстрый старт (Windows, PowerShell)

Все команды ниже — из **корня проекта** (`KR4`), если не указано иное.

```powershell
# 1. DLL MSYS2 (нужно каждый раз в новом окне PowerShell)
$env:PATH = "C:\msys64\ucrt64\bin;" + $env:PATH

# 2. Первая сборка
cmake -S . -B build
cmake --build build

# 3. Запуск программы
cd build
.\target_exec.exe

# 4. Запуск тестов
.\run_tests.exe
cd ..
```

---

## Сборка и пересборка

### Первая сборка (создать каталог `build/`)

```bash
cmake -S . -B build
cmake --build build
```

- `cmake -S . -B build` — конфигурация (генерация Ninja/Makefile в `build/`, исходники не засоряются).
- `cmake --build build` — компиляция целей `target_exec` и `run_tests`.

### Пересборка после изменения кода

Достаточно одной команды (из корня проекта):

```bash
cmake --build build
```

Пересобираются только изменённые файлы.

### Переконфигурация (изменили `CMakeLists.txt`)

```bash
cmake -S . -B build
cmake --build build
```

### Полная пересборка с нуля (clean)

**Вариант 1 — удалить каталог `build/`** (самый простой):

```powershell
# PowerShell, из корня KR4
Remove-Item -Recurse -Force build
cmake -S . -B build
cmake --build build
```

```bash
# MSYS2 / bash
rm -rf build
cmake -S . -B build
cmake --build build
```

**Вариант 2 — очистка через CMake, без удаления папки:**

```bash
cmake --build build --target clean
cmake --build build
```

**Вариант 3 — одна пересборка «с нуля» через Ninja:**

```bash
cmake --build build --clean-first
```

---

## Запуск программы

Собираются две программы в `build/`:

| Файл                              | Назначение                              |
| --------------------------------- | --------------------------------------- |
| `target_exec` / `target_exec.exe` | Основное приложение (демо тайм-трекера) |
| `run_tests` / `run_tests.exe`     | Модульные тесты (Google Test)           |

### MSYS2 UCRT64 (bash)

```bash
cd build
./target_exec
```

### Windows PowerShell

```powershell
$env:PATH = "C:\msys64\ucrt64\bin;" + $env:PATH
cd build
.\target_exec.exe
```

Ожидаемый вывод: запуск задачи → пауза → продолжение → остановка → состояние `Stopped` и запись в историю.

> **Важно:** без `C:\msys64\ucrt64\bin` в `PATH` Windows может выдать ошибку отсутствующих DLL (`0xC0000139`). Добавляйте путь в `PATH` в каждом новом окне терминала или пропишите его в переменных среды системы.

---

## Запуск тестов

Тесты собираются вместе с проектом (цель `run_tests`).

```bash
# из корня проекта
cmake --build build
cd build
./run_tests          # bash
.\run_tests.exe      # PowerShell
```

Успешный прогон:

```text
[==========] Running 36 tests from 3 test suites.
...
[  PASSED  ] 36 tests.
```

Запуск **только тестов** без пересборки приложения (если уже собрано):

```bash
cd build
./run_tests
```

Запуск **одного** теста (фильтр Google Test):

```bash
./run_tests --gtest_filter=TaskTest.*
./run_tests --gtest_filter=TrackerFixture.StopSavesTaskToHistory
```

---

## Типичные сценарии

| Задача                   | Команды                                                                      |
| ------------------------ | ---------------------------------------------------------------------------- |
| Изменил `.cpp` / `.h`    | `cmake --build build` → запуск `target_exec` или `run_tests`                 |
| Изменил `CMakeLists.txt` | `cmake -S . -B build` → `cmake --build build`                                |
| Странные ошибки линковки | Удалить `build/`, собрать заново (см. **clean** выше)                        |
| Проверить перед сдачей   | `cmake --build build` → `cd build` → `.\run_tests.exe` → `.\target_exec.exe` |

---

## Тесты (состав)

В `tests/test_main.cpp` — **36** тестов на Google Test:

| Группа                            | Количество |
| --------------------------------- | ---------- |
| `Task` / `WorkTask` / `BreakTask` | 7          |
| `TaskFactory`                     | 6          |
| `TrackerManager`                  | 7          |
| `StoppedState`                    | 5          |
| `RunningState`                    | 6          |
| `PausedState`                     | 5          |

Перед тестами менеджера вызывается `resetForTesting()` (фикстура `TrackerFixture`) — singleton не «загрязняется» между тестами.

---

## Структура проекта

```
KR4/
├── CMakeLists.txt
├── README.md
├── include/
│   ├── Task.h
│   ├── TaskFactory.h
│   └── TrackerManager.h
├── src/
│   ├── main.cpp
│   ├── TaskFactory.cpp
│   └── TrackerManager.cpp
├── tests/
│   └── test_main.cpp
└── build/              # каталог сборки (создаётся cmake, можно удалять)
    ├── target_exec.exe
    └── run_tests.exe
```

---

## Устранение неполадок

| Проблема                        | Решение                                                                                                       |
| ------------------------------- | ------------------------------------------------------------------------------------------------------------- |
| `cmake` не найден               | Открыть **MSYS2 UCRT64**, установить пакеты (см. **Требования**) или добавить `C:\msys64\ucrt64\bin` в `PATH` |
| `Could NOT find GTest`          | `pacman -S mingw-w64-ucrt64-gtest`, затем `cmake -S . -B build` заново                                        |
| exe не запускается в PowerShell | `$env:PATH = "C:\msys64\ucrt64\bin;" + $env:PATH`                                                             |
| Старая сборка мешает            | Удалить папку `build` и собрать с нуля                                                                        |
