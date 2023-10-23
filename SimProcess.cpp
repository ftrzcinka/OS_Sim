//Filip Trzcinka

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include "SimProcess.h"

//SimProcess Class Constructor/Function Definitions:
SimProcess::SimProcess(){
    process_priority = 0;
    process_size = 0;
    process_PID = 0;
    parent_ID = 0;
}

SimProcess::SimProcess(int priority, unsigned long long size, int PID){
    process_priority = priority;
    process_size = size;
    process_PID = PID;
    parent_ID = 0;
}

SimProcess::SimProcess(int priority, unsigned long long size, int PID, int parent){
    process_priority = priority;
    process_size = size;
    process_PID = PID;
    parent_ID = parent;
}

int SimProcess::GetPriority() const{
    return process_priority;
}

unsigned long long SimProcess::GetSize() const{
    return process_size;
}

int SimProcess::GetPID() const{
    return process_PID;
}

int SimProcess::GetParentID() const{
    return parent_ID;
}

void SimProcess::setInformation(int priority, int size, int PID){
    process_priority = priority;
    process_size = size;
    process_PID = PID;
}

void SimProcess::addChild(int pid){
    child_list.push_back(pid);
}

void SimProcess::removeChild(int pid){
    child_list.erase(std::remove(child_list.begin(), child_list.end(), pid), child_list.end());
}

std::vector<int> SimProcess::getChildList(){
    return child_list;
}

bool SimProcess::childListEmpty(){
    if(child_list.size() == 0)
        return true;
    else
        return false;
}

void SimProcess::childListUpdate(std::vector<int>& list){
    child_list.clear();
    for(int i = 0; i < list.size(); i++){
        child_list.push_back(list[i]);
    }
}