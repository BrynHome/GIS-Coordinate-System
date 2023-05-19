//
// Created by root on 24/11/22.
//

#ifndef FINALPROJECT_QUADTREE_H
#define FINALPROJECT_QUADTREE_H

#include <vector>
#include <typeinfo>
#include "node.h"

// Start with one node(index) with region size of the world.
// store up to k data objects in the node.
// Split node into 4 more leafs that split the region size. insert the values into their corresponding regions
class quadtree {
private:
    long x_low, x_high, y_low, y_high;
    int k;
    int size = 0;
    node root;
public:
    explicit quadtree(long xl, long xh, long yl, long yh ,int k_size = 4) {
        k = k_size;
        x_low = xl;
        x_high = xh;
        y_low = yl;
        y_high = yh;
        root = node();
        root.setWorld(xl, xh, yl, yh);
    }

    quadtree() {
        k = 4;
        x_low = 0;
        x_high = 0;
        y_low = 0;
        y_high = 0;
        root = node();
    }

    void setWorld(long xl, long xh, long yl, long yh)
    {
        x_low = xl;
        x_high = xh;
        y_low = yl;
        y_high = yh;
        root.setWorld(xl,xh,yl,yh);
    }

    int getSize() const
    {
        return size;
    }

    bool insert(const coord& c)
    {
        if (c.inArea(x_low, x_high, y_low, y_high)) {
            insert(&root, c);
            size++;
            return true;
        }
        return false;

    }

    std::string toString()
    {
        std::string out = " ";
        std::string pre = " ";
        toString(root, &out, &pre, "r");
        return out;
    }
private:
    void toString(node n, std::string * out
                  , std::string * pre, const std::string& fix)
    {
        if (n.isLeaf)
        {
            out->append(*pre);
            out->append(n.dataRawToString()
            + "\n");
        }
        else
        {
            out->append(*pre + fix + "\n");
            if(*fix.begin() == '+')
                pre->append("\t");
            else if(*fix.begin() == 'L')
                pre->append("\t");
            toString(*n.ne, out, pre, "+ NE");
            toString(*n.nw, out, pre, "+ NW");
            toString(*n.se, out, pre, "+ SE");
            toString(*n.sw, out, pre, "L SW");
            if (pre->length() > 0)
                *pre = pre->substr(0, pre->length() -2);
        }

    }
    void insert(node * n, const coord& c)
    {
        if (n->isEmpty) {
            n->insert(c);
            return ;
        }
        std::vector<coord> buff = std::vector<coord>();
        buff.push_back(c);
        if(n->isLeaf)
        {
            for(auto & i : n->data)
            {
                if (i.equals(c))
                {
                    i.increaseOffset(c.getOffset()[0]);
                    return;
                }

            }
            if (n->data.size() < k)
            {
                n->insert(c);
                return ;
            }
            buff.insert(buff.end(), n->data.begin(), n->data.end());

            n->makeIntern();
            long x = (n->xl+n->xh)/2;
            long y = (n->yl+n->yh)/2;
            n->ne->setWorld(x, n->xh, y, n->yh);
            n->nw->setWorld(n->xl, x, y, n->yh);
            n->se->setWorld(x, n->xh, n->yl, y);
            n->sw->setWorld(n->xl, x, n->yl, y);

        }

        for (auto &e : buff) {

            if(e.inArea(n->ne->xl, n->ne->xh, n->ne->yl, n->ne->yh))
                insert(n->ne, e);
            if(e.inArea(n->nw->xl, n->nw->xh, n->nw->yl, n->nw->yh))
                insert(n->nw, e);
            if(e.inArea(n->se->xl, n->se->xh, n->se->yl, n->se->yh))
                insert(n->se, e);
            if(e.inArea(n->sw->xl, n->sw->xh, n->sw->yl, n->sw->yh))
                insert(n->sw, e);

        }
    }
    coord * find(node * n, const coord& c)
    {
        if (n->isEmpty)
            return nullptr;

        if (n->isLeaf) {
            for(auto & i : n->data)
            {
                if (i.equals(c))
                    return &i;
            }
        } else {
            if(c.inArea(n->ne->xl, n->ne->xh, n->ne->yl, n->ne->yh))
                return find(n->ne, c);
            if(c.inArea(n->nw->xl, n->nw->xh, n->nw->yl, n->nw->yh))
                return find(n->nw, c);
            if(c.inArea(n->se->xl, n->se->xh, n->se->yl, n->se->yh))
                return find(n->se, c);
            if(c.inArea(n->sw->xl, n->sw->xh, n->sw->yl, n->sw->yh))
                return find(n->sw, c);
        }
        return nullptr;

    }
    void find(node * n, std::vector<coord> * c
            , long xlr, long xhr, long ylr, long yhr)
    {
        if (n->isEmpty)
            return;

        if (n->isLeaf) {
            for(auto & i : n->data)
            {
                if (i.inArea(xlr, xhr, ylr, yhr))
                    c->push_back(i);
            }
        } else {
            if (n->ne->xl <= xhr || n->ne->xh >= xlr || n->ne->yl <= yhr || n->ne->yh >= ylr)
                find(n->ne, c
                        ,xlr, xhr, ylr, yhr);
            if (n->nw->xl <= xhr || n->nw->xh >= xlr || n->nw->yl <= yhr || n->nw->yh >= ylr)
                find(n->nw, c
                        ,xlr, xhr, ylr, yhr);
            if (n->se->xl <= xhr || n->se->xl >= xlr || n->se->yl <= yhr || n->se->yh >= ylr)
                find(n->se, c
                        ,xlr, xhr, ylr, yhr);
            if (n->sw->xl <= xhr || n->sw->xl >= xlr || n->sw->yl <= yhr || n->sw->yh >= ylr)
                find(n->sw, c
                        ,xlr, xhr, ylr, yhr);
        }
    }
public:
    std::vector<coord> find(long xl, long xh, long yl, long yh)
    {
        std::vector<coord> r = std::vector<coord>();
        find(&root, &r, xl, xh, yl, yh);
        return r;
    }

    coord * find(coord c)
    {
        return find(&root, c);
    }


};




#endif //FINALPROJECT_QUADTREE_H
