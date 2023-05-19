//
// Created by root on 21/11/22.
//

#ifndef FINALPROJECT_NAMEINDEX_H
#define FINALPROJECT_NAMEINDEX_H


#include <string>
#include <utility>
#include <vector>

class NameIndex {
public:
    std::string feature_name;
    std::string state_abbrev;
    int offset;
    bool isActive;

    explicit NameIndex(std::string  f_name, std::string  s_abbrev, bool a , int o):
    feature_name(std::move(f_name)), state_abbrev(std::move(s_abbrev)),
    isActive(a), offset(o) {}

    NameIndex() {
        feature_name = "";
        state_abbrev = "";
        offset = -1;
        isActive = false;
    }


    int hash()
    {
        std::string concat = feature_name+state_abbrev;
        int value = 0;
        int  x = 0;
        int len = concat.length();
        for(int i  = 0; i < len; i++)
        {
            value = (value << 4) + (concat[i]);
            if((x = value & 0xF0000000) != 0)
            {
                value ^= (x >> 24);
            }
            value &= ~x;
        }
        return value;
    }

    std::string toString(){
        std::string buff = "[" + feature_name + ":"+state_abbrev+", [" +std::to_string(offset)+"]]";
        return buff;
    }
private:

};


#endif //FINALPROJECT_NAMEINDEX_H
