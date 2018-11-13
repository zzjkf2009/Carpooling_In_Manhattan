#include "carpooling_cloud_server.h"

int main(){
        Json::Reader reader;
        Json::Value request;
        std::string json_text1 = "{\"request\":[{\"name\":\"Elson\",\"start\":[2,3],\"end\":[3,8]},{\"name\":\"George\",\"start\":[8,3],\"end\":[4,5]}]}";
        std::string json_text2 = "{\"request\":[{\"name\":\"Mike\",\"start\":[2,3],\"end\":[6,2]},{\"name\":\"Peter\",\"start\":[0,3],\"end\":[4,0]}]}";
        Carpooling_cloud_server cloudServer(10,10); // The car is initialized at location (0,0)

        // The location of the car also be initialized manually as :
        // Car car({4,5});
        // Carpooling_cloud_server cloudServer(car,10,10);

        int step = 0;
        while(step < 100) { // Assume we step the time(step) limit for this carpooling
                if(step == 2) // a request is reveived at 2 (step time), this time can be adjusted manually to any time you want
                        reader.parse(json_text1,request);
                else if(step == 6)  // an other request is reveived at 6 (step time),this time can be adjusted manually to any time you want
                        reader.parse(json_text2,request);
                cloudServer.carpooling(request);
                step++;
                request.clear();
        }
        std::cout << "Task finished, All passenger delivered" << '\n';
}
