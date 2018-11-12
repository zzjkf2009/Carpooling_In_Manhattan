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
Carpooling_cloud_server::Carpooling_cloud_server(unsigned int x, unsigned int y) : x_boundry(x),y_boundry(y){
        places.push_back(vhicle.v_loc);
}

Carpooling_cloud_server::Carpooling_cloud_server(Car v,unsigned int x, unsigned int y) : vhicle(v),x_boundry(x),y_boundry(y){
        if(vhicle.v_loc.first > x_boundry || vhicle.v_loc.second > y_boundry)
                throw("Invalid init vechile location");
        else
                places.push_back(vhicle.v_loc);
}

/**
 * @brif: calculate the mahanttan distance between two vertex
 * @param  a [Location] corrdinate info (x,y) of the vertex
 * @param  b [Location] corrdinate info (x,y) of the vertex
 * @return   [int] mahanttan distance
 */
int Carpooling_cloud_server::calcualteD(Location a, Location b) const {
        return abs(a.first - b.first) + abs(a.second - b.second);
}
/**
 * @brif: Create a grid, which is a skew matrix, contain distance info between each vertex
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
        VISITED_ALL = (1<<size) -1;
}
/**
 * @brif: add the request info to the server, which will read the info of each person and store them
 * @param request [Json::Value] read the request info
 */
void Carpooling_cloud_server::add_start(Json::Value request){
        if(!request.empty()) {
                for(int i = 0; i < request["request"].size(); i++) {
                        // add the name of the passanger to the list
                        std::string personName = request["request"][i]["name"].asString();
                        names.push_back(personName);
                        // Create the location for request's start and desitination point
                        Location start(request["request"][i]["start"][0].asInt(),request["request"][i]["start"][1].asInt());
                        Location end(request["request"][i]["end"][0].asInt(),request["request"][i]["end"][1].asInt());
                        if(start.first < x_boundry && end.first < x_boundry && start.second < y_boundry && end.second < y_boundry) {
                                startToend.insert(std::make_pair(personName,end));
                                // add the location of the start point
                                places.push_back(start);
                                // add the indicator of the location as a pick up or drop point, 0 for pick up, 1 for drop
                                pick_drop.push_back(0);
                        }
                        else
                                throw("Invalid request location");

                }

        }
}
/**
 * @brif This is the algorithm to solve the Travel Sales Man problem using Dynamic programming, the mask can be consiter as a array that marker if the vertex is visited or not,
 * here, it is convert an int to binary form using >> opeartor (suck as 15 is 1111) and check if it is visited (0 for univisited, 1 for visited)
 * @param  mask [int]
 * @param  pos  [int] start vertex, usually as 0
 * @param  node [Node] init node, use to find its next node
 * @return      [int] next node index
 */
int Carpooling_cloud_server::TravelSalesMan(int mask,int pos,std::shared_ptr<Node> node) {
        // if all nodes are visited return the init node, which means stay in place.
        if(mask==VISITED_ALL) {
                return 0;
        }
        int ans = INT_MAX;
        // Using Dynamic Programming to find the shortest path that can Travel all the vertex at once
        for (int city = 0; city < places.size(); city++) {
                if((mask&(1<<city))==0) {
                        std::shared_ptr<Node> newNode =  std::make_shared<Node>(city);
                        int newAns = grid[pos][city] + TravelSalesMan( mask|(1<<city), city,newNode);
                        if(ans > newAns)
                                // If find a shorter path, mark this vertex as the next vertex of the original vertex
                                node->next = newNode;
                        ans = std::min(ans, newAns);
                }
        }

        return node->next->data;
}

void Carpooling_cloud_server::makeAction(Location target,int index){

// comaparing the current location of the car with the target node location

// If the current location is same as target location, means arriveed, time to pick up or drop passangers
        if(vhicle.v_loc.first == target.first && vhicle.v_loc.second == target.second) {
// Check if the target location is for picking up or droppings, 0 for pick up , 1 for drop
                if(pick_drop[index -1] == 0) { // pick up
                        std::cout <<"The vhicle now is at ["<<vhicle.v_loc.first<<", "<<vhicle.v_loc.second<<"]. "<< "Pick up the passanger: "<< names[index -1] << '\n';
                        vhicle.n_passangers++;
                        vhicle.passanger.insert(names[index -1]); // add this passanger
                        add_end(index); // add the end location to the graph
                        deleteVertex(index); // delete this visited start point
                        return;
                }
                else if(pick_drop[index -1] == 1) { // 1 for drop
                        std::cout <<"The vhicle now is at ["<<vhicle.v_loc.first<<", "<<vhicle.v_loc.second<<"]. "<< "Drop the passanger: "<< names[index -1] << '\n';
                        vhicle.n_passangers--;
                        if(vhicle.passanger.find(names[index -1]) == vhicle.passanger.end()) {
                                //std::cout << "This passanger is not on the vechile" << '\n';
                                throw("This passanger is not on the vechile");
                                return;
                        }
                        vhicle.passanger.erase(vhicle.passanger.find(names[index -1]));
                        startToend.erase(startToend.find(names[index -1]));
                        deleteVertex(index);
                }
                else {
                        std::cout << "There is the error in the index" << '\n';
                        return;
                }

        }
        // If not same, need to move one step
        else {
                // choose to the path that make mininmal turns, so the car will move along x first and then move along y
                if(vhicle.v_loc.first != target.first)
                        vhicle.v_loc.first < target.first ? vhicle.moveOneStep(1) : vhicle.moveOneStep(-1);
                else
                        vhicle.v_loc.second < target.second ? vhicle.moveOneStep(2) : vhicle.moveOneStep(-2);
                places[0] = vhicle.v_loc; // update the current location
                std::cout << "The vhicle now is at ["<<vhicle.v_loc.first<<", "<<vhicle.v_loc.second<<"]"<< ". There are "<<vhicle.n_passangers<<" passangers in the car.";
                if(vhicle.n_passangers > 0) {
                        std::cout << " They are : ";
                        for(const auto & i : vhicle.passanger)
                                std::cout << i << " ";
                }
                std::cout << '\n';
        }
        return;
}
/**
 * @brif: delete the start or end point in the vector,NOTE: the index is the deisred element in vector places, which contains one more element comaparing tp other vectors,
 *
 * @param index [int] keep in mind, the index is the index of the desired element in vector places, it should minus 1 for ohter vector
 */
void Carpooling_cloud_server::deleteVertex(int index) {
        names.erase(names.begin() + index -1);
        places.erase(places.begin() + index);
        pick_drop.erase(pick_drop.begin() + index -1);
        return;
}
/**
 * @brif: add the end point to the vector place as a vertex,NOTE: the index is same as the last one from the vector places, remmember to minus one
 * @param index [description]
 */
void Carpooling_cloud_server::add_end(int index){
        Location end = startToend[names[index -1]];
        names.push_back(names[index -1]);
        places.push_back(end);
        pick_drop.push_back(1);
        return;
}
/**
 * @brif: function that implement the carpooling algorithm
 * @param request [Json::Value request]
 */
void Carpooling_cloud_server::carpooling(Json::Value request) {
        add_start(request);  // Read the request
        if(!names.empty()) {    // If has request
                buildGrids();   // Build the vertex graph
                int index = TravelSalesMan(1,0,initNode); // Find the shortest path and return the next node index
                Location point = places[index];
                makeAction(point,index);   // Make action according to the desired node and corrent vhicle location
        }
}
