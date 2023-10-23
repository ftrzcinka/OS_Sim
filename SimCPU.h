//Filip Trzcinka

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include "SimProcess.h"

class SimCPU{
public:
    //Constructor
    SimCPU();

    //Functions
    void RunProcess(const SimProcess& process);
    void StopRunning();
    int GetCurrentPID() const;
    int GetCurrentPriority() const;
    int GetCurrentParentID() const;
    SimProcess& GetCurrentProcess();
private:
    SimProcess current_running_process;
    bool process_running;
};