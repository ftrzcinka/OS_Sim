//Filip Trzcinka

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>


class SimProcess{
public:
    //Constructor
    SimProcess();
    SimProcess(int priority, unsigned long long size, int PID);
    SimProcess(int priority, unsigned long long size, int PID, int parent);

    //Functions
    int GetPriority() const;
    unsigned long long GetSize() const;
    int GetPID() const;
    int GetParentID() const;
    void setInformation(int priority, int size, int PID);
    void addChild(int pid);
    void removeChild(int pid);
    std::vector<int> getChildList();
    bool childListEmpty();
    void childListUpdate(std::vector<int>& list);
private:
    int process_priority;
    unsigned long long process_size;
    int process_PID;
    std::vector<int> child_list;
    int parent_ID;
};