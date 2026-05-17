#include "TrackerManager.h"
#include "TaskFactory.h"
#include <iostream>

void checkExpectedFailure(const TrackerManager& tracker, const std::string& context) {
    std::cout << "[" << context << "] Действие отклонено: " 
              << (!tracker.wasLastActionOk() ? "ДА (Корректно)" : "НЕТ (Ошибка автомата)") 
              << " | Текст: " << tracker.getLastMessage() << "\n";
}

int main() {
    std::cout << "=== СЦЕНАРИЙ 2: Обработка ошибок пользователя ===\n";

    TrackerManager& tracker = TrackerManager::getInstance();
    tracker.resetForTesting();

    // 1. Попытка вызвать остановку, когда ничего не запущено
    tracker.stop();
    checkExpectedFailure(tracker, "Стоп на старте");

    // 2. Запуск задачи
    auto task1 = TaskFactory::createTask(TaskType::WORK, "Задача 1");
    tracker.startNewTask(std::move(task1));
    
    // 3. Попытка запустить вторую задачу поверх выполняющейся
    auto task2 = TaskFactory::createTask(TaskType::WORK, "Задача 2");
    tracker.startNewTask(std::move(task2));
    checkExpectedFailure(tracker, "Повторный старт новой задачи");

    // 4. Перевод на паузу
    tracker.pause();
    
    // 5. Попытка повторно вызвать паузу на паузе
    tracker.pause();
    checkExpectedFailure(tracker, "Двойная пауза");

    // Очищаем состояние
    tracker.stop();
    std::cout << "=== Сценарий 2 успешно завершен ===\n\n";
    return 0;
}