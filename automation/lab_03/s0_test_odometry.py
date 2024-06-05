import requests
import json
import csv
import matplotlib.pyplot as plt
import time
import datetime
import os
from math import pi

# Robot API URL
robot_url = "http://192.168.91.158"

# Function to set the robot speed and angle
def set_robot_speed(speed, angle):
    reqUrl = f"{robot_url}/robot/speed/set"
    
    headersList = {
        "Accept": "*/*",
        "User-Agent": "Thunder Client (https://www.thunderclient.com)",
        "Content-Type": "application/json"
    }
    
    payload = json.dumps({
        "robot_speed": speed,
        "robot_angle": angle
    })
    
    response = requests.request("POST", reqUrl, data=payload, headers=headersList)
    
    print(response.text)

# Function to stop the robot
def stop_robot():
    for i in range(1, 3):
        reqUrl = f"{robot_url}/reference{i}/set"
        
        headersList = {
            "Accept": "*/*",
            "User-Agent": "Thunder Client (https://www.thunderclient.com)",
            "Adc-Resolution": "10",
            "Controller": "DEFAULT",
            "Content-Type": "application/json"
        }
        
        payload = json.dumps({
            "referenceSignal": 0
        })
        
        response = requests.request("POST", reqUrl, data=payload, headers=headersList)
        
        print(response.text)

# Function to get the current position of the robot
def get_robot_position():
    reqUrl = f"{robot_url}/robot/distance/get"
    
    headersList = {
        "Accept": "*/*",
        "User-Agent": "Thunder Client (https://www.thunderclient.com)"
    }
    
    payload = ""
    
    response = requests.request("GET", reqUrl, data=payload, headers=headersList, timeout=10)
    
    return response.json()

# Function to save data to a CSV file
def save_data_to_file(column_names, data, filename):
    if not os.path.isfile(filename):
        # If the file does not exist, write the column names
        with open(filename, 'w', newline='') as file:
            csv_writer = csv.writer(file)
            csv_writer.writerow(column_names)

    # Append the data to the file
    with open(filename, 'a', newline='') as file:
        csv_writer = csv.writer(file)
        csv_writer.writerows(data)

# Function to generate a filename with datetime
def generate_data_filename():
    filename = f"robot_positions.csv"
    return os.path.join("data", filename)

if __name__ == "__main__":
    # Set robot speed and angle
    linear_speed=0.5 #m/s
    angular_speed=pi/12  #radians
    set_robot_speed(linear_speed, angular_speed)
    start_time = time.time()  # Record the start time
    
    # Wait for 5 seconds
    time.sleep(2)
    
    # Stop the robot
    stop_robot()
    end_time = time.time()  # Record the end time
    
    # Get the robot's initial position
    initial_position_data = get_robot_position()
    print("Initial Robot Position:", initial_position_data)
    
    # Prompt the user for the actual distance the robot moved
    actual_distance = float(input("Enter the actual distance the robot moved: "))
    
    # Get the robot's final position
    final_position_data = get_robot_position()
    
    # Calculate the difference in positions
    position_difference = {
        "robot_x": final_position_data["robot_x"] - initial_position_data["robot_x"],
        "robot_y": final_position_data["robot_y"] - initial_position_data["robot_y"],
        "robot_theta": final_position_data["robot_theta"] - initial_position_data["robot_theta"]
    }

    print("Final Robot Position:", position_difference)

    calculated_distance = (position_difference["robot_x"]**2+position_difference["robot_y"]**2)**0.5
    distance_error = abs(calculated_distance-actual_distance)
    
    # Calculate the time delta
    time_delta = end_time - start_time
    
    # Save data to a CSV file
    column_names = ["robot_x", "robot_y", "robot_theta", "time_delta", "commanded_speed", "commanded_angle", "calculated_distance", "actual_distance", "distance_error"]
    data = [
        (position_difference["robot_x"], position_difference["robot_y"], position_difference["robot_theta"],
         time_delta, linear_speed, angular_speed, calculated_distance, actual_distance, distance_error)
    ]
    filename = generate_data_filename()
    save_data_to_file(column_names, data, filename)
