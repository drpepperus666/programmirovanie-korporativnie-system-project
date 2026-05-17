#ifndef TASK_FACTORY_H
#define TASK_FACTORY_H

#include "Task.h"
#include <memory>

enum class TaskType { WORK, BREAK };

class TaskFactory {
public:
    // Фабричный метод, возвращающий умный указатель на базовый класс
    static std::unique_ptr<Task> createTask(TaskType type, const std::string& name);
};

#endif