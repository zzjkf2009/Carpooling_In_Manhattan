# Carpooling Algorithm for Manhattan
# Overview 
Ridesharing is an important component of sustaubable urban transportation as it increase vehicle utiliztion while reducing polution. By sharing rides, drivers offer free seats in their vehicles to passangers who want to travel in same directions [1]. Based on this reaseon, a carpooling algorithm for ride-hailing companies like Uber and Lyft is designed, aimming to work in cities like Manhattan (cities are made of perpendicular blocks). The routing proble for companies, like Uber, is to build a ridesharing route for a group of vehicles tht minimizes th cost of servicing all the passengers, while passangers split the fuel bill or benefit from catpool lanes. Therefore, prior arranfements are necessary to make a rideshare work [1]. In reallity, the algorithm may consider more variables, such as live traffic at specific times, drivers rating, customer rating, surge pricing etc. Although, a more flexible enhancement of ridesharing, danamic ridesharing, has already implemented by Uber[2] [1]. For this coding challage, it more focuses on the traditional ride-matching process that passengers traveling from fixed departure and destination locations and assume all the traffic are the same for each road. And more details about theis coding challage can be found [here](https://docs.google.com/document/d/180vpmeSb3_fxncKhpiJjBPlwsk3cBH6_SobPsLNZV1c/edit).
## Strategy
For solving this problem, a variant of [Travel Salesman Problem](https://en.wikipedia.org/wiki/Travelling_salesman_problem) is proposed using Dynamic programming, let's call the algorithm VTSP (variant travel salesman problem) For each request reviced, the start nodes will be added to the graph, and the VTSP will find the shortest possible route that visits each node once without returning back to original node. When it find the target node to move, if the current location is not at the target location, it will move one step toward to this node (minimal turns), until it reach the target. Once it got the target node (pick the passanger), this start node will be delete and the end node of this passanger will be add to the graph accordingly. If this node is for dropping the passanger, this end node will be delete from the graph as well. A new shortest path will be calculate for every step of action (move/drop/pick) in case of receiving new request or picking/dropping passangers. 
## Prerequisite
### Google test
googletest is a testing framework developed by the Testing Technology team with Google's specific requirements and constraints in mind. It provides all kinds of test. In this program, google test is used for unit testing. See more details about google test [google primer](https://github.com/google/googletest/blob/master/googletest/docs/primer.md) and [advanced topics](https://github.com/google/googletest/blob/master/googletest/docs/advanced.md). 

### Jsoncpp 
JSON (JavaScript Object Notation) is a lightweight data-interchange format. It can represent numbers, strings, ordered sequences of values, and collections of name/value pairs. To install JsonCpp library on Ubuntu:
```
sudo apt-get install libjsoncpp-dev
```
See more about how to use Jsoncpp [here](https://github.com/nlohmann/json).
## Build and Run
To build it:
```
git clone 
cd <directory>
mkdir build
cd build 
cmake ..
make
```
To run it
```
./src/carpooling
```
To run the unit test
```
./test/cpp-test
```
## Reuslt
There is the reult for reveiving two requests, check request1.json and request2.json for content. The first request was received at 2 (step time) and the second was reveived at 6 (step time). The result was showed in the following figure.
- ![](https://github.com/zzjkf2009/Carpooling_In_Manhatton/blob/master/CarpoolingResult.png)
The unit tests result is showed below:
- ![](https://github.com/zzjkf2009/Carpooling_In_Manhatton/blob/master/CarpoolingTest.png)

## Reference
- [1] [A Matching Algorithm for Dynamic Ridesharing](https://ac.els-cdn.com/S2352146516308730/1-s2.0-S2352146516308730-main.pdf?_tid=15380348-1de5-48fa-88f2-438a96625c2f&acdnat=1541361850_9e2f8f83ef5387ebdf9cf816ea90cee3)
- [2 ] Uber Update it Uberpool algorithm, see the  [News](https://techcrunch.com/2017/05/22/uber-debuts-a-smarter-uberpool-in-manhattan/)
