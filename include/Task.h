#ifndef TASK_H
#define TASK_H

#include <string>

// Базовый класс задачи
class Task {
protected:
    std::string name;
    int durationSeconds;
public:
    Task(const std::string& n) : name(n), durationSeconds(0) {}
    virtual ~Task() = default;

    virtual std::string getType() const = 0;
    std::string getName() const { return name; }
    void addTime(int seconds) { durationSeconds += seconds; }
    int getDuration() const { return durationSeconds; }
};

// Конкретный тип: Рабочая задача
class WorkTask : public Task {
public:
    WorkTask(const std::string& n) : Task(n) {}
    std::string getType() const override { return "Work"; }
};

// Конкретный тип: Перерыв
class BreakTask : public Task {
public:
    BreakTask(const std::string& n) : Task(n) {}
    std::string getType() const override { return "Break"; }
};

#endif