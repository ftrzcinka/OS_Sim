//Filip Trzcinka

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <list>
#include <algorithm>
#include <limits.h>
#include "SimMemory.h"


SimMemory::SimMemory(){
    total_RAM = 0;
    available_RAM = 0;
    number_of_items = 0;
}

SimMemory::SimMemory(unsigned long long totalRAM){
    total_RAM = totalRAM;
    available_RAM = totalRAM;
    number_of_items = 0;

    MemoryItem frontBorder;
    frontBorder.itemAddress = 0;
    frontBorder.itemSize = 0;
    frontBorder.PID = -1;
    frontBorder.endingAddress = 0;

    memory.push_front(frontBorder);

    MemoryItem endBorder;
    endBorder.itemAddress = total_RAM;
    endBorder.itemSize = 0;
    endBorder.PID = -1;
    endBorder.endingAddress = total_RAM;

    memory.push_back(endBorder);
}

bool SimMemory::addItem(unsigned long long item_Size, int item_PID){
    if(item_Size > available_RAM){
        return false;
    }else{

        MemoryItem item;
        item.itemSize = item_Size;
        item.PID = item_PID;

        std::list<MemoryItem>::iterator it = memory.begin();

        if(number_of_items == 0){
            
            item.itemAddress = 0;
            item.endingAddress = item_Size;

            std::advance(it, 1);

            memory.insert(it, item);

            number_of_items++;
            available_RAM = available_RAM - item_Size;

            return true;
        }else{
            unsigned long long address = findBestFit(item_Size);

            if(address == total_RAM+1){
                return false;
            }
            
            if(address == 0){
                std::advance(it, 1);
                item.itemAddress = address;
                item.endingAddress = item.itemAddress + item.itemSize;
                number_of_items++;

                memory.insert(it, item);
                return true;
            }

            item.itemAddress = address;
            for(int i = 0; i < number_of_items+1; i++){ //We do +1 since there are border nodes
                std::advance(it, 1);
                MemoryItem temp = *it;
                if(temp.endingAddress == address){
                    break;
                }
            }

            item.itemAddress = address;
            item.endingAddress = item.itemAddress + item.itemSize;

            std::advance(it, 1);

            memory.insert(it, item);

            number_of_items++;
            available_RAM = available_RAM - item_Size;

            return true;
        }
    }
}


void SimMemory::removeItem(int item_PID){
    
    if(number_of_items == 0){
        return;
    }else{
        std::list<MemoryItem>::iterator it = memory.begin();
        MemoryItem temp;

        bool found = false;

        for(int i = 0; i < number_of_items+1; i++){
            temp = *it;
            if(temp.PID == item_PID){
                found = true;
                break;
            }
            std::advance(it, 1);
        }

        if(!found){
            return;
        }

        if(temp.PID == -1){
            return;
        }

        available_RAM = available_RAM + temp.itemSize;
        memory.erase(it);
        number_of_items--;
        return;

    }
}


unsigned long long SimMemory::findBestFit(unsigned long long item_Size){
    if(item_Size > available_RAM){
        return total_RAM+1;
    }else{
        unsigned long long bestAddress = 0;
        unsigned long long  sizeDifference = ULLONG_MAX;
        unsigned long long memHole;
        signed long long difference;

        std::list<MemoryItem>::iterator it = memory.begin();

        for(int i = 0; i < number_of_items+1; i++){
            MemoryItem current = *it;
            std::advance(it, 1);
            MemoryItem next = *it;
            memHole = next.itemAddress - current.endingAddress;
            if(memHole == 0){
                continue;
            }
            difference = memHole - item_Size;
            if(difference < 0){
                continue;
            }
            if(sizeDifference > difference){
                sizeDifference = difference;
                bestAddress = current.endingAddress;
            }
        }

        if(difference < 0){
            return total_RAM+1;
        }

        if((bestAddress + item_Size) > total_RAM){
            return total_RAM+1;
        }

        return bestAddress;
    }
}

std::vector<MemoryItem> SimMemory::getMemoryVector(){
    std::vector<MemoryItem> vec;
    std::list<MemoryItem>::iterator it = memory.begin();
    std::advance(it, 1);
    for(int i = 0; i < number_of_items; i++){
        vec.push_back(*it);
        std::advance(it, 1);
    }
    return vec;
}