//Filip Trzcinka

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include "SimCPU.h"
#include "SimMemory.h"

//Define a structure:
struct FileReadRequest
{
    int  PID{0};
    std::string fileName{""};
};
 
using MemoryUsage = std::vector<MemoryItem>;

class SimOS{
public:
    //Constructor
    SimOS(int numberOfDisks, unsigned long long amountOfRAM);

    //My Functions
    void addToReadyQ(const SimProcess& process);
    int getReadyQTop();
    void popReadyQ();
    SimProcess& getProcessFromPID(int pid);
    bool NewProcessFromFork(int priority, unsigned long long size, int ParentID);
    void RemoveFromVector(int pid, std::vector<int>& vec);
    void RemoveFromDatabase(int pid);
    void updateDataBase(int pid, std::vector<int> childList);
    void terminateChildren(SimProcess& parent);
    
    //Req Functions
    bool NewProcess(int priority, unsigned long long size);
    bool SimFork(); //This works
    void SimExit();                 //BIG PROBLEM!!!!!!
    void SimWait(); //This Works
    void DiskReadRequest(int diskNumber, const std::string& fileName); //This works
    void DiskJobCompleted(int diskNumber); //This works
    int GetCPU() const; //This works
    std::vector<int> GetReadyQueue() const; //This works
    MemoryUsage GetMemory();                            //Not even implemented
    FileReadRequest GetDisk(int diskNumber); //This works
    std::queue<FileReadRequest> GetDiskQueue(int diskNumber) const; //This works

private:
    int pid_generator;
    int num_of_disks;
    unsigned long long total_RAM;
    unsigned long long available_RAM;
    std::vector<int> ready_queue;
    std::vector<std::queue<FileReadRequest>> IO_q_list;
    SimCPU processor;
    std::vector<int> waiting;
    std::vector<int> zombies;
    std::vector<FileReadRequest> disks;
    SimMemory memory_RAM;

    std::vector<SimProcess> process_database;
};