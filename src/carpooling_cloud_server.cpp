/**
 * @Author: Zejiang Zeng <yzy>
 * @Date:   2018-11-10T15:40:20-05:00
 * @Email:  zzeng@terpmail.umd.edu
 * @Filename: carpooling_cloud_server.cpp
 * @Last modified by:   yzy
 * @Last modified time: 2018-11-10T15:40:27-05:00
 * @Copyright: (C) 2017 Zejiang Zeng - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the BSD license.
 */
#include "carpooling_cloud_server.h"
#include <math.h>
/*
 * @brief: Constructor, initialize the Manhattan map as a (x by y) grid
 */
Carpooling_cloud_server::Carpooling_cloud_server(unsigned int x, unsigned int y) : x_boundary(x),y_boundary(y){
        places.push_back(vehicle.v_loc);
        std::cout << "The car initial location is [0,0]" << '\n';
}

/*
 * @brief: Constructor, initialize the Manhattan map as a (x by y) grid and check if the init location of the car is beyond the range
 */
Carpooling_cloud_server::Carpooling_cloud_server(Car v,unsigned int x, unsigned int y) : vehicle(v),x_boundary(x),y_boundary(y){
        if(vehicle.v_loc.first > x_boundary || vehicle.v_loc.second > y_boundary)
                throw("Invalid init vechile location");
        else {
                places.push_back(vehicle.v_loc);
                std::cout << "The car inital location is ["<<vehicle.v_loc.first<<", "<<vehicle.v_loc.second <<" ]"<<'\n';
        }

}

/**
 * @brief: Calculate the Manhattan distance between two vertex
 * @param  a [Location] coordinate (x,y) of the vertex
 * @param  b [Location] coordinate (x,y) of the vertex
 * @return   [int] Manhattan distance
 */
int Carpooling_cloud_server::calcualteD(Location a, Location b) const {
        return abs(a.first - b.first) + abs(a.second - b.second);
}
/**
 * @brief: Create a skew matrix that contains the distance between each vertex
 */
void Carpooling_cloud_server::buildGrids(){
        int size = places.size();
        grid = std::vector<std::vector<unsigned int> >(size, std::vector<unsigned int>(size,0));
        for(int i = 0; i< size - 1; i++) {
                for(int j = i+1; j< size; j++) {
                        int dis = calcualteD(places[i],places[j]);
                        grid[i][j] = grid[j][i] = dis;
                }
        }
        VISITED_ALL.assign(size,1);
        mask.assign(size,0);
        mask[0] = 1;
        createDPtable(size);
}
/**
 * @brief: add the request info to the RequestList, which will read the info (name, start, end) of each person.
 * @param request [Json::Value]  request info in Json
 */
void Carpooling_cloud_server::ReadRequest(Json::Value request){
        if(!request.empty()) {
                for(int i = 0; i < request["request"].size(); i++) {
                        RequestInfo requestinfo;
                        // Read the name, start and end location from the request
                        std::string personName = request["request"][i]["name"].asString();
                        Location start(request["request"][i]["start"][0].asInt(),request["request"][i]["start"][1].asInt());
                        Location end(request["request"][i]["end"][0].asInt(),request["request"][i]["end"][1].asInt());
                        requestinfo.name = personName; requestinfo.start = start; requestinfo.end = end;
                        if(start.first < x_boundary && end.first < x_boundary && start.second < y_boundary && end.second < y_boundary)
                                RequestList.push_back(requestinfo);
                        else
                                throw("Invalid request location");
                }
        }
}
/**
 * @brief: Base on a specific threshold, read limit number of requests from the RequestList
 * @param NumVertex [int] threshold of the request number (each request has two nodes/locations) to insert into the graph.The maximum value would be 32, but a relatively small number is recommended, say 5 or 10.
 */
void Carpooling_cloud_server::add_start(int NumVertex){
        if(!RequestList.empty() && startToend.size() < NumVertex) {
                for(int i = startToend.size(); i < NumVertex; i++) {
                        if (!RequestList.empty()) {
                                // Add the name of the passenger to the list
                                RequestInfo requestinfo = RequestList.front();
                                names.push_back(requestinfo.name);
                                // Read the start and end location from the request
                                startToend.insert(std::make_pair(requestinfo.name,requestinfo.end));
                                places.push_back(requestinfo.start);
                                pick_drop.push_back(0);
                                RequestList.pop_front();
                        }
                        else
                                return;
                }
        }
}

/**
 * @brief: Create a dp table to store the possible states with a given mask and init position. It is used to optimize the path search in TSP function]
 * @param size [int] number of vertex
 */
void Carpooling_cloud_server::createDPtable(int size){
        std::size_t exp = exp2(size); // Distinct states in DP is 2 ^n
        std::vector<std::vector<int> > dpTable(exp,std::vector<int>(size)); // a 2^n by n table
        for(int i = 0; i < dpTable.size(); i++) {
                for(int j = 0; j < dpTable[i].size(); j++) {
                        dpTable[i][j] = -1;   // initialize all the element in the vector as -1
                }
        }
        dp = dpTable;
}
/**
 * @brief: This is the algorithm to solve the TravelSalesMan problem using Dynamic programming, it will find the shortest possible route that visits each node once without returning back to the original node.
 * with the dp table, this optimized search will provide the solution with time complexity O(n^2 * 2^n)
 * @param  mask [vector<int>] 0 for unvisited node, 1 for visited node
 * @param  pos  [int] start vertex index, usually is 0
 * @param  node [Node] init node. Use to find the next node to travel
 * @return      [int]  the length of the shortest path
 */
int Carpooling_cloud_server::TravelSalesMan(std::vector<int> mask,int pos,std::shared_ptr<Node> node) {
        // if all nodes are visited, then return the init node, which means remain in a place.
        if(mask == VISITED_ALL) {
                return 0;
        }
        std::size_t m = 0;
        for(int i = 0; i < mask.size(); i++) {
                m += mask[i]*exp2(i);
        }
        if(dp[m][pos] != -1) { // if the sub-problem is overlapped. Namely, the sub-problem is calculated before
                return dp[m][pos];
        }
        int ans = INT_MAX;
        // Using Dynamic Programming to find the shortest path that can Travel all the vertexes
        for (int index = 0; index < places.size(); index++) {
                if(mask[index] == 0) {
                        std::shared_ptr<Node> newNode =  std::make_shared<Node>(index);
                        std::vector<int> newMask = mask;
                        newMask[index] = 1; // Mark the visited node as 1
                        // Break the problem into sub-problems following the recursive relation below
                        int newAns = grid[pos][index] + TravelSalesMan(newMask,index,newNode);
                        if(ans > newAns)
                                // If find a shorter path, mark this vertex as the next vertex of the original vertex
                                node->next = newNode;
                        ans = std::min(ans, newAns);
                }
        }

        // store the current problem into the look-up table
        return dp[m][pos] = ans;
}
/**
 * [Carpooling_cloud_server::makeAction Making an action: move, drop or pick for the current vertex]
 * @param target [Location] the target vertex need to move to
 * @param index  [int] index number
 */
void Carpooling_cloud_server::makeAction(Location target,int index){

// Comparing the current location of the car with the target node location

// If the current location is same as target location, which means arrived,then it's time to pickup or drop off passengers
        if(vehicle.v_loc.first == target.first && vehicle.v_loc.second == target.second) {
// Check if the target location is for picking up or dropping off, 0 for pick up, 1 for drop off
                if(pick_drop[index -1] == 0) { // pick up
                        std::cout <<"The vehicle now is at ["<<vehicle.v_loc.first<<", "<<vehicle.v_loc.second<<"]. "<< "Pick up the passenger: "<< names[index -1] << '\n';
                        vehicle.n_passengers++;
                        vehicle.passenger.insert(names[index -1]); // add this passenger
                        add_end(index); // add the end location to the graph
                        deleteVertex(index); // delete this visited start vertex
                        return;
                }
                else if(pick_drop[index -1] == 1) { // 1 for drop off
                        std::cout <<"The vehicle now is at ["<<vehicle.v_loc.first<<", "<<vehicle.v_loc.second<<"]. "<< "Drop off the passenger: "<< names[index -1] << '\n';
                        vehicle.n_passengers--;
                        if(vehicle.passenger.find(names[index -1]) == vehicle.passenger.end()) {
                                //std::cout << "This passenger is not on the vechile" << '\n';
                                throw("This passenger is not on the vehicle");
                                return;
                        }
                        vehicle.passenger.erase(vehicle.passenger.find(names[index -1]));
                        startToend.erase(startToend.find(names[index -1]));
                        deleteVertex(index);
                }
                else {
                        std::cout << "Check the index" << '\n';
                        return;
                }

        }
        // If not the same, then need to move one step forward
        else {
                // choose to the path that makes minimal turns, the car will move along x axis first and then move along y axis
                if(vehicle.v_loc.first != target.first)
                        vehicle.v_loc.first < target.first ? vehicle.moveOneStep(1) : vehicle.moveOneStep(-1);
                else
                        vehicle.v_loc.second < target.second ? vehicle.moveOneStep(2) : vehicle.moveOneStep(-2);
                places[0] = vehicle.v_loc; // update the current location
                std::cout << "The vehicle now is at ["<<vehicle.v_loc.first<<", "<<vehicle.v_loc.second<<"]"<< ". There are "<<vehicle.n_passengers<<" passengers in the car.";
                if(vehicle.n_passengers > 0) {
                        std::cout << " They are : ";
                        for(const auto & i : vehicle.passenger)
                                std::cout << i << " ";
                }
                std::cout << '\n';
        }
        return;
}
/**
 * @brief: delete the start or end point in the list, NOTE: the index here is the specified position in vector "places", which contains one more element comparing to other vectors
 * @param index [int]
 **/
void Carpooling_cloud_server::deleteVertex(int index) {
        names.erase(names.begin() + index -1);
        places.erase(places.begin() + index);
        pick_drop.erase(pick_drop.begin() + index -1);
        return;
}
/**
 * @brief: add the endpoint to the vector "places",which will be used to build the vertex graph later.
 * @param index [int]
 */
void Carpooling_cloud_server::add_end(int index){
        Location end = startToend[names[index -1]];
        names.push_back(names[index -1]);
        places.push_back(end);
        pick_drop.push_back(1);
        return;
}
/**
 * @brief: The function that implements the carpooling algorithm
 * @param request [Json::Value request]
 */
void Carpooling_cloud_server::carpooling(Json::Value request) {
        ReadRequest(request);
        add_start(10); // Read maximum of 10 requests and put other requests in the waiting list
        if(!names.empty()) {
                buildGrids();   // Build the vertex graph
                int shortestPath = TravelSalesMan(mask,0,initNode); // Find the shortest path and return the index of next node
                int index = initNode->next->data;
                Location point = places[index];
                makeAction(point,index);   // Make action according to the desired node and corrent vehicle location
                dp.clear();
        }
}
