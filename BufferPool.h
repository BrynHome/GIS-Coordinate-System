//
// Created by root on 30/11/22.
//

#ifndef FINALPROJECT_BUFFERPOOL_H
#define FINALPROJECT_BUFFERPOOL_H

#include <string>
#include <list>
#include <unordered_map>
#include "GISRecord.h"

struct DMS_POS {
    int longitude;
    int latitude;

    DMS_POS(int longitude, int latitude) : longitude(longitude), latitude(latitude) {}
};

class FailedPOS: std::exception{

};

DMS_POS dmsPosition(const std::string& longitude, const std::string& latitude)
{
    try {
        int startLong;
        if (longitude.length() == 8)
            startLong = 3;
        else
            startLong = 2;
        int a = std::stoi(longitude.substr(0, startLong)) * 3600;
        int b = std::stoi(longitude.substr(startLong, 2)) * 60;
        int c = std::stoi(longitude.substr(startLong + 2, 2));
        int lon = a
                  + b
                  + c;
        int d =std::stoi(latitude.substr(0, 2)) * 3600;
        int e = std::stoi(latitude.substr(2, 2)) * 60;
        int f = std::stoi(latitude.substr(4, 2));
        int lat = d
                  + e
                  + f;
        if (longitude[startLong + 4] == 'W')
            lon *= -1;
        if (latitude[6] == 'S')
            lat *= -1;

        return {lon, lat};
    }
    catch (std::exception e)
    {
        throw FailedPOS();
    }
}

std::vector<int> defWorld(const std::string& l, const std::string& r, const std::string& b, const std::string& t)
{
    try {
        DMS_POS rt = dmsPosition(r, t);
        DMS_POS lb = dmsPosition(l, b);
        std::vector<int> buff;
        buff.push_back(lb.longitude);
        buff.push_back(rt.longitude);
        buff.push_back(lb.latitude);
        buff.push_back(rt.latitude);
        return buff;
    }
    catch(std::exception e) {
        exit(-1);
    }
}

class BufferPool{
private:
    int size{};
    std::list<GISRecord> pool;

public:
    BufferPool(int n = 15)
    {
        size = n;
    }

    int getSize() const {
        return size;
    }

    void setSize(int s) {
        BufferPool::size = s;
    }

    bool insert(const GISRecord& g)
    {
        std::list<GISRecord> tmp = pool;
        for(auto &i : tmp)
        {
            if(i == g) {
                pool.remove(i);
                pool.push_front(i);
                return true;
            }
        }
        pool.push_front(g);
        if (pool.size() > size)
        {
            pool.pop_back();
        }
        return true;
    }

    std::vector<GISRecord> findFeature(const std::string& f_name, const std::string& state)
    {
        std::vector<GISRecord> buff;
        std::list<GISRecord> tmp = pool;
        for(auto &i : tmp)
        {
            if(i.featureEqual(f_name, state)) {
                buff.push_back(i);
                pool.remove(i);
                pool.push_front(i);
            }
        }
        return buff;
    }

    std::vector<GISRecord> findOff(int off)
    {
        std::vector<GISRecord> buff;
        std::list<GISRecord> tmp = pool;
        for(auto &i : tmp)
        {
            if(i.getOffSet() == off) {
                buff.push_back(i);
                pool.remove(i);
                pool.push_front(i);
            }
        }
        return buff;
    }

    std::vector<GISRecord> findCoordWithOpt(int latitude, int longitude, const std::string& o)
    {
        std::vector<GISRecord> buff;
        std::list<GISRecord> tmp = pool;
        for(auto &i : tmp)
        {
            if(i.getCoord().equals(coord(longitude, latitude)) && single(i, o)) {
                buff.push_back(i);
                pool.remove(i);
                pool.push_front(i);
            }
        }
        return buff;
    }

    std::vector<GISRecord> findCoord(int latitude, int longitude)
    {
        std::vector<GISRecord> buff;
        std::list<GISRecord> tmp = pool;
        for(auto &i : tmp)
        {
            if(i.getCoord().equals(coord(longitude, latitude))) {
                buff.push_back(i);
                pool.remove(i);
                pool.push_front(i);
            }
        }
        return buff;
    }

    std::vector<GISRecord> inWorld(int xl, int xh, int yl, int yh)
    {
        std::vector<GISRecord> buff;
        for(auto &i : pool)
        {
            if(i.getCoord().inArea(xl,xh,yl,yh)) {
                buff.push_back(i);
                pool.remove(i);
                pool.push_front(i);
            }
        }
        return buff;
    }




    std::string toString()
    {
        std::string buff;
        buff.append("MRU\n");
        for(auto &i : pool)
        {
            buff.append(i.toStringRaw());
        }
        buff.append("LRU\n");
        return buff;
    }

private:
    static bool single(const GISRecord& g, const std::string& filter)
    {
        std::vector<GISRecord> filtered;
        if (filter == "pop")
        {
                if(g.getFeatureClass() == "Populated Place")
                {
                    return true;
                }

        }
        else if (filter == "water")
        {

                const std::string& a = g.getFeatureClass();
                if(a == "Arroyo"
                   || a == "Bay"
                   || a == "Basin"
                   || a == "Bend"
                   || a == "Canal"
                   || a == "Channel"
                   || a == "Falls"
                   || a == "Glacier"
                   || a == "Gut"
                   || a == "Harbour"
                   || a == "Lake"
                   || a == "Rapids"
                   || a == "Reservoir"
                   || a == "Sea"
                   || a == "Spring"
                   || a == "Stream"
                   || a == "Swamp"
                   || a == "Well")
                {
                    return true;
                }

        }
        else if (filter == "structure")
        {

                const std::string& a = g.getFeatureClass();
                if(a == "Airport"
                   || a == "Bridge"
                   || a == "Building"
                   || a == "Church"
                   || a == "Dam"
                   || a == "Hospital"
                   || a == "Levee"
                   || a == "Park"
                   || a == "Post Office"
                   || a == "School"
                   || a == "Tower"
                   || a == "Tunnel")
                {
                    return true;
                }

        }
        return false;
    }



};


#endif //FINALPROJECT_BUFFERPOOL_H
