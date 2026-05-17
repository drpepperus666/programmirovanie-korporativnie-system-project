#include "TaskFactory.h"

std::unique_ptr<Task> TaskFactory::createTask(TaskType type, const std::string& name) {
    if (type == TaskType::WORK) {
        return std::make_unique<WorkTask>(name);
    } else {
        return std::make_unique<BreakTask>(name);
    }
}