#include <gtest/gtest.h>
#include "TaskFactory.h"
#include "TrackerManager.h"

class TrackerFixture : public ::testing::Test {
protected:
    TrackerManager& tracker = TrackerManager::getInstance();

    void SetUp() override { tracker.resetForTesting(); }
};

// --- Task / WorkTask / BreakTask (7 тестов) ---

TEST(TaskTest, WorkTaskReportsWorkType) {
    WorkTask task("Coding");
    EXPECT_EQ(task.getType(), "Work");
}

TEST(TaskTest, BreakTaskReportsBreakType) {
    BreakTask task("Coffee");
    EXPECT_EQ(task.getType(), "Break");
}

TEST(TaskTest, WorkTaskPreservesName) {
    WorkTask task("Report");
    EXPECT_EQ(task.getName(), "Report");
}

TEST(TaskTest, BreakTaskAllowsEmptyName) {
    BreakTask task("");
    EXPECT_TRUE(task.getName().empty());
}

TEST(TaskTest, InitialDurationIsZero) {
    WorkTask task("New");
    EXPECT_EQ(task.getDuration(), 0);
}

TEST(TaskTest, AddTimeIncreasesDuration) {
    WorkTask task("Session");
    task.addTime(60);
    EXPECT_EQ(task.getDuration(), 60);
}

TEST(TaskTest, AddTimeAccumulates) {
    WorkTask task("Session");
    task.addTime(30);
    task.addTime(45);
    EXPECT_EQ(task.getDuration(), 75);
}

// --- TaskFactory (6 тестов) ---

TEST(TaskFactoryTest, CreatesWorkTaskType) {
    auto task = TaskFactory::createTask(TaskType::WORK, "Coding");
    EXPECT_EQ(task->getType(), "Work");
}

TEST(TaskFactoryTest, CreatesBreakTaskType) {
    auto task = TaskFactory::createTask(TaskType::BREAK, "Lunch");
    EXPECT_EQ(task->getType(), "Break");
}

TEST(TaskFactoryTest, WorkTaskKeepsName) {
    auto task = TaskFactory::createTask(TaskType::WORK, "Design");
    EXPECT_EQ(task->getName(), "Design");
}

TEST(TaskFactoryTest, BreakTaskKeepsName) {
    auto task = TaskFactory::createTask(TaskType::BREAK, "Tea");
    EXPECT_EQ(task->getName(), "Tea");
}

TEST(TaskFactoryTest, EmptyNameIsAllowed) {
    auto task = TaskFactory::createTask(TaskType::WORK, "");
    EXPECT_TRUE(task->getName().empty());
}

TEST(TaskFactoryTest, EachCallReturnsDistinctObject) {
    auto a = TaskFactory::createTask(TaskType::WORK, "A");
    auto b = TaskFactory::createTask(TaskType::WORK, "B");
    EXPECT_NE(a.get(), b.get());
}

// --- TrackerManager (7 тестов) ---

TEST_F(TrackerFixture, SingletonReturnsSameInstance) {
    EXPECT_EQ(&TrackerManager::getInstance(), &tracker);
}

TEST_F(TrackerFixture, InitialStateIsStopped) {
    EXPECT_EQ(tracker.getCurrentStateName(), "Stopped");
}

TEST_F(TrackerFixture, StartNewTaskSetsRunningState) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Task"));
    EXPECT_EQ(tracker.getCurrentStateName(), "Running");
    ASSERT_NE(tracker.getCurrentTask(), nullptr);
    EXPECT_EQ(tracker.getCurrentTask()->getName(), "Task");
}

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

// --- StoppedState (5 тестов) ---

TEST_F(TrackerFixture, StoppedPauseIsRejected) {
    tracker.pause();
    EXPECT_EQ(tracker.getCurrentStateName(), "Stopped");
    EXPECT_FALSE(tracker.wasLastActionOk());
}

TEST_F(TrackerFixture, StoppedStopIsRejected) {
    tracker.stop();
    EXPECT_EQ(tracker.getCurrentStateName(), "Stopped");
    EXPECT_FALSE(tracker.wasLastActionOk());
}

TEST_F(TrackerFixture, StoppedStartTransitionsToRunning) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Go"));
    EXPECT_EQ(tracker.getCurrentStateName(), "Running");
    EXPECT_TRUE(tracker.wasLastActionOk());
}

TEST_F(TrackerFixture, StoppedStartWorksWithBreakTask) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::BREAK, "Rest"));
    EXPECT_EQ(tracker.getCurrentStateName(), "Running");
    EXPECT_EQ(tracker.getCurrentTask()->getType(), "Break");
}

TEST_F(TrackerFixture, StoppedPauseSetsErrorMessage) {
    tracker.pause();
    EXPECT_NE(tracker.getLastMessage().find("паузу"), std::string::npos);
}

// --- RunningState (6 тестов) ---

TEST_F(TrackerFixture, RunningDoubleStartIsRejected) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Active"));
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Other"));
    EXPECT_EQ(tracker.getCurrentStateName(), "Running");
    EXPECT_FALSE(tracker.wasLastActionOk());
    EXPECT_EQ(tracker.getCurrentTask()->getName(), "Active");
}

TEST_F(TrackerFixture, RunningPauseTransitionsToPaused) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Task"));
    tracker.pause();
    EXPECT_EQ(tracker.getCurrentStateName(), "Paused");
    EXPECT_TRUE(tracker.wasLastActionOk());
}

TEST_F(TrackerFixture, RunningStopSavesAndReturnsStopped) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Finish"));
    tracker.stop();
    EXPECT_EQ(tracker.getCurrentStateName(), "Stopped");
    EXPECT_EQ(tracker.getHistory().size(), 1u);
}

TEST_F(TrackerFixture, RunningPauseAccumulatesTime) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Split"));
    tracker.addElapsedSeconds(40);
    tracker.pause();
    tracker.stop();
    ASSERT_EQ(tracker.getHistory().size(), 1u);
    EXPECT_EQ(tracker.getHistory()[0]->getDuration(), 40);
}

TEST_F(TrackerFixture, RunningDoubleStartSetsErrorMessage) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "One"));
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Two"));
    EXPECT_NE(tracker.getLastMessage().find("уже"), std::string::npos);
}

TEST_F(TrackerFixture, RunningStopSetsSuccessMessage) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "End"));
    tracker.stop();
    EXPECT_TRUE(tracker.wasLastActionOk());
    EXPECT_NE(tracker.getLastMessage().find("Остановка"), std::string::npos);
}

// --- PausedState (5 тестов) ---

TEST_F(TrackerFixture, PausedResumeReturnsToRunning) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Resume"));
    tracker.pause();
    tracker.resume();
    EXPECT_EQ(tracker.getCurrentStateName(), "Running");
    EXPECT_TRUE(tracker.wasLastActionOk());
    ASSERT_NE(tracker.getCurrentTask(), nullptr);
    EXPECT_EQ(tracker.getCurrentTask()->getName(), "Resume");
}

TEST_F(TrackerFixture, PausedDoublePauseIsRejected) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "Hold"));
    tracker.pause();
    tracker.pause();
    EXPECT_EQ(tracker.getCurrentStateName(), "Paused");
    EXPECT_FALSE(tracker.wasLastActionOk());
}

TEST_F(TrackerFixture, PausedStopSavesToHistory) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "FromPause"));
    tracker.pause();
    tracker.stop();
    ASSERT_EQ(tracker.getHistory().size(), 1u);
    EXPECT_EQ(tracker.getHistory()[0]->getName(), "FromPause");
}

TEST_F(TrackerFixture, PausedStopReturnsStopped) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::WORK, "X"));
    tracker.pause();
    tracker.stop();
    EXPECT_EQ(tracker.getCurrentStateName(), "Stopped");
}

TEST_F(TrackerFixture, BreakTaskPausedThenStopped) {
    tracker.startNewTask(TaskFactory::createTask(TaskType::BREAK, "Break"));
    tracker.pause();
    tracker.stop();
    ASSERT_EQ(tracker.getHistory().size(), 1u);
    EXPECT_EQ(tracker.getHistory()[0]->getType(), "Break");
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
