//
// Created by root on 21/11/22.
//

#ifndef FINALPROJECT_HASHTABLE_H
#define FINALPROJECT_HASHTABLE_H

#include <string>
#include <utility>
#include <vector>
#include "NameIndex.h"


class hashtable {
public:
    hashtable()
    {
        size = 0;
        longestProbe = 0;
        capacity = 1024;
        array = std::vector<NameIndex>(capacity);
    }

    void makeEmpty() {
        size = 0;
        for (auto &entry: array)
            entry.isActive = false;
    }

    int getLongestProbe() const
    {
        return longestProbe;
    }

    bool insert(NameIndex x)
    {
        float load = (float)size/(float)capacity;
        if (load > load)
            reSize();
        int base = abs(x.hash() % capacity);
        int count = 0;
        while (true)
        {
            int pos = (base + (count * count + count)/2) % capacity;
            if(!array[pos].isActive)
            {
                array[pos] = x;
                size++;
                if (count > longestProbe)
                    longestProbe = count;
                return true;
            }
            count++;
        }

        return false;
    }

    // Method that search all hash function places
    int findPos(const std::string& f_name, const std::string& state_abbrev) const {
        NameIndex tmp = NameIndex();
        tmp.state_abbrev = state_abbrev;
        tmp.feature_name = f_name;
        int base = abs(tmp.hash() % capacity);
        int count = 0;
        while (true) {
            int pos = (base + (count * count + count) / 2) % capacity;
            if (isActive(pos) && (array[pos].feature_name == f_name && array[pos].state_abbrev == state_abbrev))
                return array[pos].offset;
            if (count > capacity)
                return -1;
            count++;
        }

        return -1;
    }

    int getSize() const
    {
        return size;
    }

    void reSize()
    {
        reHash(capacity*2);
    }

    void reHash() {
        reHash(capacity);
    }

    void reHash(int newSize)
    {
        std::vector<NameIndex> old = array;
        array.resize(newSize);
        for (auto  &entry: array)
            entry.isActive = false;

        size = 0;
        for(auto &entry: old)
            if (entry.isActive)
                insert(std::move(entry));
    }

    std::string toString()
    {
        std::string buff = "Format of display is\n";
        buff.append("Slot number: data record\n");
        buff.append("Current table size is "+std::to_string(capacity) +"\n");
        buff.append("Number of elements in table is "+std::to_string(size) +"\n");
        buff.append("\n");
        for (int i = 0; i < capacity; ++i) {
            if (array[i].isActive) {
                buff.append(std::to_string(i)+": ");
                buff.append(array[i].toString());
                buff.append("\n");
            }
        }
        return buff;
    }

private:

    std::vector<NameIndex> array;
    int capacity;
    int size;
    int longestProbe;
    constexpr static const double LOAD = 0.7;

    bool isActive(int currentPos) const { return currentPos != -1 && array[currentPos].isActive; }
};




#endif //FINALPROJECT_HASHTABLE_H
