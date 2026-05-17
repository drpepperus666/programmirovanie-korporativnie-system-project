#ifndef TRACKER_MANAGER_H
#define TRACKER_MANAGER_H

#include "Task.h"
#include <chrono>
#include <memory>
#include <string>
#include <vector>

class TrackerManager;

class State {
public:
    virtual ~State() = default;
    virtual void handleStart(TrackerManager* context) = 0;
    virtual void handlePause(TrackerManager* context) = 0;
    virtual void handleStop(TrackerManager* context) = 0;
    virtual std::string getName() const = 0;
};

class TrackerManager {
    std::shared_ptr<State> currentState;
    std::unique_ptr<Task> currentTask;
    std::vector<std::unique_ptr<Task>> history;
    std::chrono::steady_clock::time_point runningSince{};
    std::string lastMessage;
    bool lastActionOk = true;

    TrackerManager();

    void accumulateRunningTime();
    void setLastResult(bool ok, const std::string& message);

    friend class StoppedState;
    friend class RunningState;
    friend class PausedState;

public:
    static TrackerManager& getInstance() {
        static TrackerManager instance;
        return instance;
    }

    TrackerManager(const TrackerManager&) = delete;
    TrackerManager& operator=(const TrackerManager&) = delete;

    void changeState(std::shared_ptr<State> newState);
    void startNewTask(std::unique_ptr<Task> task);
    void resume();
    void pause();
    void stop();

    void saveTaskToHistory();
    void addElapsedSeconds(int seconds);

    void resetForTesting();

    std::string getCurrentStateName() const { return currentState->getName(); }
    Task* getCurrentTask() const { return currentTask.get(); }
    const std::vector<std::unique_ptr<Task>>& getHistory() const { return history; }
    std::string getLastMessage() const { return lastMessage; }
    bool wasLastActionOk() const { return lastActionOk; }
};

class StoppedState : public State {
public:
    void handleStart(TrackerManager* context) override;
    void handlePause(TrackerManager* context) override;
    void handleStop(TrackerManager* context) override;
    std::string getName() const override { return "Stopped"; }
};

class RunningState : public State {
public:
    void handleStart(TrackerManager* context) override;
    void handlePause(TrackerManager* context) override;
    void handleStop(TrackerManager* context) override;
    std::string getName() const override { return "Running"; }
};

class PausedState : public State {
public:
    void handleStart(TrackerManager* context) override;
    void handlePause(TrackerManager* context) override;
    void handleStop(TrackerManager* context) override;
    std::string getName() const override { return "Paused"; }
};

#endif
