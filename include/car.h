/**
 * @Author: Zejiang Zeng <yzy>
 * @Date:   2018-11-12T14:36:21-05:00
 * @Email:  zzeng@terpmail.umd.edu
 * @Filename: car.h
 * @Last modified by:   yzy
 * @Last modified time: 2018-11-12T14:38:27-05:00
 * @Copyright: (C) 2017 Zejiang Zeng - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the BSD license.
 */

#ifndef CAR_H
#define CAR_H

 #include <iostream>
 #include <utility>
 #include <unordered_set>
 #include <string>
using namespace std;

typedef pair< int, int> Location;

struct Car {
        Car();
        Car(Location init_loc);
        unsigned int n_passangers;
        Location v_loc;
        std::unordered_set<std::string> passanger;
        void moveOneStep(const int &Direction);
};

#endif
