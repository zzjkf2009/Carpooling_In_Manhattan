#include <gtest/gtest.h>
#include <iostream>
#include <carpooling_cloud_server.h>

/*
   class carpoolingTest : public ::testing::Test {
   protected:
   carpoolingTest {
   }
   void SetUp() override {
        Location a(2,3);
        Location b(4,6);
        Location c(1,7);
        Carpooling_cloud_server cloudServer;
        cloudServer.places.push_back(a);
        cloudServer.places.push_back(b);
        cloudServer.places.push_back(c);
        cloudServer.buildGrids();
   }
   }; */

/**
 * [TEST Testing if the init car location is beyond the range]
 */
TEST(carpoolingTest,constructorTest){
        Car car({12,32});
        EXPECT_ANY_THROW(Carpooling_cloud_server cloudServer(car,10,10));

}

/**
 * [TEST Testing the Manhattan distance between two vertexes (location)]
 */
TEST(carpoolingTest, calcualteDTest) {
        Location a(2,3);
        Location b(4,6);
        Location c(1,7);
        Carpooling_cloud_server cloudServer(10,10);
        EXPECT_EQ(5, cloudServer.calcualteD(a,b));
        EXPECT_EQ(5, cloudServer.calcualteD(a,c));
        EXPECT_EQ(4, cloudServer.calcualteD(c,b));
}

/**
 * [TEST Testing if the distance grid(skew matrix) is built correctly]
 */
TEST(carpoolingTest, buildGridsTest) {
        Location a(2,3);
        Location b(4,6);
        Location c(1,7);
        Carpooling_cloud_server cloudServer(10,10);   // the init_loc of the car has already been added to the vector "places" in the constructor
        cloudServer.places.push_back(a);
        cloudServer.places.push_back(b);
        cloudServer.places.push_back(c);
        cloudServer.buildGrids();
        EXPECT_EQ(4, cloudServer.grid.size());
        EXPECT_EQ(0, cloudServer.grid[1][1]);
        EXPECT_EQ(0, cloudServer.grid[2][2]);
        EXPECT_EQ(0, cloudServer.grid[3][3]);
        EXPECT_EQ(10, cloudServer.grid[0][2]);
        EXPECT_EQ(cloudServer.calcualteD(a,b), cloudServer.grid[1][2]);
        EXPECT_EQ(cloudServer.calcualteD(b,a), cloudServer.grid[2][1]);
        EXPECT_EQ(cloudServer.grid[2][1], cloudServer.grid[1][2]);
        EXPECT_EQ(cloudServer.calcualteD(b,c), cloudServer.grid[3][2]);
}

/**
 * [TEST Testing if the TravelSalesMan solver can find the shortest path]
 */
TEST(carpoolingTest, TSPTest) {
        Location a(2,3);
        Location b(4,6);
        Location c(0,7);
        Carpooling_cloud_server cloudServer(10,10);
        cloudServer.places.push_back(a);
        cloudServer.places.push_back(b);
        cloudServer.places.push_back(c);
        cloudServer.buildGrids();
        cloudServer.TravelSalesMan(cloudServer.mask,0,cloudServer.initNode);
        EXPECT_EQ(1, cloudServer.initNode->next->data);
        std::cout << "next node is "<<cloudServer.places[cloudServer.TravelSalesMan(cloudServer.mask,0,cloudServer.initNode)].second << '\n';
}
/**
   *[TEST Testing reading requests from the JSON file]
 **/

TEST(carpoolingTest, add_startTest){
        Json::Reader reader;
        Json::Value request;
        std::string text = "{\"request\":[{\"name\":\"Elson\",\"start\":[2,3],\"end\":[3,8]},{\"name\":\"Peter\",\"start\":[2,3],\"end\":[3,8]}]}";
        if(!reader.parse(text,request))
                std::cout<<reader.getFormattedErrorMessages()<<std::endl;
        Carpooling_cloud_server cloudServer(10,10);          // the init_loc of the car is allready added to the places in constructor
        cloudServer.ReadRequest(request); // Read the request
        cloudServer.add_start(10);
        EXPECT_EQ(2,cloudServer.names.size()); // add two names
        EXPECT_EQ(3,cloudServer.places.size()); // including the current location
        EXPECT_EQ(2,cloudServer.pick_drop.size()); // add two locations mark as start point
        EXPECT_EQ(2,cloudServer.startToend.size()); // add two pair<name, end> to map
}
/**
 * [TEST Testing if the customer location is out of defined range, it will throw an assertion]
 **/
TEST(carpoolingTest, add_start_Out_Test){
        Json::Reader reader;
        Json::Value request;
        std::string text = "{\"request\":[{\"name\":\"Elson\",\"start\":[2,3],\"end\":[3,18]},{\"name\":\"Peter\",\"start\":[2,3],\"end\":[3,8]}]}";
        if(!reader.parse(text,request))
                std::cout<<reader.getFormattedErrorMessages()<<std::endl;
        Carpooling_cloud_server cloudServer(10,10);    // the init_loc of the car is allready added to the places in constructor
        EXPECT_ANY_THROW(cloudServer.ReadRequest(request));
}
/**
 * [TEST Testing when the current car is not at the target node, then move to the target node with minimal turns]
 */
TEST(carpoolingTest, makeActionTestMove) {
        Location a(2,3);
        Location b(4,6);
        Carpooling_cloud_server cloudServer(10,10);   // the car was initialized at (0,0)
        Car car(b);
        Carpooling_cloud_server cloudServer2(car,10,10);
        cloudServer.makeAction(a,0);
        EXPECT_EQ(1,cloudServer.vehicle.v_loc.first);
        cloudServer2.makeAction(a,0);
        EXPECT_EQ(3,cloudServer2.vehicle.v_loc.first);
}
/**
 * [TEST Testing the case when the car reach the target node. It will make the correct action: pickup the passenger]
 */
TEST(carpoolingTest, makeActionTestPick) {
        Json::Reader reader;
        Json::Value request;
        std::string text = "{\"request\":[{\"name\":\"Elson\",\"start\":[2,3],\"end\":[3,8]},{\"name\":\"Peter\",\"start\":[4,5],\"end\":[7,1]}]}";
        if(!reader.parse(text,request))
                std::cout<<reader.getFormattedErrorMessages()<<std::endl;
        Car car({2,3}); // assume the car arrives at (2,3), where Elson is located
        Carpooling_cloud_server cloudServer(car,10,10);   // the init_loc of the car is allready added to the places in constructor
        cloudServer.ReadRequest(request); // Read the request
        cloudServer.add_start(10);
        Location a(2,3);
        cloudServer.makeAction(a,1); // so the car should pick Elson up
        EXPECT_EQ(2,cloudServer.names.size());
        EXPECT_EQ(3,cloudServer.places.size());
        EXPECT_EQ(2,cloudServer.pick_drop.size());
        EXPECT_EQ(1,cloudServer.vehicle.passenger.size());
}

/**
 * [TEST Testing if it deletes the vertex on the graph successfully]
 */
TEST(carpoolingTest, deleteVertexTest) {
        Json::Reader reader;
        Json::Value request;
        std::string text = "{\"request\":[{\"name\":\"Elson\",\"start\":[2,3],\"end\":[3,8]},{\"name\":\"Peter\",\"start\":[4,5],\"end\":[7,1]}]}";
        if(!reader.parse(text,request))
                std::cout<<reader.getFormattedErrorMessages()<<std::endl;
        Carpooling_cloud_server cloudServer(10,10);
        cloudServer.ReadRequest(request);        // Read the request
        cloudServer.add_start(10);
        cloudServer.deleteVertex(1);
        EXPECT_EQ(1,cloudServer.names.size());
        EXPECT_EQ(2,cloudServer.places.size());
        EXPECT_EQ(1,cloudServer.pick_drop.size());
        EXPECT_EQ(4,cloudServer.places[1].first);
        EXPECT_EQ(5,cloudServer.places[1].second);
}
/**
 * [TEST Testing if it adds the end node successfully when it reaches the start node]
 */
TEST(carpoolingTest, add_endTest) {
        Json::Reader reader;
        Json::Value request;
        std::string text = "{\"request\":[{\"name\":\"Elson\",\"start\":[2,3],\"end\":[3,8]},{\"name\":\"Peter\",\"start\":[4,6],\"end\":[1,9]}]}";
        if(!reader.parse(text,request))
                std::cout<<reader.getFormattedErrorMessages()<<std::endl;
        Carpooling_cloud_server cloudServer(10,10);
        cloudServer.ReadRequest(request); // Read the request
        cloudServer.add_start(10);
        cloudServer.add_end(1);
        EXPECT_EQ(3,cloudServer.names.size());
        EXPECT_EQ(4,cloudServer.places.size());
        EXPECT_EQ(3,cloudServer.pick_drop.size());
        EXPECT_EQ(3,cloudServer.places[3].first);
        EXPECT_EQ(8,cloudServer.places[3].second);
}
