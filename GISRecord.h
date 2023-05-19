//
// Created by root on 30/11/22.
//

#ifndef FINALPROJECT_GISRECORD_H
#define FINALPROJECT_GISRECORD_H


#include <string>
#include "coord.h"

class GISRecord{
public:
    GISRecord()= default;

    GISRecord(int featureId, std::string featureName, std::string featureClass,
              std::string stateAlpha, int stateNumber, std::string countryName, int countryNumber,
              coord primary, float primaryLatDec, float primaryLongDec, coord source,
              float sourceLatDec, float sourceLongDec, int elevM, int elevFt, std::string mapName,
              std::string dateCreated, std::string dateEdited, int off) :
            feature_id(featureId),
            feature_name(std::move(featureName)),
            feature_class(std::move(featureClass)),
            state_alpha(std::move(stateAlpha)),
            state_number(stateNumber),
            country_name(std::move(countryName)),
            country_number(countryNumber),
            primary_lat_dec(primaryLatDec),
            primary_long_dec(primaryLongDec),
            source_lat_dec(sourceLatDec),
            source_long_dec(sourceLongDec),
            elev_m(elevM), elev_ft(elevFt),
            map_name(std::move(mapName)),
            date_created(std::move(dateCreated)),
            date_edited(std::move(dateEdited)) {

        featCoord = std::move(primary);
        sourceCoord = std::move(source);
        offset = off;
    }

    int getFeatureId() const {
        return feature_id;
    }

    int getOffSet() const{
        return offset;
    }

    const std::string &getFeatureName() const {
        return feature_name;
    }

    const std::string &getFeatureClass() const {
        return feature_class;
    }

    const std::string &getStateAlpha() const {
        return state_alpha;
    }

    int getStateNumber() const {
        return state_number;
    }

    const std::string &getCountryName() const {
        return country_name;
    }

    int getCountryNumber() const {
        return country_number;
    }

    int getFeatureLat() const {
        return featCoord.getLat();
    }

    int getFeatureLong() const {
        return featCoord.getLong();
    }

    float getPrimaryLatDec() const {
        return primary_lat_dec;
    }

    float getPrimaryLongDec() const {
        return primary_long_dec;
    }

    int getSourceLat() const {
        return sourceCoord.getLat();
    }

    int getSourceLong() const {
        return sourceCoord.getLong();
    }

    float getSourceLatDec() const {
        return source_lat_dec;
    }

    float getSourceLongDec() const {
        return source_long_dec;
    }

    int getElevM() const {
        return elev_m;
    }

    int getElevFt() const {
        return elev_ft;
    }

    const std::string &getMapName() const {
        return map_name;
    }

    const std::string &getDateCreated() const {
        return date_created;
    }

    const std::string &getDateEdited() const {
        return date_edited;
    }

    coord getCoord() {
        return featCoord;
    }

    bool featureEqual(const std::string& feat, const std::string& state)
    {
        return (feat == feature_name && state == state_alpha);
    }



    std::string coordSearchToString() const {
        return std::to_string(offset) + ": " + feature_name + " " + country_name + " " + state_alpha;
    }

    std::string featureSearchToString() const {
        return std::to_string(offset) + ": " + country_name + " " + featCoord.stringCoord();
    }

    std::string toStringRaw() const {
        return std::to_string(offset) + ": " + std::to_string(feature_id) + "|"
        + feature_name + "|" +feature_class+"|"+state_alpha+"|"+std::to_string(state_number)
        +"|"+country_name + std::to_string(country_number) +"|"
        +featCoord.stringCoord() +"|"+ std::to_string(primary_lat_dec) +"|"+ std::to_string(primary_long_dec)
        +"|"+ sourceCoord.stringCoord() +"|"+ std::to_string(source_lat_dec) +"|"+ std::to_string(source_long_dec)
        +"|" + std::to_string(elev_m) +"|"+ std::to_string(elev_ft) +"|"+ map_name +"|"+ date_created
        +"|"+ date_edited + "\n";
    }

    std::string toStringIn() const {
        return std::to_string(offset) + ": " + feature_name + " " + state_alpha + " " + featCoord.stringCoord() +"\n";
    }

        std::string toStringLong() const {
        std::string buff;
        buff.append(" Feature ID   : "
        + std::to_string(feature_id) +"\n" +
        "  Feature Name : " + feature_name +"\n" +
        "  Feature Cat  : "+feature_class+"\n" +
                    "  State        : "+ state_alpha +"\n" +
                    "  County       : "+ country_name +"\n" +
                    "  Longitude    : " + featCoord.stringLong() +"\n" +
                    "  Latitude     : "+ featCoord.stringLat()+"\n" +
                    "  Elev in ft   : "+ std::to_string(elev_ft) +"\n" +
                    "  USGS Quad    : "+ map_name +"\n" +
                    "  Date created : "+ date_created +"\n");
        return buff;
    }

    bool operator==(const GISRecord &rhs) const {
        return feature_id == rhs.feature_id &&
               feature_name == rhs.feature_name &&
               feature_class == rhs.feature_class &&
               state_alpha == rhs.state_alpha &&
               state_number == rhs.state_number &&
               country_name == rhs.country_name &&
               country_number == rhs.country_number &&
               featCoord.equals(rhs.featCoord) &&
               primary_lat_dec == rhs.primary_lat_dec &&
               primary_long_dec == rhs.primary_long_dec &&
               sourceCoord.equals(rhs.sourceCoord) &&
               source_lat_dec == rhs.source_lat_dec &&
               source_long_dec == rhs.source_long_dec &&
               elev_m == rhs.elev_m &&
               elev_ft == rhs.elev_ft &&
               map_name == rhs.map_name &&
               date_created == rhs.date_created &&
               date_edited == rhs.date_edited &&
               offset == rhs.offset;
    }

    bool operator!=(const GISRecord &rhs) const {
        return !(rhs == *this);
    }

private:
    int feature_id{};
    std::string  feature_name;
    std::string feature_class;
    std::string state_alpha;
    int state_number{};
    std::string country_name;
    int country_number{};
    coord featCoord = coord(0, 0);
    float primary_lat_dec{};
    float primary_long_dec{};
    coord sourceCoord = coord(0, 0);
    float source_lat_dec{};
    float source_long_dec{};
    int elev_m{};
    int elev_ft{};
    std::string  map_name;
    std::string  date_created;
    std::string  date_edited;
    int offset;
};


#endif //FINALPROJECT_GISRECORD_H
