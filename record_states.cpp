// Copyright (c) 2017 Franka Emika GmbH
// Use of this source code is governed by the Apache-2.0 license, see LICENSE
#include <iostream>
#include <stdio.h>
#include <fstream>
using namespace std;

#include <franka/exception.h>
#include <franka/robot.h>
#include <franka/duration.h>

#define NO_JOINTS 7   // number of joints (that won't probably change)
#define SECONDS 8     // number of seconds you want to record the motion
#define RECORD 50     // every <record> records I want the state to be put in final file

/**
 * @example echo_robot_state.cpp
 * An example showing how to continuously read the robot state.
 */


int main(int argc, char** argv) {
  if (argc != 2) {
    cerr << "Usage: " << argv[0] << " <robot-hostname>" << std::endl;
    return -1;
  }

  try {
    franka::Robot robot(argv[1]);

    size_t count = 0;
 
    double *joint_pos = (double *)malloc(sizeof(double)*SECONDS*1000*NO_JOINTS);
    // joint positions array

    //TODO: uncomment this if you want to know velocity of joints
    //double *joint_vel = (double*)malloc(sizeof(double)*SECONDS*1000*NO_JOINTS);
    //joint velocity

    double *force = (double *)malloc(sizeof(double*)*SECONDS*1000*3);
    // forces array

    uint64_t *times = (uint64_t *)malloc(sizeof(uint64_t)*1000*SECONDS);
    // times array

    // robot.read([&counts, &joint_pos, &times, &joint_vel, &force](const franka::RobotState& robot_state){
    robot.read([&count, &joint_pos, &times, &force](const franka::RobotState& robot_state) {
      //cout << robot_state << endl;
      for(int i = 0; i < NO_JOINTS; i++){
        joint_pos[count*NO_JOINTS+i] = robot_state.q[i];
        times[i] = robot_state.time.toMSec();
      }
      force[count*3] = robot_state.O_F_ext_hat_K[0];
      force[count*3 + 1] = robot_state.O_F_ext_hat_K[1];
      force[count*3 + 2] = robot_state.O_F_ext_hat_K[2];
      return count++ < SECONDS*1000;
    });


    double real_pos[count/RECORD][7];
    double real_times[count/RECORD];
    double real_force[count/RECORD][3];
    //double real_vel[count/RECORD][7]
    for(int i = 0; i < count/RECORD; i++){
      for(int j = 0; j < 7; j++){
        real_pos[i][j] = joint_pos[i*RECORD*7 + j];
        //real_vel[i][j] = joint_vel[i*RECORD*7 + j];
      }
      real_times[i] = times[i*RECORD];

      for(int k = 0; k < 3; k++){
        real_force[i][k] = force[i*RECORD*3 + k];
      }
    }

    union serialize{
      double pos;
      uint8_t array[sizeof(double)];
    }serialize; // this union is because we need to write double to binary file - so we need bytes

    ofstream my_file;
    my_file.open ("joint_position.bin", ios::out | ios::binary);

    for(int j = 0; j < int(count/RECORD); j++){
      for(int k = 0; k < 7; k++){
        serialize.pos = real_pos[j][k];
        for(int i = 0; i <sizeof(double);i++) 
            my_file << serialize.array[i];
      }
    }
    my_file.close();

    union time_ser{
      uint64_t pos;
      uint8_t array[sizeof(uint64_t)];
    }time_ser;  //this union is for times - there are not double, but uint64

    my_file.open ("times.bin", ios::out | ios::binary);
    for(int j = 0; j < int(count/RECORD); j++){
      time_ser.pos = real_times[j];
      for(int i = 0; i < sizeof(double);i++)
          my_file << time_ser.array[i];
    }
    my_file.close();

    my_file.open("force.bin", ios::out | ios::binary);
    for(int i = 0; i < int(count/RECORD); i++){
      for(int j = 0; j < 3; j++){
        serialize.pos = real_force[i][j];
        for(int l = 0; l < sizeof(double); l++){
          my_file << serialize.array[l];
        }
      }
    }
    my_file.close();

    /*
    my_file.open("velocity.bin", ios::out| ios::binary);
    for(int j = 0; j < count/RECORD; j++){
      for(int k = 0; k < 7; k++){
        serialize.pos = real_vel[j][k];
        for(int i = 0; i <sizeof(double);i++)
            my_file << serialize.array[i];
      }
    }
     my_file.close()
     */

    free(joint_pos);
    free(times);
    free(force);
    //free(joint_vel);
    

    cout << "Done." << endl;
  } catch (franka::Exception const& e) {
    cout << e.what() << endl;
    return -1;
  }

  return 0;
}
