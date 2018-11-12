/**
 * @Author: Zejiang Zeng <yzy>
 * @Date:   2018-11-12T15:27:04-05:00
 * @Email:  zzeng@terpmail.umd.edu
 * @Filename: carpooling_cloud_server.h
 * @Last modified by:   yzy
 * @Last modified time: 2018-11-12T15:27:11-05:00
 * @Copyright: (C) 2017 Zejiang Zeng - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the BSD license.
 */
#ifndef CARPOOLING_CLOUD_SERVER
#define CARPOOLING_CLOUD_SERVER

 #include "car.h"
 #include <jsoncpp/json/json.h>
 #include <jsoncpp/json/value.h>
 #include <vector>
 #include <utility>
 #include <unordered_map>
 #include <cmath>
 #include <memory>
 #include <climits>

struct Node {
        Node(int i) : data(i){
                next = NULL;
        };
        int data;
        std::shared_ptr<Node> next;
};

/**
 * @brif: This is the class that reads the carpool requests and use the Dynamic Programming to
 * solve the shortest path among the vertex (start and end locations). Travelling Salesman Problem
 */
class Carpooling_cloud_server {
public:
Carpooling_cloud_server(unsigned int x, unsigned int y);
Carpooling_cloud_server(Car v,unsigned int x, unsigned int y);
//private:
std::vector<std::vector<unsigned int> > grid;  // A skew matrix that contains the distance between each vertex
std::unordered_map<std::string,Location> startToend;  // A map that maps the name of the customer to his/her desitination
// The following vector share the same index, TODO: find a beeter container to store the data, list is good for inserting and deleting, but bad at accessing element
std::vector<std::string> names;  // an array to record the name
std::vector<int> pick_drop;  // an array to record the if the location is for droping or picking up.
// the array places has one more element comaparing to vector above, the init_loc
std::vector<Location> places;  // an array to record the start and end locations
int VISITED_ALL;
std::shared_ptr<Node> initNode = std::make_shared<Node>(0);
Car vehicle;
unsigned int x_boundry;
unsigned int y_boundry;

int calcualteD(Location a, Location b) const;
void buildGrids();
void add_start(Json::Value request);
int  TravelSalesMan(int mask,int pos,std::shared_ptr<Node> node);
void makeAction(Location target,int index);
void deleteVertex(int index);
void add_end(int index);
void carpooling(Json::Value request);
};

#endif
