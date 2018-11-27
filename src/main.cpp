#include "carpooling_cloud_server.h"

int main(){
        // Create or load json files
        Json::Reader reader;
        Json::Value request;
        std::string json_text1 = "{\"request\":[{\"name\":\"Elson\",\"start\":[2,3],\"end\":[3,8]},{\"name\":\"George\",\"start\":[8,3],\"end\":[4,5]}]}";
        std::string json_text2 = "{\"request\":[{\"name\":\"Mike\",\"start\":[2,3],\"end\":[6,2]},{\"name\":\"Peter\",\"start\":[0,3],\"end\":[4,0]}]}";

        // Set the city as a 10 by 10 grid. The car is initialized at location (0,0) by default if no init location is provided.
        Carpooling_cloud_server cloudServer(10,10);

        /*
           // The location of the car also can be initialized manually, for example :

           Car car({4,5});
           Carpooling_cloud_server cloudServer(car,10,10);
         */

        int step = 0;
        while(step < 100) {
                if(step == 2) // The first request was received at the second step time
                        reader.parse(json_text1,request);
                else if(step == 6) // Another request was received at the 6th step time
                        reader.parse(json_text2,request);
                cloudServer.carpooling(request);
                step++;
                request.clear();
        }
        std::cout << "Task finished, All passenger delivered" << '\n';
}
