#include "TrackerManager.h"
#include "TaskFactory.h"
#include <iostream>
#include <string>

// Функция вывода уведомлений (оставлена из твоей исходной логики)
static void printAction(const TrackerManager& tracker) {
    if (!tracker.getLastMessage().empty()) {
        const char* prefix = tracker.wasLastActionOk() ? "[Таймер] " : "[Ошибка] ";
        std::cout << prefix << tracker.getLastMessage() << "\n";
    }
}

int main() {
    // Подключаем менеджер (Singleton)
    TrackerManager& tracker = TrackerManager::getInstance();
    int choice = 0;

    std::cout << "=== Запуск Интерактивного Тайм-Трекера ===\n";

    while (true) {
        std::cout << "\n-----------------------------------" << "\n";
        std::cout << "Текущее состояние трекера: " << tracker.getCurrentStateName() << "\n";
        std::cout << "1. Создать и ЗАПУСТИТЬ новую задачу\n";
        std::cout << "2. Поставить на ПАУЗУ\n";
        std::cout << "3. ВОЗОБНОВИТЬ выполнение\n";
        std::cout << "4. ОСТАНОВИТЬ задачу (сохранить в историю)\n";
        std::cout << "5. Просмотреть ИСТОРИЮ сессий\n";
        std::cout << "6. Выйти из программы\n";
        std::cout << "Выберите действие (1-6): ";
        
        if (!(std::cin >> choice)) {
            std::cin.clear();
            std::string dummy;
            std::getline(std::cin, dummy);
            std::cout << "[Ошибка] Некорректный ввод. Введите число.\n";
            continue;
        }

        if (choice == 6) {
            std::cout << "\n=== Работа завершена. До свидания! ===\n";
            break;
        }

        switch (choice) {
            case 1: {
                int typeChoice = 0;
                std::cout << "Выберите тип задачи (1 - Работа, 2 - Перерыв): ";
                std::cin >> typeChoice;

                TaskType type;
                if (typeChoice == 1) {
                    type = TaskType::WORK;
                } else if (typeChoice == 2) {
                    type = TaskType::BREAK;
                } else {
                    std::cout << "[Ошибка] Неверный тип задачи.\n";
                    break;
                }

                std::cout << "Введите уникальное имя задачи: ";
                std::cin.ignore(); // Очищаем буфер после ввода чисел
                std::string name;
                std::getline(std::cin, name);

                // Используем Фабричный метод для создания задачи
                auto task = TaskFactory::createTask(type, name);
                
                // Передаем задачу в менеджер
                tracker.startNewTask(std::move(task));
                printAction(tracker);
                break;
            }
            case 2:
                tracker.pause();
                printAction(tracker);
                break;
            case 3:
                tracker.resume();
                printAction(tracker);
                break;
            case 4:
                tracker.stop();
                printAction(tracker);
                break;
            case 5: {
                const auto& history = tracker.getHistory();
                if (history.empty()) {
                    std::cout << "[Информация] История сессий пока пуста.\n";
                } else {
                    std::cout << "\n=== ИСТОРИЯ НАКОПЛЕННЫХ СЕССИЙ ===\n";
                    for (size_t i = 0; i < history.size(); ++i) {
                        std::cout << i + 1 << ". [" << history[i]->getType() << "] "
                                  << "Имя: \"" << history[i]->getName() << "\" | "
                                  << "Время: " << history[i]->getDuration() << " сек.\n";
                    }
                }
                break;
            }
            default:
                std::cout << "[Ошибка] Пожалуйста, выберите пункт от 1 до 6.\n";
                break;
        }
    }

    return 0;
}
