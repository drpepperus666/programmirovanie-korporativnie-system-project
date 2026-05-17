#include "TrackerManager.h"
#include "TaskFactory.h"
#include <iostream>

static void printAction(const TrackerManager& tracker) {
    if (!tracker.getLastMessage().empty()) {
        const char* prefix = tracker.wasLastActionOk() ? "[Таймер] " : "[Ошибка] ";
        std::cout << prefix << tracker.getLastMessage() << "\n";
    }
}

int main() {
    std::cout << "=== Запуск Тайм-Трекера ===\n";

    TrackerManager& tracker = TrackerManager::getInstance();

    auto workTask = TaskFactory::createTask(TaskType::WORK, "Написание практической работы");

    tracker.startNewTask(std::move(workTask));
    printAction(tracker);
    std::cout << "Текущее состояние: " << tracker.getCurrentStateName() << "\n\n";

    tracker.pause();
    printAction(tracker);
    std::cout << "Текущее состояние: " << tracker.getCurrentStateName() << "\n\n";

    tracker.resume();
    printAction(tracker);
    std::cout << "Текущее состояние: " << tracker.getCurrentStateName() << "\n\n";

    tracker.stop();
    printAction(tracker);
    std::cout << "Текущее состояние: " << tracker.getCurrentStateName() << "\n";

    if (!tracker.getHistory().empty()) {
        std::cout << "Задача в истории: " << tracker.getHistory().back()->getName()
                  << ", время (сек): " << tracker.getHistory().back()->getDuration() << "\n";
    }

    std::cout << "\n=== Работа завершена успешно ===\n";
    return 0;
}
