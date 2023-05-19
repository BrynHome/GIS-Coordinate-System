#include <iostream>
#include <fstream>
#include "BufferPool.h"
#include "hashtable.h"
#include "quadtree.h"

#define K_VALUE 4



class Logger{
    public:
    int commandCount = 0;
    explicit Logger(char *logfile) : logfile(logfile) {}


    int logCommand(const std:: string& command)
    {
        std::ofstream f;
        f.open(logfile, std::ios::app);
        if(commandCount > 0 && command[0] != ';') {
            f << commandCount << ": ";
            f << command << "\n";
            f << "\n";
            commandCount++;
        } else 
            f << command << "\n";
        f.close();
        return 1;
    }

    int logOutPut(const std:: string& output)
    {
        std::ofstream f;
        f.open(logfile, std::ios::app);
        f << output << "\n";
        f << "------------------------------------------------------------------------------------------"<< "\n";
        f.close();
        return 1;
    }


    int InitialLog(char *db, char *script)
    {
        time_t now = time(nullptr);
        std::ofstream f;
        f.open(logfile);
        if(!f.is_open())
            std::cerr << "File not opened";

        f << "Course Project for COMP 8042" << "\n";
        f << "Student Name: Bryan Hill, Student Id: A01020530" << "\n";
        f << "Begin of GIS Program log:" << "\n";
        f << "dbFile:\t" << db << "\n";
        f << "script:\t" <<script << "\n";
        f <<"log:\t" << logfile << "\n";
        f <<"Start Time: " << ctime(&now)<< "\n";
        f.close();
        return 1;
    }

    int logWorld(long xl, long xh, long yl, long yh)
    {
        std::ofstream f;
        f.open(logfile, std::ios::app);
        f << "\n";
        f << "------------------------------------------------------------------------------------------\n"
             "Latitude/longitude values in index entries are shown as signed integers, in total seconds.\n"
             "------------------------------------------------------------------------------------------" << "\n";
        f << "						                World boundaries are set to:\n";
        f << "                                                   " + std::to_string(yh) + "\n";
        f << "                                        "+std::to_string(xl) +"              " + std::to_string(xh) + "\n";
        f << "                                                   " + std::to_string(yl) + "\n";
        f << "------------------------------------------------------------------------------------------\n";
        f.close();
        return 1;
    }

    int FinalLog()
    {
        time_t now = time(nullptr);
        std::fstream f;
        f.open(logfile, std::ios::app);
        f << "End Time: " << ctime(&now)<<"\n";
        f.close();
        return 1;
    }

    private:
    char *logfile;

};

class CommandProcessor{
    public:
    CommandProcessor(char *database, char *commandScript, char *logFile)
    : command_script(commandScript),database(database), logger(logFile) {
        world = false;
        data = false;
        hash = hashtable();
        bufferPool = BufferPool();
        quad = quadtree(0,0,0,0,K_VALUE);
        xl = 0, xh = 0, yl= 0, yh = 0;
    }

    int Driver()
    {
        logger.InitialLog(database,command_script);
        std::ofstream tmp(database);
        tmp.close();
        std::ifstream script(command_script);
        std::string buff;
        while (std::getline(script,buff))
        {
            logger.logCommand(buff);
            ParseCommands(buff);
        }
        script.close();
        logger.FinalLog();
        return 0;
    }

    static std::vector<std::string> split(std::string const &str, char delim) {
        std::vector<std::string> tokens;
        size_t start = 0;
        size_t end = 0;

        while ((end = str.find(delim, start))!=std::string::npos)
        {
            if((end - start) == 0)
            {
                tokens.emplace_back("");
                start = end +1;
            } else{
                tokens.push_back(str.substr(start, end - start));
                start = end+1;
            }
        }

        /*while ((start = str.find_first_not_of(delim, end)) != std::string::npos) {
            end = str.find(delim, start);
            tokens.push_back(str.substr(start, end - start));
            }
            */
        return tokens;
    }



    static GISRecord processRaw(const std::string& line, int offs) {
        std::vector<std::string> str = split(line, '|');
        int fid = std::stoi(str[0]);
        std::string fname = str[1];
        std::string fclass = str[2];
        std::string state = str[3];
        int s_code = std::stoi(str[4]);
        std::string country = str[5];
        int c_code = std::stoi(str[6]);
        DMS_POS prim = {0, 0};
        DMS_POS sec = {0, 0};

        if (!str[8].empty() && !str[7].empty())
            prim = dmsPosition(str[8], str[7]);
        float pdecLat = std::stof(str[9]);
        float pdecLong = std::stof(str[10]);
        if (!str[12].empty() && !str[11].empty())
            sec = dmsPosition(str[12], str[11]);
        float sdecLat = 0;
        float sdecLong = 0;
        if (!str[13].empty() && !str[14].empty())
        {
            sdecLat = std::stof(str[13]);
            sdecLong = std::stof(str[14]);
        }
        coord primary = coord(prim.longitude, prim.latitude, offs);
        coord source = coord(sec.longitude, sec.latitude, offs);
        int elevM =0;
        int elevFT = 0;
        if (!str[15].empty())
            elevM = std::stoi(str[15]);
        if (!str[16].empty())
            elevFT = std::stoi(str[16]);
        std::string map = str[17];
        std::string dateC = str[18];
        std::string dateE;
        if (str.size() == 20)
            dateE = str[19];
        return {fid,fname,fclass,state,s_code,country,c_code,
                         primary, pdecLat, pdecLong, source, sdecLat, sdecLong,
                         elevM, elevFT, map, dateC, dateE, offs};
    }

    GISRecord getFromOffset(int offset)
    {
        int cnt = 0;
        std::ifstream records(database);
        std::string buff;
        while (std::getline(records,buff))
        {
            if (offset == cnt)
            {
                GISRecord g = processRaw(buff, offset);
                return g;
            }
            cnt++;
        }
        return {};
    }

    void import(const std::string& path)
    {
        std::ifstream records(path);
        std::ofstream db(database, std::ios::app);
        std::string buff;
        std::getline(records,buff);
        int prevQ = quad.getSize();
        int prevH = hash.getSize();
        int names = 0;
        int coords = 0;
        while (std::getline(records,buff))
        {
            GISRecord g = processRaw(buff, off);
            if (quad.insert(g.getCoord()))
            {
                NameIndex n = NameIndex(g.getFeatureName(), g.getStateAlpha(), true, g.getOffSet());
                hash.insert(n);
                db << buff << "\n";
                off++;
            }


        }

        logger.logOutPut("Imported Features by name: "+ std::to_string(hash.getSize()-prevH)+"\n" +
        "Longest probe sequence:    "+std::to_string(hash.getLongestProbe())+"\n"+
        "Imported Locations:        "+std::to_string(quad.getSize()-prevQ)+"\n");
        records.close();
        db.close();
    }

    int debug(const std::string& cmd)
    {
        if (cmd == "quad")
            logger.logOutPut(quad.toString());
        if (cmd == "hash")
            logger.logOutPut(hash.toString());
        if (cmd == "world")
            logger.logWorld(xl, xh, yl, yh);
        if (cmd == "pool")
            logger.logOutPut(bufferPool.toString());
        return 1;
    }

    int what_is_at(const std::string& latitude, const std::string& longitude)
    {
        DMS_POS g = dmsPosition(longitude, latitude);
        coord c = coord(g.longitude, g.latitude);

        coord * r = quad.find(c);
        if (r == nullptr)
        {
            logger.logOutPut("No records match "+ latitude + " and " + longitude + "\n");
            return 1;
        }
        std::vector <GISRecord> res;

        std::string out;

        for(auto &i: r->getOffset())
        {
            std::vector<GISRecord> a = bufferPool.findOff(i);
            if (a.empty())
                res.push_back(getFromOffset(i));
            else
                res.push_back(a[0]);
        }

        out.append("The following feature(s) were found at " + c.toString() + "\n");
        for(auto &i: res) {
            out.append(std::to_string(i.getOffSet()) + ": " + i.getFeatureName() + " " + i.getCountryName() + " " +
                       i.getStateAlpha() + "\n");
            bufferPool.insert(i);
        }
        logger.logOutPut(out);
        return 0;
    }

    int what_is(const std::string& f_name, const std::string& s_abbrev)
    {
        std::vector <GISRecord> res = bufferPool.findFeature(f_name, s_abbrev);
        std::string out;
        if (!res.empty())
        {
            out.append("The following feature(s) were found at " + f_name +"   "+ s_abbrev +"\n");
            out.append(std::to_string(res[0].getOffSet()) + ": " + res[0].getCountryName() + " " + res[0].getCoord().stringCoord());
            bufferPool.insert(res[0]);
            return 1;
        }
        int i = hash.findPos(f_name, s_abbrev);
        if (i < 0){
            logger.logOutPut("No records match "+ f_name + " and " + s_abbrev);
            return 1;
        }
        GISRecord g = getFromOffset(i);
        out.append("The following feature(s) were found at " + f_name +"   "+ s_abbrev +"\n");
        out.append(std::to_string(g.getOffSet()) + ": " + g.getCountryName() + " " + g.getCoord().stringCoord());
        bufferPool.insert(g);
        logger.logOutPut(out);
        return 0;
    }

    int what_is_in(const std::string& latitude, const std::string& longitude
                   , const std::string& h, const std::string& w, const std::string& opt)
    {
        DMS_POS d = dmsPosition(longitude, latitude);
        coord c = coord(d.longitude, d.latitude);
        long x = std::stoi(w);
        long y = std::stoi(h);
        long xlr = d.longitude - x;
        long xhr = d.longitude + x;
        long ylr = d.latitude - y;
        long yhr = d.latitude + y;
        std::vector<coord> r = quad.find(xlr,xhr,ylr,yhr);
        std::vector<coord> t;
        std::vector<GISRecord> got;
        for (auto &i : r) {
            std::vector <GISRecord> res = bufferPool.findCoordWithOpt(i.getLat(), i.getLong(), opt);
            if (!res.empty())
            {
                for (auto &e : res) {
                    got.push_back(e);
                }
            } else
                t.push_back(i);
        }
        std::string buff;
        std::vector<GISRecord> test = got;
        for (auto &i: t)
            for (auto &e : i.getOffset())
                got.push_back(getFromOffset(e));
        if (opt == "pop" || opt == "water" || opt == "structure")
        {
            got = what_is_in_filter(got, opt);
        }
        buff.append("The following feature(s) were found at ("
        + c.stringLat() + " +/- " + h + ", " + c.stringLong() + " +/- " + w + ")\n");
        if(opt == "long")
        {
            for(auto &i : got)
            {
                buff.append(i.toStringLong());
                buff.append("\n");
                bufferPool.insert(i);
            }
        } else
        {
            for(auto &i : got)
            {
                buff.append(i.toStringIn());
                bufferPool.insert(i);
            }
        }

        logger.logOutPut(buff);
        return 0;

    }

private:
    static std::vector<GISRecord> what_is_in_filter(const std::vector<GISRecord>& g, const std::string& filter)
    {
        std::vector<GISRecord> filtered;
        if (filter == "pop")
        {
            for(auto &i: g)
            {
                if(i.getFeatureClass() == "Populated Place")
                {
                    filtered.push_back(i);
                    continue;
                }
            }
        }
        else if (filter == "water")
        {
            for(auto &i: g)
            {
                const std::string& a = i.getFeatureClass();
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
                    filtered.push_back(i);
                    continue;
                }
            }
        }
        else if (filter == "structure")
        {
            for(auto &i: g)
            {
                const std::string& a = i.getFeatureClass();
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
                    filtered.push_back(i);
                    continue;
                }
            }
        }
        return filtered;
    }
public:

    int ParseCommands(std::string buff)
    {
        if (buff[0] == ';')
            return 0;
        int pos_command = buff.find('\t');
        std::string cmd = buff.substr(0, pos_command);
        if(cmd == "world")
        {
            if(world){
                logger.logCommand("Invalid command. World already called");
                return 0;
            }
            else {
                world = true;
                int space = buff.find('\t', pos_command+1);
                int space2 = buff.find('\t', space+1);
                int space3 = buff.find('\t', space2+1);
                std::string cord_long_left = buff.substr(pos_command+1, space-pos_command-1);
                std::string cord_long_right = buff.substr(space+1, space2-space-1);
                std::string cord_lat_bottom = buff.substr(space2+1, space3-space2-1);
                std::string cord_lat_top = buff.substr(space3+1);
                std::vector<int> a = defWorld(cord_long_left, cord_long_right, cord_lat_bottom, cord_lat_top);
                xl = a[0], xh = a[1], yl= a[2], yh = a[3];
                quad.setWorld(xl, xh, yl, yh);
                logger.logWorld(xl, xh, yl, yh);
                logger.commandCount++;
                return 1;
            }
        }
        if(!world){
            logger.logCommand("Invalid command. Must be called");
            return 0;
        }
        if(cmd == "import")
        {
            data = true;
            std::string file = buff.substr(pos_command+1);
            import(file);
        }
        else if (cmd == "debug")
        {
            std::string structure = buff.substr(pos_command+1);
            debug(structure);
        }
        else if(cmd == "quit")
        {
            logger.logOutPut("Terminating execution of commands");
            logger.FinalLog();
            exit(0);
        }
        if(!data){
            logger.logCommand("Invalid command. No database. Import must be called first");
            return 0;
        }
        if(cmd == "what_is_at")
        {
            int space = buff.find("\t", pos_command+1);
            std::string cord_lat = buff.substr(pos_command+1, space-pos_command-1);
            std::string cord_long = buff.substr(space+1);
            what_is_at(cord_lat, cord_long);
        }
        else if(cmd == "what_is")
        {
            int space = buff.find("\t", pos_command+1);
            std::string f_name = buff.substr(pos_command+1, space-pos_command-1);
            std::string s_abbrev = buff.substr(space+1);
            what_is(f_name, s_abbrev);
        }
        else if(cmd == "what_is_in")
        {
            int space = buff.find('\t', pos_command+1);
            int space2 = buff.find('\t', space+1);
            int space3 = buff.find('\t', space2+1);
            int space4 = buff.find('\t', space3+1);
            int space5 = buff.find('\t', space4+1);
            std::string cord_lat = buff.substr(pos_command+1, space-pos_command-1);
            std::string opt;
            std::string opt2;
            if(cord_lat == "-long")
            {
                cord_lat = buff.substr(space+1, space2-space-1);
                std::string cord_long = buff.substr(space2+1, space3-space2-1);
                std::string half_h = buff.substr(space3+1, space4-space3-1);
                std::string half_w = buff.substr(space4+1, space5-space4-1);
                what_is_in(cord_lat, cord_long, half_h, half_w, "long");
            }
            else if(cord_lat=="-filter")
            {
                int space6 = buff.find("\t", space5+1);
                opt2 = buff.substr(space+1, space2-space-1);
                cord_lat = buff.substr(space2+1, space3-space2-1);
                std::string cord_long = buff.substr(space3+1, space4-space3-1);
                std::string half_h = buff.substr(space4+1, space5-space4-1);
                std::string half_w = buff.substr(space5+1, space6-space5-1);
                what_is_in(cord_lat, cord_long, half_h, half_w, opt2);
            } else {
                std::string cord_long = buff.substr(space+1, space2-space-1);
                std::string half_h = buff.substr(space2+1, space3-space2-1);
                std::string half_w = buff.substr(space3+1, space4-space3-1);
                what_is_in(cord_lat, cord_long, half_h, half_w, "None");
            }
        }
        return 0;
    }
private:
    char *database;
    char *command_script;

    Logger logger;
    BufferPool  bufferPool;
    quadtree  quad;
    hashtable  hash;
    bool world;
    bool data;
    int xl, xh, yl, yh;
    long off = 0;



};




int main(int argc, char *argv[]) {
    if(argc != 4)
        exit(-1);
    CommandProcessor manager = CommandProcessor(argv[1], argv[2], argv[3]);
    manager.Driver();
    return 0;
}
