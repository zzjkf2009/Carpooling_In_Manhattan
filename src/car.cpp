/**
 * @Author: Zejiang Zeng <yzy>
 * @Date:   2018-11-07T17:19:13-05:00
 * @Email:  zzeng@terpmail.umd.edu
 * @Filename: car.cpp
 * @Last modified by:   yzy
 * @Last modified time: 2018-11-07T17:20:28-05:00
 * @Copyright: (C) 2017 Zejiang Zeng - All Rights Reserved
 * You may use, distribute and modify this code under the
 * terms of the BSD license.
 */
#include "car.h"

/**
 * [Car::Car Constructor, init the car location to (0,0)]
 */
Car::Car() : n_passengers(0){
        v_loc.first = 0; v_loc.second = 0;
}
/**
 * [Car::Car Constructor, init the car location to input location]
 */
Car::Car(Location init_loc) : v_loc(init_loc),n_passengers(0){

}
/**
 * [car::moveOneStep Let the car move one step forward,+-1 for moving to horizantal,+-2 for moving verticle, 0 for standing still.Other nubers are not accpect
 *  the location of the car will change accordingly]
 * @param toHere [direction of moving]
 * @param v_loc  [Location of the car]
 */
void Car::moveOneStep(const int &Direction){
        switch(Direction) {
        case 1: v_loc.first++;
                break;
        case -1: v_loc.first--;
                break;
        case 2: v_loc.second++;
                break;
        case -2: v_loc.second--;
                break;
        case 0: break;
        default: throw("+-1 for moving to horizantal,+-2 for moving verticle, 0 for standing still.Other nubers are not accpect");
                break;
        }

}
