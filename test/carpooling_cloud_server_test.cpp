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
 * [TEST Testing if the init car location is out of range, then it will throw assertions]
 */
TEST(carpoolingTest,constructorTest){
        Car car({12,32});
        EXPECT_ANY_THROW(Carpooling_cloud_server cloudServer(car,10,10));

}

/**
 * [TEST Testing the mahanttan distance between two vertex (location)]
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
 * [TEST Testing if the distance grid(skew matrix) is correctly built by giving the distance the vector of the location of each node]
 */
TEST(carpoolingTest, buildGridsTest) {
        Location a(2,3);
        Location b(4,6);
        Location c(1,7);
        Carpooling_cloud_server cloudServer(10,10);   // the init_loc of the car is allready added to the places in constructor
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
 * [TEST Testing if the TravelSalesMan solver can find the shortest path for traveling each node once]
 */
TEST(carpoolingTest, TSPTest) {
        Location a(2,3);
        Location b(4,6);
        Location c(0,7);
        Carpooling_cloud_server cloudServer(10,10);   // the init_loc of the car is allready added to the places in constructor
        cloudServer.places.push_back(b);
        cloudServer.places.push_back(a);
        cloudServer.places.push_back(c);
        cloudServer.buildGrids();
        EXPECT_EQ(2, cloudServer.TravelSalesMan(1,0,cloudServer.initNode));
}
/**
   *[TEST Testing reading the request in JSON]
 */
TEST(carpoolingTest, add_startTest){
        Json::Reader reader;
        Json::Value request;
        std::string text = "{\"request\":[{\"name\":\"Elson\",\"start\":[2,3],\"end\":[3,8]},{\"name\":\"Peter\",\"start\":[2,3],\"end\":[3,8]}]}";
        if(!reader.parse(text,request))
                std::cout<<reader.getFormattedErrorMessages()<<std::endl;
        Carpooling_cloud_server cloudServer(10,10);          // the init_loc of the car is allready added to the places in constructor
        cloudServer.add_start(request);
        EXPECT_EQ(2,cloudServer.names.size()); // add two names
        EXPECT_EQ(3,cloudServer.places.size()); // including the current location
        EXPECT_EQ(2,cloudServer.pick_drop.size()); // add two locations mark as start point
        EXPECT_EQ(2,cloudServer.startToend.size()); // add two pair<name, end> to map
}
/**
 * [TEST Testing if the customer location is out of defined range, it will throw an assertion]
 */
TEST(carpoolingTest, add_start_Out_Test){
        Json::Reader reader;
        Json::Value request;
        std::string text = "{\"request\":[{\"name\":\"Elson\",\"start\":[2,3],\"end\":[3,18]},{\"name\":\"Peter\",\"start\":[2,3],\"end\":[3,8]}]}";
        if(!reader.parse(text,request))
                std::cout<<reader.getFormattedErrorMessages()<<std::endl;
        Carpooling_cloud_server cloudServer(10,10);    // the init_loc of the car is allready added to the places in constructor
        EXPECT_ANY_THROW(cloudServer.add_start(request));
}
/**
 * [TEST Testing if the current car lcoation is not at ant node, then move to desired node with minimal turns]
 */
TEST(carpoolingTest, makeActionTestMove) {
        Location a(2,3);
        Location b(4,6);
        Carpooling_cloud_server cloudServer(10,10);   // without the input car, it will generate a car at (0,0)
        Car car(b);
        Carpooling_cloud_server cloudServer2(car,10,10);
        cloudServer.makeAction(a,0);
        EXPECT_EQ(1,cloudServer.vhicle.v_loc.first);
        cloudServer2.makeAction(a,0);
        EXPECT_EQ(3,cloudServer2.vhicle.v_loc.first);
}
/**
 * [TEST Testing if the current car location is at goal node, then check if it is pick node or drop node, if ir is pick node then add this passanger]
 */
TEST(carpoolingTest, makeActionTestPick) {
        Json::Reader reader;
        Json::Value request;
        std::string text = "{\"request\":[{\"name\":\"Elson\",\"start\":[2,3],\"end\":[3,8]},{\"name\":\"Peter\",\"start\":[4,5],\"end\":[7,1]}]}";
        if(!reader.parse(text,request))
                std::cout<<reader.getFormattedErrorMessages()<<std::endl;
        Car car({2,3}); // assume the car arrives at (2,3), where Elson is located
        Carpooling_cloud_server cloudServer(car,10,10);   // the init_loc of the car is allready added to the places in constructor
        cloudServer.add_start(request);
        Location a(2,3);
        cloudServer.makeAction(a,1); // so the car should pick Elson up
        EXPECT_EQ(2,cloudServer.names.size());
        EXPECT_EQ(3,cloudServer.places.size());
        EXPECT_EQ(2,cloudServer.pick_drop.size());
        EXPECT_EQ(1,cloudServer.vhicle.passanger.size());
}

/**
 * [TEST Testing if it sucessfully delete the vertex on the graph]
 */
TEST(carpoolingTest, deleteVertexTest) {
        Json::Reader reader;
        Json::Value request;
        std::string text = "{\"request\":[{\"name\":\"Elson\",\"start\":[2,3],\"end\":[3,8]},{\"name\":\"Peter\",\"start\":[4,5],\"end\":[7,1]}]}";
        if(!reader.parse(text,request))
                std::cout<<reader.getFormattedErrorMessages()<<std::endl;
        Carpooling_cloud_server cloudServer(10,10); // the init_loc of the car is allready added to the places in constructor
        cloudServer.add_start(request);
        cloudServer.deleteVertex(1);
        EXPECT_EQ(1,cloudServer.names.size()); // add two names
        EXPECT_EQ(2,cloudServer.places.size()); // including the current location
        EXPECT_EQ(1,cloudServer.pick_drop.size()); // add two locations mark as start point
        EXPECT_EQ(4,cloudServer.places[1].first);
        EXPECT_EQ(5,cloudServer.places[1].second);
}
/**
 * [TEST Testing if it is successfully add the end location when it reach the start node]
 */
TEST(carpoolingTest, add_endTest) {
        Json::Reader reader;
        Json::Value request;
        std::string text = "{\"request\":[{\"name\":\"Elson\",\"start\":[2,3],\"end\":[3,8]},{\"name\":\"Peter\",\"start\":[4,6],\"end\":[1,9]}]}";
        if(!reader.parse(text,request))
                std::cout<<reader.getFormattedErrorMessages()<<std::endl;
        Carpooling_cloud_server cloudServer(10,10);  // the init_loc of the car is allready added to the places in constructor
        cloudServer.add_start(request);
        cloudServer.add_end(1);
        EXPECT_EQ(3,cloudServer.names.size()); // add two names
        EXPECT_EQ(4,cloudServer.places.size()); // including the current location
        EXPECT_EQ(3,cloudServer.pick_drop.size()); // add two locations mark as start point
        EXPECT_EQ(3,cloudServer.places[3].first);
        EXPECT_EQ(8,cloudServer.places[3].second);
}
