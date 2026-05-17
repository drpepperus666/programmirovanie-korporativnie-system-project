#include "TrackerManager.h"
#include "TaskFactory.h"
#include <iostream>

int main() {
    std::cout << "=== СЦЕНАРИЙ 3: Чередование задач и перерывов (Pomodoro) ===\n";

    TrackerManager& tracker = TrackerManager::getInstance();
    tracker.resetForTesting();

    // Сессия 1: Работа
    std::cout << "-> Шаг 1: Запуск Рабочей сессии №1\n";
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Кодинг спринта"));
    tracker.stop();

    // Сессия 2: Перерыв
    std::cout << "-> Шаг 2: Время на перерыв\n";
    tracker.startNewTask(TaskFactory::createTask(TaskType::BREAK, "Кофе-брейк"));
    tracker.stop();

    // Сессия 3: Снова работа
    std::cout << "-> Шаг 3: Запуск Рабочей сессии №2\n";
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Ревью кода"));
    tracker.stop();

    // Итоговая валидация последовательности элементов в истории
    std::cout << "\n=== ИТОГОВЫЙ ОТЧЕТ СЦЕНАРИЯ ===\n";
    const auto& history = tracker.getHistory();
    std::cout << "Всего задач в логе: " << history.size() << " (Ожидается: 3)\n";

    for (size_t i = 0; i < history.size(); ++i) {
        std::cout << "Запись [" << i + 1 << "]: " 
                  << history[i]->getName() << " [" << history[i]->getType() << "]\n";
    }

    if (history.size() == 3 && history[1]->getType() == "Break") {
        std::cout << "Валидация последовательности: УСПЕШНО\n";
    } else {
        std::cout << "Валидация последовательности: НЕУСПЕШНО\n";
        return 1;
    }

    std::cout << "=== Сценарий 3 успешно завершен ===\n\n";
    return 0;
}