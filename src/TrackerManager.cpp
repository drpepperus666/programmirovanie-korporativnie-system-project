#include "TrackerManager.h"

TrackerManager::TrackerManager() {
    currentState = std::make_shared<StoppedState>();
}

void TrackerManager::setLastResult(bool ok, const std::string& message) {
    lastActionOk = ok;
    lastMessage = message;
}

void TrackerManager::accumulateRunningTime() {
    if (!currentTask) {
        return;
    }
    const auto now = std::chrono::steady_clock::now();
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now - runningSince).count();
    if (seconds > 0) {
        currentTask->addTime(static_cast<int>(seconds));
    }
}

void TrackerManager::changeState(std::shared_ptr<State> newState) {
    if (currentState && currentState->getName() == "Running") {
        accumulateRunningTime();
    }
    currentState = std::move(newState);
    if (currentState->getName() == "Running") {
        runningSince = std::chrono::steady_clock::now();
    }
}

void TrackerManager::startNewTask(std::unique_ptr<Task> task) {
    if (currentState->getName() == "Running") {
        setLastResult(false, "Задача уже выполняется");
        return;
    }
    if (currentState->getName() == "Stopped") {
        currentTask = std::move(task);
    }
    currentState->handleStart(this);
}

void TrackerManager::resume() {
    currentState->handleStart(this);
}

void TrackerManager::pause() {
    currentState->handlePause(this);
}

void TrackerManager::stop() {
    currentState->handleStop(this);
}

void TrackerManager::saveTaskToHistory() {
    if (currentTask) {
        history.push_back(std::move(currentTask));
    }
}

void TrackerManager::addElapsedSeconds(int seconds) {
    if (currentTask && seconds > 0) {
        currentTask->addTime(seconds);
    }
}

void TrackerManager::resetForTesting() {
    if (currentState && currentState->getName() == "Running") {
        accumulateRunningTime();
    }
    currentState = std::make_shared<StoppedState>();
    currentTask.reset();
    history.clear();
    lastMessage.clear();
    lastActionOk = true;
}

void StoppedState::handleStart(TrackerManager* context) {
    const Task* task = context->getCurrentTask();
    context->setLastResult(
        true,
        "Запуск задачи: " + task->getName() + " (" + task->getType() + ")");
    context->changeState(std::make_shared<RunningState>());
}

void StoppedState::handlePause(TrackerManager* context) {
    context->setLastResult(false, "Нельзя поставить на паузу: трекер остановлен");
}

void StoppedState::handleStop(TrackerManager* context) {
    context->setLastResult(false, "Трекер уже остановлен");
}

void RunningState::handleStart(TrackerManager* context) {
    context->setLastResult(false, "Задача уже выполняется");
}

void RunningState::handlePause(TrackerManager* context) {
    context->setLastResult(true, "Пауза для задачи");
    context->changeState(std::make_shared<PausedState>());
}

void RunningState::handleStop(TrackerManager* context) {
    context->setLastResult(true, "Остановка задачи и сохранение результатов");
    context->changeState(std::make_shared<StoppedState>());
    context->saveTaskToHistory();
}

void PausedState::handleStart(TrackerManager* context) {
    context->setLastResult(true, "Продолжение выполнения задачи");
    context->changeState(std::make_shared<RunningState>());
}

void PausedState::handlePause(TrackerManager* context) {
    context->setLastResult(false, "Таймер уже на паузе");
}

void PausedState::handleStop(TrackerManager* context) {
    context->setLastResult(true, "Остановка задачи из режима паузы");
    context->changeState(std::make_shared<StoppedState>());
    context->saveTaskToHistory();
}
