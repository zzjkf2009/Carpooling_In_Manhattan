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

/*
 * @brif: Constructor, initialize the Manhattan map as a (x by y) grid
 */
Carpooling_cloud_server::Carpooling_cloud_server(unsigned int x, unsigned int y) : x_boundary(x),y_boundary(y){
        places.push_back(vehicle.v_loc);
        std::cout << "The car initial location is [0,0]" << '\n';
}

/*
 * @brif: Constructor, initialize the Manhattan map as a (x by y) grid and check if the init location of the car is beyond the range
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
 * @brif: Calculate the Manhattan distance between two vertex
 * @param  a [Location] coordinate (x,y) of the vertex
 * @param  b [Location] coordinate (x,y) of the vertex
 * @return   [int] Manhattan distance
 */
int Carpooling_cloud_server::calcualteD(Location a, Location b) const {
        return abs(a.first - b.first) + abs(a.second - b.second);
}
/**
 * @brif: Create a skew matrix that contains the distance between each vertex
 */
void Carpooling_cloud_server::buildGrids(){
        unsigned int size = places.size();
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
}
/**
 * @brif: add the request info to the server, which will read the info (name, start, end) of each person.
 * @param request [Json::Value] read the request info
 */
void Carpooling_cloud_server::add_start(Json::Value request){
        if(!request.empty()) {
                for(int i = 0; i < request["request"].size(); i++) {
                        // Add the name of the passenger to the list
                        std::string personName = request["request"][i]["name"].asString();
                        names.push_back(personName);
                        // Read the start and end location from the request
                        Location start(request["request"][i]["start"][0].asInt(),request["request"][i]["start"][1].asInt());
                        Location end(request["request"][i]["end"][0].asInt(),request["request"][i]["end"][1].asInt());
                        // Check if the location is inside the boundary
                        if(start.first < x_boundary && end.first < x_boundary && start.second < y_boundary && end.second < y_boundary) {
                                startToend.insert(std::make_pair(personName,end));
                                // Add the location of the start point to the list
                                places.push_back(start);
                                // Add the indicator of the location, 0 for pick up, 1 for drop off
                                pick_drop.push_back(0);
                        }
                        else
                                throw("Invalid request location");

                }

        }
}
/**
 * @brif This is the algorithm to solve the TravelSalesMan problem using Dynamic programming, it will find the shortest possible route that visits each node once without returning back to the original node.
 * @param  mask [vector<int>] 0 for unvisited node, 1 for visited node
 * @param  pos  [int] start vertex index, usually is 0
 * @param  node [Node] init node. Use to find the next node to travel
 * @return      [int]  index of the next node
 */
int Carpooling_cloud_server::TravelSalesMan(std::vector<int> mask,int pos,std::shared_ptr<Node> node) {
        // if all nodes are visited, then return the init node, which means remain in a place.
        if(mask == VISITED_ALL) {
                return 0;
        }
        int ans = INT_MAX;
        // Using Dynamic Programming to find the shortest path that can Travel all the vertex
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

        return node->next->data;
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
 * @brif: delete the start or end point in the list, NOTE: the index here is the specified position in vector "places", which contains one more element comparing to other vectors
 * @param index [int]
 **/
void Carpooling_cloud_server::deleteVertex(int index) {
        names.erase(names.begin() + index -1);
        places.erase(places.begin() + index);
        pick_drop.erase(pick_drop.begin() + index -1);
        return;
}
/**
 * @brif: add the endpoint to the vector "places",which will be used to build the vertex graph later.
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
 * @brif: The function that implements the carpooling algorithm
 * @param request [Json::Value request]
 */
void Carpooling_cloud_server::carpooling(Json::Value request) {
        add_start(request);  // Read the request
        if(!names.empty()) {    // If there is a request at this time
                buildGrids();   // Build the vertex graph
                int index = TravelSalesMan(mask,0,initNode); // Find the shortest path and return the index of next node
                Location point = places[index];
                makeAction(point,index);   // Make action according to the desired node and corrent vehicle location
        }
}
