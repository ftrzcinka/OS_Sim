//Filip Trzcinka

#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <list>
#include <algorithm>

struct MemoryItem
{
    unsigned long long itemAddress;
    unsigned long long itemSize;
    int PID; // PID of the process using this chunk of memory

    unsigned long long endingAddress;
};

using MemoryUsage = std::vector<MemoryItem>;

class SimMemory{
public:
    SimMemory();
    SimMemory(unsigned long long totalRAM);
    bool addItem(unsigned long long item_Size, int item_PID);
    void removeItem(int item_PID);
    unsigned long long findBestFit(unsigned long long item_Size); //returns address of place
    MemoryUsage getMemoryVector();
private:
    std::list<MemoryItem> memory;
    unsigned long long total_RAM;
    unsigned long long available_RAM;
    int number_of_items;
};