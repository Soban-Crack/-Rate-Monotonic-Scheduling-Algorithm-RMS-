#include <iostream>
#include <vector>
#include <list>
#include <cmath>
#include <algorithm>
#include <numeric>

using namespace std;

class Task {
public:
    int p;  // period
    int c;  // computation time
    int r;  // remaining computation time after preemption
    int entryTime;
    int id;

    Task(int period, int cTime, int identification) {
        p = period;
        c = cTime;
        r = cTime;
        id = identification;
        entryTime = 0;
    }
};

class ReadyQueue {
public:
    Task* lastExecutedTask;
    list<Task*> TheQueue;
    int timeLapsed;

    ReadyQueue() {
        timeLapsed = 0;
        lastExecutedTask = nullptr;
    }

    int executeOneUnit() {
        if (TheQueue.empty()) {
            timeLapsed++;
            return 0;
        }
        timeLapsed++;
        Task* T = TheQueue.front();
        if (T->r <= 0) {
            throw runtime_error("Remaining time became negative, or is 0 from the first execution");
        }
        T->r--;
        if ((T->r + 1 == T->c) || (T != lastExecutedTask && lastExecutedTask != nullptr)) {
            cout << "At time " << (timeLapsed - 1) << ", task " << T->id << " has started execution" << endl;
        }

        lastExecutedTask = T;
        if (T->r == 0) {
            if (T->entryTime + T->p >= timeLapsed) {
                cout << "At time " << timeLapsed << ", task " << T->id << " has been completely executed." << endl;
                TheQueue.pop_front();
                return 2;
            } else {
                cout << "Task " << TheQueue.front()->id << " finished at time " << timeLapsed << " thus, missing its deadline of time " << (TheQueue.front()->entryTime + TheQueue.front()->p) << "." << endl;
                TheQueue.pop_front();
                return -1;
            }
        }
        return 99;
    }

    int addNewTask(Task* T) {
        if (TheQueue.empty()) {
            TheQueue.push_front(T);
            T->entryTime = timeLapsed;
            return 0;
        }
        if (T->p == TheQueue.front()->p) {
            auto it = TheQueue.begin();
            advance(it, 1);
            TheQueue.insert(it, T);
            T->entryTime = timeLapsed;
            return 1;
        }
        if (T->p < TheQueue.front()->p) {
            bool tFlag = TheQueue.front()->c == TheQueue.front()->r;
            TheQueue.push_front(T);
            T->entryTime = timeLapsed;
            if (tFlag) return 2;
            else {
                cout << "At time " << timeLapsed << ", task " << TheQueue.front()->id << " has been preempted." << endl;
                return 3;
            }
        }
        if (T->p > TheQueue.front()->p) {
            if (TheQueue.size() == 1) {
                TheQueue.push_back(T);
                T->entryTime = timeLapsed;
                return 4;
            }
            for (auto it = TheQueue.begin(); it != TheQueue.end(); ++it) {
                if (T->p < (*it)->p) {
                    TheQueue.insert(it, T);
                    T->entryTime = timeLapsed;
                    return 5;
                }
                if (T->p > (*it)->p) {
                    if (next(it) == TheQueue.end()) {
                        TheQueue.push_back(T);
                        T->entryTime = timeLapsed;
                        return 6;
                    }
                }
            }
        }
        return 7;
    }
};

int gcd(int a, int b) {
    while (b > 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

int lcm(int a, int b) {
    return a * (b / gcd(a, b));
}

int lcm(vector<int>& input) {
    int result = input[0];
    for (size_t i = 1; i < input.size(); i++) result = lcm(result, input[i]);
    return result;
}

double sigma(const list<Task*>& taskList) {
    double returnValue = 0.0;
    for (const auto& eachTask : taskList) {
        returnValue += static_cast<double>(eachTask->c) / eachTask->p;
    }
    return returnValue;
}

double muSigma(int n) {
    return static_cast<double>(n) * (pow(2.0, (1.0 / n)) - 1.0);
}

int main() {
    list<Task*> PeriodicTaskList;
    ReadyQueue readyQueue;

    try {
        int totalTask;
        cout << "Enter the number of tasks: ";
        cin >> totalTask;

        vector<int> periodList(totalTask);
        cout << "This task set consists of " << totalTask << " tasks. They are:" << endl;

        for (int i = 0; i < totalTask; i++) {
            int period, cTime;
            cout << "Enter period and computation time for task " << i << ": ";
            cin >> period >> cTime;
            periodList[i] = period;
            Task* tempTask = new Task(period, cTime, i);
            PeriodicTaskList.push_back(tempTask);
            cout << "Task " << tempTask->id << ":\n\tPeriod: " << tempTask->p << "\n\tComputation Time: " << tempTask->c << endl;
        }

        bool isFailure = !(sigma(PeriodicTaskList) <= muSigma(PeriodicTaskList.size()));

        if (isFailure) {
            cout << "\n## Note that this task set does not satisfy the schedulability check,\n"
                 << "therefore there are chances of deadline misses." << endl;
        } else {
            cout << "\n## Note that this task set satisfies the schedulability check,\n"
                 << "therefore it is definitely schedulable by RMS" << endl;
        }

        int periodLCM = lcm(periodList);
        cout << "=========================================================\n"
             << "The Execution Begins:\n"
             << "=========================================================" << endl;

        for (int i = 0; ; i++) {
            for (auto individualTask : PeriodicTaskList) {
                if (i % individualTask->p == 0) {
                    readyQueue.addNewTask(new Task(individualTask->p, individualTask->c, individualTask->id));
                }
            }

            if (i != 0 && i % periodLCM == 0) {
                int tempVar = 0;
                if (readyQueue.TheQueue.size() == PeriodicTaskList.size()) {
                    for (auto it = readyQueue.TheQueue.begin(); it != readyQueue.TheQueue.end(); ++it) {
                        if ((*it)->r != (*it)->c) {
                            tempVar = 1;
                            break;
                        }
                    }
                }
                if (tempVar == 0) {
                    cout << "End of one complete cycle" << endl;
                    return 0;
                }
            }

            int output = readyQueue.executeOneUnit();
            if (output == -1) {
                return 0;
            }
        }
    } catch (const exception& e) {
        cout << "Some error occurred. Program will now terminate: " << e.what() << endl;
    }

    return 0;
}
