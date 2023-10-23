//Filip Trzcinka

#include <iostream>
#include <vector>
#include <string>
#include <queue>
#include <algorithm>
#include "SimProcess.h"
#include "SimCPU.h"
#include "SimOS.h"

// SimOS Class Constructor/Function Definitions:
SimOS::SimOS(int numberOfDisks, unsigned long long amountOfRAM)
{
    pid_generator = 0;
    num_of_disks = numberOfDisks;
    total_RAM = amountOfRAM;
    available_RAM = amountOfRAM;
    processor = SimCPU();
    memory_RAM = SimMemory(amountOfRAM);

    FileReadRequest tempFile;
    for (int i = 0; i < numberOfDisks; i++)
    {
        disks.push_back(tempFile);
        std::queue<FileReadRequest> IO_q;
        IO_q_list.push_back(IO_q);
    }
}

void SimOS::addToReadyQ(const SimProcess &process)
{
    ready_queue.push_back(process.GetPID());

    for (int i = 0; i < ready_queue.size() - 1; i++)

        // Last i elements are already
        // in place
        for (int j = 0; j < ready_queue.size() - i - 1; j++)
            if (getProcessFromPID(ready_queue[j]).GetPriority() < getProcessFromPID(ready_queue[j + 1]).GetPriority())
                std::swap(ready_queue[j], ready_queue[j + 1]);
}

int SimOS::getReadyQTop()
{
    if (ready_queue.size() != 0)
    {
        return ready_queue[0];
    }
    else
    {
        return 0;
    }
}

void SimOS::popReadyQ()
{
    if (ready_queue.size() != 0)
    {
        ready_queue.erase(std::remove(ready_queue.begin(), ready_queue.end(), getReadyQTop()), ready_queue.end());
    }
}

SimProcess &SimOS::getProcessFromPID(int pid)
{
    for (int i = 0; i < process_database.size(); i++)
    {
        if (pid == process_database[i].GetPID())
            return process_database[i];
    }
}

bool SimOS::NewProcessFromFork(int priority, unsigned long long size, int ParentID)
{
    if (available_RAM < size)
    {
        return false;
    }
    else if (size < 1)
    {
        return false;
    }
    else
    {
        pid_generator++;

        if (!memory_RAM.addItem(size, pid_generator))
        {
            return false;
        }
        available_RAM = available_RAM - size;
        SimProcess newProcess = SimProcess(priority, size, pid_generator, ParentID);
        process_database.push_back(newProcess);
        if (processor.GetCurrentPID() == 0)
        {
            if (ready_queue.empty())
            {
                processor.RunProcess(newProcess);
            }
            else
            {
                ready_queue.push_back(newProcess.GetPID());
            }
        }
        else
        {
            addToReadyQ(newProcess);

            if (processor.GetCurrentPriority() < getProcessFromPID(getReadyQTop()).GetPriority())
            {
                addToReadyQ(processor.GetCurrentProcess());

                processor.StopRunning();

                processor.RunProcess(getProcessFromPID(getReadyQTop()));
                popReadyQ();
            }
        }
        return true;
    }
}

void SimOS::RemoveFromVector(int pid, std::vector<int> &vec)
{
    vec.erase(std::remove(vec.begin(), vec.end(), pid), vec.end());
}

void SimOS::RemoveFromDatabase(int pid)
{
    
    bool found = false;
    int loc;
    for (int i = 0; i < process_database.size(); i++)
    {
        if (pid == process_database[i].GetPID())
        {
            found = true;
            loc = i;
            break;
        }
    }
    if(found){
        process_database.erase(process_database.begin() + loc);
    }

}

void SimOS::updateDataBase(int pid, std::vector<int> childList)
{
    for (int i = 0; i < process_database.size(); i++)
    {
        if (process_database[i].GetPID() == pid)
        {
            process_database[i].childListUpdate(childList);
        }
    }
}

void SimOS::terminateChildren(SimProcess &parent)
{
    
    if (!parent.childListEmpty())
    {
        
        for (int i = 0; i < parent.getChildList().size(); i++)
        {
            terminateChildren(getProcessFromPID(parent.getChildList()[i]));
        }
    }

    // Remove current process from everywhere

    // Remove from ready Q \/
    if (ready_queue.size() > 0)
    {
        
        bool found = false;
        for (int i = 0; i < ready_queue.size(); i++)
        {
            if (ready_queue[i] == parent.GetPID())
            {
                found = true;
                break;
            }
        }
        if (found)
        {
            RemoveFromVector(parent.GetPID(), ready_queue);
        }
    }

    // Remove from IO qs \/
    std::queue<FileReadRequest> helper;
    for (int i = 0; i < IO_q_list.size(); i++)
    {
        
        if (!IO_q_list[i].empty())
        {
            int size = IO_q_list[i].size();
            for (int j = 0; j < size; j++)
            {
                if (IO_q_list[i].front().PID != parent.GetPID())
                {
                    helper.push(IO_q_list[i].front());
                }
                IO_q_list[i].pop();
            }

            size = helper.size();
            for (int k = 0; k < size; k++)
            {
                IO_q_list[i].push(helper.front());
                helper.pop();
            }
        }
    }

    // Remove from Disks \/
    for (int i = 0; i < disks.size(); i++)
    {
        if (disks[i].PID == parent.GetPID())
        {

            if (IO_q_list[i].empty())
            {
                FileReadRequest tempFile;
                disks[i] = tempFile;
            }
            else
            {
                disks[i] = IO_q_list[i].front();
                IO_q_list[i].pop();
            }
        }
    }

    // Remove from waiting
    if (waiting.size() != 0)
    {
        bool found = false;

        for (int i = 0; i < waiting.size(); i++)
        {
            if (waiting[i] == parent.GetPID())
                found = true;
        }

        if (found)
        {
            RemoveFromVector(parent.GetPID(), waiting);
        }
    }

    // remove from memory
    memory_RAM.removeItem(parent.GetPID());
    available_RAM += parent.GetSize();
    // Remove from database
    RemoveFromDatabase(parent.GetPID());

    return;
}



// Req functions

bool SimOS::NewProcess(int priority, unsigned long long size)
{
    if (available_RAM < size)
    {
        return false;
    }
    else if (size < 1)
    {
        return false;
    }
    else
    {
        pid_generator++;

        if (!memory_RAM.addItem(size, pid_generator))
        {
            return false;
        }
        available_RAM = available_RAM - size;
        SimProcess newProcess = SimProcess(priority, size, pid_generator);
        process_database.push_back(newProcess);

        if (processor.GetCurrentPID() == 0)
        {
            if (ready_queue.empty())
            {
                processor.RunProcess(newProcess);
            }
            else
            {
                ready_queue.push_back(newProcess.GetPID());
            }
        }
        else
        {
            addToReadyQ(newProcess);

            if (processor.GetCurrentPriority() < getProcessFromPID(getReadyQTop()).GetPriority())
            {
                addToReadyQ(processor.GetCurrentProcess());

                processor.StopRunning();

                processor.RunProcess(getProcessFromPID(getReadyQTop()));
                popReadyQ();
            }
        }
        return true;
    }
}

bool SimOS::SimFork()
{
    if (processor.GetCurrentPID() == 0)
    {
        return false;
    }
    else if ((available_RAM - processor.GetCurrentProcess().GetSize()) < 0)
    {
        return false;
    }
    else
    {
        processor.GetCurrentProcess().addChild(pid_generator + 1);
        updateDataBase(processor.GetCurrentProcess().GetPID(), processor.GetCurrentProcess().getChildList());
        return NewProcessFromFork(processor.GetCurrentProcess().GetPriority(), processor.GetCurrentProcess().GetSize(), processor.GetCurrentPID());
    }
}

void SimOS::SimExit()
{
    
    if (processor.GetCurrentPID() == 0)
    {
        return;
    }

    int pid_kill = processor.GetCurrentPID();

    if(processor.GetCurrentProcess().childListEmpty()){
        memory_RAM.removeItem(pid_kill);
        available_RAM += processor.GetCurrentProcess().GetSize();
    }

    if (!processor.GetCurrentProcess().childListEmpty())
    {
        terminateChildren(processor.GetCurrentProcess());
    }

    // If parent is waiting, move back to ready Q
    if (processor.GetCurrentParentID() != 0)
    {
        // see if child becomes zombie
        bool parentWaiting = false;
        for (int i = 0; i < waiting.size(); i++)
        {
            if (processor.GetCurrentParentID() == waiting[i])
            {
                parentWaiting = true;
                break;
            }
        }
        if (!parentWaiting)
        {
            zombies.push_back(processor.GetCurrentPID());
            processor.StopRunning();
            if (ready_queue.size() != 0)
            {
                processor.RunProcess(getProcessFromPID(getReadyQTop()));
                popReadyQ();
            }
            RemoveFromDatabase(pid_kill);
            return;
        }
        // child does not become zombie
        SimProcess temp = getProcessFromPID(processor.GetCurrentParentID());
        temp.removeChild(processor.GetCurrentPID());
        updateDataBase(processor.GetCurrentParentID(), temp.getChildList()); // remove child from parent's list

        if (waiting.size() != 0)
        {

            bool found = false;

            for (int i = 0; i < waiting.size(); i++)
            {
                if (waiting[i] == processor.GetCurrentParentID())
                {
                    found = true;
                    break;
                }
            }

            if (found)
            {
                addToReadyQ(getProcessFromPID(processor.GetCurrentParentID()));

                waiting.erase(std::remove(waiting.begin(), waiting.end(), processor.GetCurrentParentID()), waiting.end());
            }
        }
    }

    processor.StopRunning();
    if (ready_queue.size() != 0)
    {
        processor.RunProcess(getProcessFromPID(getReadyQTop()));
        popReadyQ();
    }

    RemoveFromDatabase(pid_kill);
}

void SimOS::SimWait()
{
    if (processor.GetCurrentProcess().childListEmpty())
    {
        return;
    }

    bool zombieFound = false;
    int zomb_pid = 0;

    // If zombie children exist
    if (!zombies.empty())
    {
        for (int i = 0; i < processor.GetCurrentProcess().getChildList().size(); i++)
        {
            for (int j = 0; j < zombies.size(); j++)
            {
                if (processor.GetCurrentProcess().getChildList()[i] == zombies[j])
                {
                    zombieFound = true;
                    zomb_pid = processor.GetCurrentProcess().getChildList()[i];
                    break;
                }
            }

            if (zombieFound)
            {
                break;
            }
        }

        processor.GetCurrentProcess().removeChild(zomb_pid);
        RemoveFromVector(zomb_pid, zombies);
        updateDataBase(processor.GetCurrentPID(), processor.GetCurrentProcess().getChildList());
        return;
    }

    waiting.push_back(processor.GetCurrentProcess().GetPID());
    processor.StopRunning();
    if (ready_queue.size() != 0)
    {
        processor.RunProcess(getProcessFromPID(getReadyQTop()));
        popReadyQ();
    }
}

void SimOS::DiskReadRequest(int diskNumber, const std::string &fileName)
{
    if ((diskNumber < 0) || (diskNumber >= num_of_disks))
    {
        return;
    }

    if (processor.GetCurrentPID() == 0)
    {
        return;
    }

    FileReadRequest new_request;
    new_request.PID = processor.GetCurrentPID();
    new_request.fileName = fileName;

    if (disks[diskNumber].PID == 0)
    {
        disks[diskNumber] = new_request;
    }
    else
    {
        IO_q_list[diskNumber].push(new_request);
    }

    processor.StopRunning();
    if (ready_queue.size() != 0)
    {
        processor.RunProcess(getProcessFromPID(getReadyQTop()));
        popReadyQ();
    }
}

void SimOS::DiskJobCompleted(int diskNumber)
{
    if ((diskNumber < 0) || (diskNumber >= num_of_disks))
    {
        return;
    }

    if (disks[diskNumber].PID == 0)
    {
        return;
    }

    if (processor.GetCurrentPID() == 0)
    {
        processor.RunProcess(getProcessFromPID(disks[diskNumber].PID));
    }
    else if (processor.GetCurrentPriority() < getProcessFromPID(disks[diskNumber].PID).GetPriority())
    {
        addToReadyQ(processor.GetCurrentProcess());
        processor.StopRunning();
        processor.RunProcess(getProcessFromPID(disks[diskNumber].PID));
    }
    else
    {
        addToReadyQ(getProcessFromPID(disks[diskNumber].PID));
    }

    if (IO_q_list[diskNumber].empty())
    {
        FileReadRequest tempFile;
        disks[diskNumber] = tempFile;
    }
    else
    {
        disks[diskNumber] = IO_q_list[diskNumber].front();
        IO_q_list[diskNumber].pop();
    }

    return;
}

int SimOS::GetCPU() const
{
    return processor.GetCurrentPID();
}

std::vector<int> SimOS::GetReadyQueue() const
{
    return ready_queue;
}

FileReadRequest SimOS::GetDisk(int diskNumber)
{
    return disks[diskNumber];
}

std::queue<FileReadRequest> SimOS::GetDiskQueue(int diskNumber) const
{
    return IO_q_list[diskNumber];
}

MemoryUsage SimOS::GetMemory()
{
    MemoryUsage temp = memory_RAM.getMemoryVector();
    return temp;
}