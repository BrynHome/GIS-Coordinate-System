# GIS Coordinate System
## Description  
System that indexes and provides search features for a file of GIS records.  
The system makes use of several in-memory index data structures to support:
- Importing new GIS records into the database file
- Retrieving data for all GIS records matching given geographic coordinates
- Retrieving data for all GIS records matching matching a given feature name and state
- Retrieving data for all GIS records matching that fall within a given rectangular geographic region
- Display the in-memory indices in a human-readable manner

## Usage Guide
### Compiling
Compiling is simple as there is just one *.cpp file, GIS.cpp. The rest of files consist of header files that must be present in order for the code to function as intended.
```
G++ GIS.cpp -o GIS
```
### Executing
```
./GIS <database file name> <command script file name> <log file name>
```
## Documentation
- [Report](https://docs.google.com/document/d/16or_QDB4okoWITpq7LFuJ33Y3Ta_ovBvKyHHJkZe41A/edit?usp=sharing)
