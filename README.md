# Recording joint positions
These two files should be enough for you to record the joint positions and start the animation.
1. First of all what you want to do is to connect to Panda robot.

        ssh ros@192.168.102.21

Once you are ensured, that you are connected, go back to your home terminal.

2. Second step is to copy there file record_states.cpp

        scp record_states.cpp ros@192.168.102.21:/home/ros/franka_panda_spade/libfranka/examples/echo_robot_state.cpp

3. Now you should have your file copied there, what you want to do, is to connect there and go to this exact folder:

        ssh ros@192.168.102.21
        cd /home/ros/franka_panda_spade/libfranka/examples/build

4. Once you are in that folder, you need to compile the .cpp code, you can do that with the simple command:

        make

5. Then after compiling you can easily run the command and record the states.

        ./echo_robot_state 192.168.102.11

This will save three files in build folder: 'joint_position.bin', 'force.bin', 'times.bin'

6. Once the states are recorded, go to your terminal ($ exit) and then copy the file "joint_position.bin' to your desired location.

        scp ros@192.168.102.21:/home/ros/franka_panda_spade/libfranka/examples/build/joint_position.bin <path/to/desired/location>

7. Then you can launch the 06_follow_joint_val_sequence.py, before you do that, check the line 9, 13, 14 in  06_follow_joint_val_sequence.py. On line 9 there should be path to joint_position.bin entered, on line 13 you should enter amount of seconds you recorded the joint positions and on line 14 there should be number, from how many records you select one (initially there is 50, that means with the frequence of 1kHz, that you record 1000 records and you select every 50th - which means you will have 20 records per second). The 06_follow_joint_val_sequence.py needs to be launched from the github repo: concretely you should copy it here:

        cp 06_follow_joint_val_sequence.py <path/to/repo>/pyphysx_envs/pyphysx_envs/examples/

8. Then you start it:

        python 06_follow_joint_val_sequence.py

# Saving the animation

If you want to save the animation, you can record it (top left corner) and then save the .png, once you untar them, use this command to make a video from them:

        ffmpeg -r 60 -i %07d.png -vcodec libx264 -preset slow -crf 18 output.mp4

    