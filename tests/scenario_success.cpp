#include "TrackerManager.h"
#include "TaskFactory.h"
#include <iostream>
#include <cassert>

void checkAction(const TrackerManager& tracker, const std::string& context) {
    std::cout << "[" << context << "] Результат: " 
              << (tracker.wasLastActionOk() ? "УСПЕХ" : "ОШИБКА") 
              << " | Сообщение: " << tracker.getLastMessage() << "\n";
    assert(tracker.wasLastActionOk() && "Критический сбой: действие должно быть успешным!");
}

int main() {
    std::cout << "=== СЦЕНАРИЙ 1: Успешный жизненный цикл задачи ===\n";
    
    TrackerManager& tracker = TrackerManager::getInstance();
    tracker.resetForTesting();

    // 1. Создание и старт задачи
    auto task = TaskFactory::createTask(TaskType::WORK, "Разработка архитектуры");
    tracker.startNewTask(std::move(task));
    checkAction(tracker, "Старт задачи");

    // 2. Постановка на паузу
    tracker.pause();
    checkAction(tracker, "Пауза");

    // 3. Возобновление работы
    tracker.resume();
    checkAction(tracker, "Возобновление");

    // 4. Остановка и сохранение
    tracker.stop();
    checkAction(tracker, "Остановка");

    // Проверка истории
    if (tracker.getHistory().size() == 1) {
        std::cout << "Проверка истории: Пройдена. Задача '" 
                  << tracker.getHistory()[0]->getName() << "' в истории.\n";
    } else {
        std::cout << "Проверка истории: СБОЙ.\n";
        return 1;
    }

    std::cout << "=== Сценарий 1 успешно завершен ===\n\n";
    return 0;
}