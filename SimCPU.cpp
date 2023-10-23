//Filip Trzcinka

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include "SimProcess.h"
#include "SimCPU.h"

//SimCPU constructor/fucntion def:
SimCPU::SimCPU(){
    process_running = false;
}

void SimCPU::RunProcess(const SimProcess& process){
    current_running_process = process;
    process_running = true;
}

void SimCPU::StopRunning(){
    process_running = false;
    current_running_process = SimProcess();
}

int SimCPU::GetCurrentPID() const{
    if(process_running){
        return current_running_process.GetPID();
    }else{
        return 0;
    }
}

int SimCPU::GetCurrentPriority() const {
    if(process_running){
        return current_running_process.GetPriority();
    }else{
        return 0;
    }
}

int SimCPU::GetCurrentParentID() const {
    if(process_running){
        return current_running_process.GetParentID();
    }else{
        return 0;
    }
}

SimProcess& SimCPU::GetCurrentProcess(){
    return current_running_process;
}