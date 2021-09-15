from pyphysx_envs.envs import ToolEnv, RobotEnv
import numpy as np
from os import path
import pickle
from matplotlib import pyplot as plt

# TODO: insert path to joint_position.bin
path_to_joints = "/home/ondra/Plocha/CIIRC/franka_exp/exp_7/joint_position.bin"

x = np.fromfile(path_to_joints, dtype=np.double)

seconds = 8 # TODO: insert amount of seconds you gathered the data
record = 50 # TODO: insert number (every 50th dataset I take)

dataset = seconds * 1000 // 50

joint_values = np.zeros((dataset, 7))

for i in range(dataset):
    joint_values[i, :] = x[i*7:(i+1)*7]

plt.plot(joint_values, marker='x')
plt.title("Joint values in time")
plt.show()


tool_name = 'spade'
robot_name = 'panda'
n_dof = 7
rate = 20

"""
# TODO: read joint_values from file
# dummy data, joint value in range from 0 to ~pi/2
#dummy_per_joint = np.linspace(0., 1.57, num=100)
#joint_values = np.zeros((100, n_dof))
#for j in range(n_dof):
#    joint_values[:, j] = dummy_per_joint
"""
demo_params = {'goal_box_position': [0.27, -0.23, 0.],
               'sand_buffer_position': [0.58, 0.29, 0.],
               'sand_buffer_yaw': 1.57}


env = RobotEnv(scene_name=tool_name, tool_name=tool_name, robot_name=robot_name,
               rate=rate,
               obs_add_q=True,
               render=True,
               add_spheres=True,
               robot_pose = ((0., 0., -0.18), ),
               demonstration_q=joint_values,
               spade_mesh_path=path.join(path.dirname(path.dirname(__file__)), 'data/spade_mesh.obj'),
               params=demo_params,
               render_dict=dict(
                   use_meshcat=True, open_meshcat=True, wait_for_open=True, render_to_animation=True,
                   animation_fps=rate,
               )
               )
env.reset()
for i in range(joint_values.shape[0] - 1):
    velocity = (joint_values[i + 1] - joint_values[i]) * rate
    _, r, _, _ = env.step(velocity)

env.renderer.publish_animation()
