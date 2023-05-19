//
// Created by root on 29/11/22.
//

#ifndef FINALPROJECT_COORD_H
#define FINALPROJECT_COORD_H


#include <string>
#include <vector>

class coord {
private:
    std::vector<int> offsets;
    int longitude;
    int latitude;

public:
    coord(int longitude, int latitude, int offset = -1)
    : longitude(longitude), latitude(latitude) {
        if (offset != -1)
            offsets.push_back(offset);
    }

    int getLong() const {
        return longitude;
    }

    int getLat() const {
        return latitude;
    }
    std::vector<int> getOffset() const {
        return offsets;
    }

    void increaseOffset(int off)  {
        offsets.push_back(off);
    }

    int dirFrom(long x, long y) const
    {
        if (longitude == x && latitude == y) {
            return 4;
        }
        if (longitude > x){
            if (latitude > y)
                return 0;
            else
                return 1;
        } else
        {
            if (latitude > y)
                return 2;
            else
                return 3;
        }
    }

    bool inArea(long xl, long xh, long yl, long yh) const
    {
        return (longitude > xl && longitude < xh
                && latitude > yl && latitude < yh);
    }

    std::string stringLat() const{
        int aLat = abs(latitude);
        return std::to_string(aLat / 3600) + "d " + std::to_string((aLat / 60) % 60) + "m "
               + std::to_string(aLat % 60) + "s "  + (latitude > 0 ? "North" : "South");
    }

    std::string stringLong() const{
        int aLong = abs(longitude);
        return std::to_string(aLong / 3600) + "d " + std::to_string((aLong / 60) % 60) + "m "
               + std::to_string(aLong % 60) + "s " + (longitude > 0 ? "Ease" : "West");
    }

    std::string stringCoord() const{
        return "(" + stringLat() + ", " + stringLong() + ")";
    }

    std::string toString() const {
        if(!offsets.empty())
        {
            std::string buff = "[" + stringCoord();
            for(auto &i : offsets)
            {
                buff.append(std::to_string(i) + "]");
            }
            return buff;
        } else {
            return "[" + stringCoord() + "]";
        }

    }

    std::string rawString() const {
        if(!offsets.empty())
        {
            std::string buff = "[(" + std::to_string(latitude) + ", " + std::to_string(longitude) + ")";
            for(auto &i : offsets)
            {
                buff.append(std::to_string(i) + "]");
            }
            return buff;
        } else {
            return "[" + stringCoord() + "]";
        }

    }


    bool equals(coord c) const
    {
        return (dirFrom(c.getLong(),c.getLat()) == 4);
    }
};




#endif //FINALPROJECT_COORD_H
