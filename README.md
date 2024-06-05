# Mobile Robot Control System

This repository contains the source code and documentation for a Differential Robot Control System developed as part of the Mechatronics course at [UCB (Universidad Católica Boliviana)](https://www.scz.ucb.edu.bo). The project is designed to be used with the ESP32 DEVKIT V1 board.

## Table of Contents
- [Mobile Robot Control System](#mobile-robot-control-system)
  - [Table of Contents](#table-of-contents)
  - [PWM in motor\_control.c](#pwm-in-motor_controlc)
  - [PID Motor Controller](#pid-motor-controller)
    - [Usage](#usage)
  - [Forward Kinematics](#forward-kinematics)
    - [Usage](#usage-1)
    - [Detailed Description](#detailed-description)
  - [Inverse Kinematics](#inverse-kinematics)
    - [Usage](#usage-2)
  - [Odometry](#odometry)
  - [Overview](#overview)
  - [Contributors](#contributors)
  - [License](#license)

## PWM in motor_control.c

The PWM control for the motor is implemented in the `motor_control.c` file. In this file, the functions `ledc_set_duty` and `ledc_update_duty` are used to set and update the motor's duty cycle. Additionally, a function is provided to calculate the duty cycle based on an angle, and methods are defined to get and set the motor angle.

For more details, check the code in [`motor_control.c`](path/to/motor_control.c).

## PID Motor Controller

The PID motor controller is a fundamental part of the two-motor control functionality in a robot. It is implemented in the `task_utils.c` file, specifically in the `Motor1ControlTask` and `Motor2ControlTask` functions. These functions handle motor control and apply a PID controller to maintain the desired speeds.

### Usage

The process is developed as follows:

1. **Speed Reading**: The functions read the current motor speeds from a message queue.

2. **Speed Filtering**: They apply an Exponential Weighted Moving Average (EWMA) filter to smooth the speed readings.

3. **Speed Control**: Depending on the selected control strategy (either open-loop or PID control), the functions calculate the duty cycle value to control the motors.

4. **PID Controller**: If using the PID controller, they calculate the error, apply PID control, and saturate the control signal as necessary.

5. **Value Logging**: They log the current values, including motor speed, desired reference, control signal, and error.

6. **Motor Update**: Finally, they set the duty cycle for the motors according to the calculated signals and move accordingly.

These functions provide a solid foundation for implementing precise and efficient motor control, which is essential for the controlled movement of a robot.

## Forward Kinematics

Forward kinematics is implemented in the `task_utils.c` file through the `DirectKinematicsTask`. This task plays a crucial role in calculating a mobile robot's linear and angular velocity based on its wheel speeds. These values are logged or sent as needed.

### Usage

To utilize this functionality, it is essential to provide the necessary parameters to the task, including the motor speed queues. The constants `WHEEL_DIAMETER` and `ROBOT_WIDTH` should also be configured according to the robot's specific dimensions. The task runs in a continuous loop, periodically processing motor speed data and updating the robot's speed state.

### Detailed Description

This code implements forward kinematics for a mobile robot, a calculation that relates the robot's wheel speeds to its real-world linear and angular velocity. The task operates as follows:

1. **Speed Reading**: The task reads the left and right wheel speeds in revolutions per minute (rpm) from the `motor1SpeedQueue` and `motor2SpeedQueue` message queues.

2. **Unit Conversion**: It converts the wheel speeds from rpm to

radians per second (rad/s).

3. **Velocity Calculation**: It calculates the robot's linear and angular velocities using the wheel speeds and parameters like the wheel diameter (`WHEEL_DIAMETER`) and the robot width (`ROBOT_WIDTH`).

4. **State Update**: It sets the robot's linear and angular velocity states.

5. **Result Logging or Sending**: It logs or sends the results, including the robot's linear and angular velocity, for further monitoring or control.

6. **Frequency Control**: The task loop runs with a controlled frequency, regulated by a 100 ms delay (`pdMS_TO_TICKS(100)`), to ensure consistent task execution and state updates.

This approach provides an effective way to obtain the forward kinematics of a mobile robot, which is essential for a wide variety of robotics applications.

## Inverse Kinematics

Inverse kinematics, a crucial part of mobile robot control, is implemented in the `http_handlers.c` file, specifically in the `handle_set_robot_speed` function. This function handles requests to update the robot's speed and calculates the left and right wheel speeds using inverse kinematics equations.

### Usage

The process is as follows:

1. **Request Reception**: The function checks that the request is of type POST.

2. **JSON Data Extraction**: It then extracts JSON data from the request, including the robot speed (`robot_speed`) and the robot angle (`robot_angle`).

3. **Wheel Speed Calculation**: Using inverse kinematics, the function calculates the left and right wheel speeds in RPM based on the robot's speed and angle.

4. **Speed Limiting**: It applies speed saturation to limit the wheel speeds to a maximum of 200 RPM.

5. **Setting References**: It sets the references for the left and right motor speeds according to the selected control strategy.

6. **Logging Updated Values**: It logs the updated motor speed values.

7. **Response**: Finally, it sends a response indicating that the robot's speed has been successfully updated.

This code provides a robust foundation for implementing inverse kinematics in mobile robot control applications, which is essential for achieving precise and efficient movement control.

## Odometry

The odometry functionality is implemented in the `OdometryTask` task in the `task_utils.c` file. In this task, the robot's positions (x, y) and orientation (theta) are calculated based on the motor speeds and wheel dimensions. These calculations are performed using the robot's kinematic equations and motor speeds.

For more details, check the code in [`task_utils.c`](path/to/task_utils.c).

## Overview

The Differential Robot Control System is designed to provide control and monitoring capabilities for a mobile robot. It includes features such as Wi-Fi connectivity, a web server for remote control and monitoring, encoder initialization, GPIO pin management, timer initialization, interrupt handling, and motor control. This codebase serves as a foundation for building and customizing mobile robot projects.

## Contributors

- [Leonardo Acha Boiano](https://github.com/leonardoAB1)
- [Bruno Ramiro Rejas](https://github.com/BrunoRRM712)
- [Gonzalo Peralta]()
- [Andrés Ayala](https://github.com/mecatrono)

## License

This project is licensed under the [MIT License](LICENSE).