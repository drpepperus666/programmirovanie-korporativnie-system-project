#include <gtest/gtest.h>
#include "TaskFactory.h"
#include "TrackerManager.h"

// Фикстура для тестов менеджера — сбрасывает синглтон перед каждым тестом
class TrackerFixture : public ::testing::Test {
protected:
    TrackerManager& tracker = TrackerManager::getInstance();

    void SetUp() override { tracker.resetForTesting(); }
};

// ============================================================================
// 1. ТЕСТЫ ДЛЯ КЛАССОВ СЕМЕЙСТВА TASK
// ============================================================================

// --- Функция Task::getType() (6 тестов) ---
TEST(TaskTypeTest, WorkTaskReportsWorkType) {
    WorkTask task("Coding");
    EXPECT_EQ(task.getType(), "Work");
}

TEST(TaskTypeTest, BreakTaskReportsBreakType) {
    BreakTask task("Coffee");
    EXPECT_EQ(task.getType(), "Break");
}

TEST(TaskTypeTest, FactoryWorkTaskType) {
    auto task = TaskFactory::createTask(TaskType::WORK, "X");
    EXPECT_EQ(task->getType(), "Work");
}

TEST(TaskTypeTest, FactoryBreakTaskType) {
    auto task = TaskFactory::createTask(TaskType::BREAK, "Y");
    EXPECT_EQ(task->getType(), "Break");
}

TEST(TaskTypeTest, WorkTaskTypeIsConstant) {
    WorkTask task("Test");
    std::string type1 = task.getType();
    std::string type2 = task.getType();
    EXPECT_EQ(type1, type2);
}

TEST(TaskTypeTest, BreakTaskTypeIsConstant) {
    BreakTask task("Test");
    EXPECT_EQ(task.getType(), "Break");
    EXPECT_NE(task.getType(), "Work");
}

// --- Функция Task::getName() (6 тестов) ---
TEST(TaskNameTest, PreservesStandardName) {
    WorkTask task("Report");
    EXPECT_EQ(task.getName(), "Report");
}

TEST(TaskNameTest, AllowsEmptyName) {
    BreakTask task("");
    EXPECT_TRUE(task.getName().empty());
}

TEST(TaskNameTest, HandlesSpecialCharacters) {
    WorkTask task("Task_#123@D!");
    EXPECT_EQ(task.getName(), "Task_#123@D!");
}

TEST(TaskNameTest, HandlesSpacesInName) {
    WorkTask task("Very Long Task Name With Spaces");
    EXPECT_EQ(task.getName(), "Very Long Task Name With Spaces");
}

TEST(TaskNameTest, HandlesNumbersInName) {
    BreakTask task("12345");
    EXPECT_EQ(task.getName(), "12345");
}

TEST(TaskNameTest, HandlesVeryLongName) {
    std::string longName(500, 'x');
    WorkTask task(longName);
    EXPECT_EQ(task.getName(), longName);
}

// --- Функции Task::addTime() и Task::getDuration() (6 тестов) ---
TEST(TaskDurationTest, InitialDurationIsZero) {
    WorkTask task("New");
    EXPECT_EQ(task.getDuration(), 0);
}

TEST(TaskDurationTest, AddTimeIncreasesDuration) {
    WorkTask task("Session");
    task.addTime(60);
    EXPECT_EQ(task.getDuration(), 60);
}

TEST(TaskDurationTest, AddTimeAccumulates) {
    WorkTask task("Session");
    task.addTime(30);
    task.addTime(45);
    EXPECT_EQ(task.getDuration(), 75);
}

TEST(TaskDurationTest, AddZeroSecondsDoesNotChangeDuration) {
    WorkTask task("Test");
    task.addTime(0);
    EXPECT_EQ(task.getDuration(), 0);
}

TEST(TaskDurationTest, AddLargeAmountOfSeconds) {
    WorkTask task("Test");
    task.addTime(86400); // Сутки
    EXPECT_EQ(task.getDuration(), 86400);
}

TEST(TaskDurationTest, AddTimeNegativeValueIsPreserved) {
    WorkTask task("Test");
    task.addTime(-10);
    EXPECT_EQ(task.getDuration(), -10);
}


// ============================================================================
// 2. ТЕСТЫ ДЛЯ КЛАССА TASKFACTORY
// ============================================================================

// --- Функция TaskFactory::createTask() (6 тестов) ---
TEST(TaskFactoryTest, CreatesValidWorkTaskObject) {
    auto task = TaskFactory::createTask(TaskType::WORK, "Design");
    ASSERT_NE(task, nullptr);
    EXPECT_EQ(task->getName(), "Design");
}

TEST(TaskFactoryTest, CreatesValidBreakTaskObject) {
    auto task = TaskFactory::createTask(TaskType::BREAK, "Tea");
    ASSERT_NE(task, nullptr);
    EXPECT_EQ(task->getName(), "Tea");
}

TEST(TaskFactoryTest, FactoryAllowsEmptyNameForWork) {
    auto task = TaskFactory::createTask(TaskType::WORK, "");
    EXPECT_TRUE(task->getName().empty());
}

TEST(TaskFactoryTest, FactoryAllowsEmptyNameForBreak) {
    auto task = TaskFactory::createTask(TaskType::BREAK, "");
    EXPECT_TRUE(task->getName().empty());
}

TEST(TaskFactoryTest, EachCallReturnsDistinctObject) {
    auto a = TaskFactory::createTask(TaskType::WORK, "A");
    auto b = TaskFactory::createTask(TaskType::WORK, "A");
    EXPECT_NE(a.get(), b.get());
}

TEST(TaskFactoryTest, CreatedTaskHasZeroDuration) {
    auto task = TaskFactory::createTask(TaskType::WORK, "Fresh");
    EXPECT_EQ(task->getDuration(), 0);
}


// ============================================================================
// 3. ТЕСТЫ ДЛЯ КЛАССА TRACKERMANAGER
// ============================================================================

// --- Функция TrackerManager::startNewTask() (6 тестов) ---
TEST_F(TrackerFixture, StartNewTaskSetsRunningState) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Task"));
    EXPECT_EQ(tracker.getCurrentStateName(), "Running");
}

TEST_F(TrackerFixture, StartNewTaskSavesPointer) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Task"));
    ASSERT_NE(tracker.getCurrentTask(), nullptr);
    EXPECT_EQ(tracker.getCurrentTask()->getName(), "Task");
}

TEST_F(TrackerFixture, StartNewTaskFailsIfAlreadyRunning) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Active"));
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Other"));
    EXPECT_FALSE(tracker.wasLastActionOk());
    EXPECT_EQ(tracker.getCurrentTask()->getName(), "Active");
}

TEST_F(TrackerFixture, StartNewTaskWorksWithBreakTask) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::BREAK, "Rest"));
    EXPECT_EQ(tracker.getCurrentStateName(), "Running");
    EXPECT_EQ(tracker.getCurrentTask()->getType(), "Break");
}

TEST_F(TrackerFixture, StartNewTaskResetsLastMessageOnSuccess) {
    tracker.pause(); 
    std::string oldMsg = tracker.getLastMessage();
    
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Valid"));
    EXPECT_TRUE(tracker.wasLastActionOk());
    EXPECT_NE(tracker.getLastMessage(), oldMsg);
}

TEST_F(TrackerFixture, StartNewTaskWithLongNameWorks) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "VeryVeryLongTaskNameInsideTrackerManagerToVerifyIt"));
    EXPECT_TRUE(tracker.wasLastActionOk());
    EXPECT_EQ(tracker.getCurrentStateName(), "Running");
}

// --- Функция TrackerManager::pause() (6 тестов) ---
TEST_F(TrackerFixture, PauseInRunningStateTransitionsToPaused) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Task"));
    tracker.pause();
    EXPECT_EQ(tracker.getCurrentStateName(), "Paused");
    EXPECT_TRUE(tracker.wasLastActionOk());
}

TEST_F(TrackerFixture, PauseInStoppedStateIsRejected) {
    tracker.pause();
    EXPECT_EQ(tracker.getCurrentStateName(), "Stopped");
    EXPECT_FALSE(tracker.wasLastActionOk());
}

TEST_F(TrackerFixture, DoublePauseIsRejected) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Hold"));
    tracker.pause(); 
    tracker.pause(); 
    EXPECT_FALSE(tracker.wasLastActionOk());
    EXPECT_EQ(tracker.getCurrentStateName(), "Paused");
}

// --- Изменено на эмуляцию накопления времени вручную ---
TEST_F(TrackerFixture, PauseAccumulatesElapsedTime) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Split"));
    tracker.addElapsedSeconds(40);
    tracker.pause();
    EXPECT_EQ(tracker.getCurrentTask()->getDuration(), 40);
}

TEST_F(TrackerFixture, PauseSetsErrorMessageInStoppedState) {
    tracker.pause();
    EXPECT_NE(tracker.getLastMessage().find("Нельзя"), std::string::npos);
}

TEST_F(TrackerFixture, PauseWorksForBreakTasks) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::BREAK, "Relax"));
    tracker.pause();
    EXPECT_EQ(tracker.getCurrentStateName(), "Paused");
}

// --- Функция TrackerManager::resume() (5 тестов) ---
TEST_F(TrackerFixture, ResumeInPausedStateReturnsToRunning) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Resume"));
    tracker.pause();
    tracker.resume();
    EXPECT_EQ(tracker.getCurrentStateName(), "Running");
    EXPECT_TRUE(tracker.wasLastActionOk());
}

TEST_F(TrackerFixture, ResumeMultipleTimesInRunningStateIsRejected) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Task"));
    tracker.pause();
    tracker.resume(); // Становится Running
    tracker.resume(); // Повторный вызов отклоняется
    EXPECT_FALSE(tracker.wasLastActionOk());
}

TEST_F(TrackerFixture, ResumeInRunningStateIsRejected) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "A"));
    tracker.resume(); 
    EXPECT_FALSE(tracker.wasLastActionOk());
    EXPECT_EQ(tracker.getCurrentStateName(), "Running");
}

TEST_F(TrackerFixture, ResumePreservesCurrentTaskData) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "KeepMe"));
    tracker.pause();
    tracker.resume();
    ASSERT_NE(tracker.getCurrentTask(), nullptr);
    EXPECT_EQ(tracker.getCurrentTask()->getName(), "KeepMe");
}

TEST_F(TrackerFixture, ResumeSetsSuccessMessage) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Msg"));
    tracker.pause();
    tracker.resume();
    EXPECT_TRUE(tracker.wasLastActionOk());
    EXPECT_NE(tracker.getLastMessage().find("Продолжение"), std::string::npos);
}

// --- Функция TrackerManager::stop() (6 тестов) ---
TEST_F(TrackerFixture, StopSavesTaskToHistory) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Done"));
    tracker.stop();
    ASSERT_EQ(tracker.getHistory().size(), 1u);
    EXPECT_EQ(tracker.getHistory()[0]->getName(), "Done");
}

TEST_F(TrackerFixture, StopClearsCurrentTask) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Temp"));
    tracker.stop();
    EXPECT_EQ(tracker.getCurrentTask(), nullptr);
}

TEST_F(TrackerFixture, MultipleStopsGrowHistory) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "A"));
    tracker.stop();
    tracker.startNewTask(TaskFactory::createTask(TaskType::BREAK, "B"));
    tracker.stop();
    EXPECT_EQ(tracker.getHistory().size(), 2u);
}

TEST_F(TrackerFixture, StopAccumulatesElapsedTime) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Timed"));
    tracker.addElapsedSeconds(120);
    tracker.stop();
    ASSERT_EQ(tracker.getHistory().size(), 1u);
    EXPECT_EQ(tracker.getHistory()[0]->getDuration(), 120);
}

TEST_F(TrackerFixture, StopInStoppedStateIsRejected) {
    tracker.stop();
    EXPECT_FALSE(tracker.wasLastActionOk());
    EXPECT_EQ(tracker.getCurrentStateName(), "Stopped");
}

TEST_F(TrackerFixture, StopCalledInPausedStateSavesCorrectly) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "FromPause"));
    tracker.pause();
    tracker.stop();
    EXPECT_EQ(tracker.getHistory().size(), 1u);
    EXPECT_EQ(tracker.getCurrentStateName(), "Stopped");
}

// --- Дополнительные тесты геттеров и синглтона (5 тестов) ---
TEST_F(TrackerFixture, SingletonReturnsSameInstance) {
    EXPECT_EQ(&TrackerManager::getInstance(), &tracker);
}

TEST_F(TrackerFixture, InitialStateIsStopped) {
    EXPECT_EQ(tracker.getCurrentStateName(), "Stopped");
}

TEST_F(TrackerFixture, HistoryReturnsEmptyVectorInitially) {
    EXPECT_TRUE(tracker.getHistory().empty());
}

TEST_F(TrackerFixture, ResetForTestingClearsEverything) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "ToReset"));
    tracker.stop();
    tracker.resetForTesting();
    EXPECT_TRUE(tracker.getHistory().empty());
    EXPECT_EQ(tracker.getCurrentTask(), nullptr);
    EXPECT_EQ(tracker.getCurrentStateName(), "Stopped");
}

TEST_F(TrackerFixture, OrderOfStatesInPomodoroChain) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Pomo"));
    EXPECT_EQ(tracker.getCurrentStateName(), "Running");
    tracker.pause();
    EXPECT_EQ(tracker.getCurrentStateName(), "Paused");
    tracker.resume();
    EXPECT_EQ(tracker.getCurrentStateName(), "Running");
    tracker.stop();
    EXPECT_EQ(tracker.getCurrentStateName(), "Stopped");
}

// ============================================================================
// ТОЧКА ВХОДА ДЛЯ ЗАПУСКА ВСЕХ ТЕСТОВ Google Test
// ============================================================================
int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
