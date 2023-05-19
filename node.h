//
// Created by root on 29/11/22.
//

#ifndef FINALPROJECT_NODE_H
#define FINALPROJECT_NODE_H


#include <vector>
#include <list>
#include "coord.h"



class node {
public:
    std::vector<coord> data;
    bool isEmpty{};
    bool isLeaf;
    long xl =0, xh = 0, yl = 0, yh = 0;
    node * nw;
    node * ne;
    node * se;
    node * sw;
    node(){
        isEmpty = true;
        isLeaf = true;
    }
    explicit node(const coord& c)
    {
        isEmpty = false;
        data.push_back(c);
        isLeaf = true;
    }

    void setWorld(long xll, long xhh, long yll, long yhh)
    {
        xl = xll;
        xh = xhh;
        yl = yll;
        yh = yhh;
    }

    void makeIntern()
    {
        if (isLeaf)
        {
            isEmpty = false;
            isLeaf = false;
            nw = new node();
            ne = new node();
            se = new node();
            sw = new node();
        }
    }

    void insert(const coord& c){
        isEmpty = false;
        data.push_back(c);
    }

    std::string dataToString()
    {

        if (isLeaf)
        {
            if (isEmpty)
                return "*\n";
            std::string buff;
            for(auto &i : data)
            {
                buff.append(i.toString() + " ");
            }
            return buff;
        } else
            return "";
    }

    std::string dataRawToString()
    {

        if (isLeaf)
        {
            if (isEmpty)
                return "*\n";
            std::string buff;
            for(auto &i : data)
            {
                buff.append(i.rawString() + " ");
            }
            return buff;
        } else
            return "";
    }


};

struct intern {
    node nw;
    node ne;
    node se;
    node sw;
};



#endif //FINALPROJECT_NODE_H
