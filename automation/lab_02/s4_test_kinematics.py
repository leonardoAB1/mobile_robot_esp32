import requests
import json
import matplotlib.pyplot as plt
import datetime
import os
import csv
import numpy as np
import time


NUM_DATA_POINTS = 100 # Number of data points to collect
ALPHA = 0.1 # Constantfor ewma filter
TIME_INTERVAL = 0.5  # Time interval
URL = '192.168.0.188'

# Create a directory for saving data files
data_directory = "data"
os.makedirs(data_directory, exist_ok=True)

# Function to generate a filename with datetime
def generate_data_filename():
    current_time = datetime.datetime.now()
    filename = current_time.strftime(f"%Y%m%d%H%M%S_robot_kinematics.csv")
    return os.path.join(data_directory, filename)

# Function to save data to a text file
def save_data_to_file(column_names, data, filename):
    with open(filename, 'w', newline='') as file:
        csv_writer = csv.writer(file)
        csv_writer.writerow(column_names)
        csv_writer.writerows(data)

# Set robot speed and angle via Wi-Fi
def set_robot_speed_and_angle(speed, angle):
    req_url = f"http://{URL}/robot/speed/set"
    headers = {
        "Accept": "*/*",
        "User-Agent": "Thunder Client (https://www.thunderclient.com)",
        "Content-Type": "application/json"
    }
    payload = json.dumps({
        "robot_speed": speed,
        "robot_angle": angle
    })
    response = requests.post(req_url, data=payload, headers=headers)
    return response.text

# Get robot speed and angle from Wi-Fi
def get_robot_speed_and_angle():
    req_url = f"http://{URL}/robot/speed/get"
    headers = {
        "Accept": "*/*",
        "User-Agent": "Thunder Client (https://www.thunderclient.com)"
    }
    response = requests.get(req_url, headers=headers)
    return response.json()  # Parse response as JSON

def stop_robot():
    for i in range(1, 3):
        reqUrl = f"http://{URL}/reference{i}/set"

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

        response = requests.request("POST", reqUrl, data=payload,  headers=headersList)

        print(response.text)

def ewma_filter(data, alpha=0.2):
    filtered_data = [data[0]]  # Initialize the filtered data with the first value

    for i in range(1, len(data)):
        filtered_value = alpha * data[i] + (1 - alpha) * filtered_data[-1]
        filtered_data.append(filtered_value)

    return filtered_data

# Initialize lists to store data for plotting
#ref_lineal_speed = np.linspace(0, 0.5, NUM_DATA_POINTS)
#ref_angular_speed = np.linspace(0, 2 * np.pi, NUM_DATA_POINTS)
ref_lineal_speed = 0.3*np.ones(NUM_DATA_POINTS)
ref_angular_speed = (0)*np.ones(NUM_DATA_POINTS)
robot_speed_values = []
robot_angle_values = []
error_lineal_speed = []
error_angular_speed = [] 
tiempo = [] 

time_delta = 0.0
last_time  = time.time()

for i in range(NUM_DATA_POINTS):
    # Get the current time
    current_time = time.time()

    # Set robot speed and angle (adjust values as needed)
    set_robot_speed_and_angle(ref_lineal_speed[i], ref_angular_speed[i])

    time_delta += current_time - last_time
    last_time = current_time

    # Get robot speed and angle
    data = get_robot_speed_and_angle()

    # Extract speed and angle values
    robot_speed = data["robotSpeed"]
    robot_angle = data["robotAngularSpeed"]

    # Append data to lists
    robot_speed_values.append(robot_speed)
    robot_angle_values.append(robot_angle)
    tiempo.append(time_delta)

    # Introduce a delay to wait for the next data point
    time.sleep(TIME_INTERVAL)

# Filter values
robot_speed_values = ewma_filter(robot_speed_values, ALPHA)
robot_angle_values = ewma_filter(robot_angle_values, ALPHA)

# Calculate error in angular speed and lineal speed
error_lineal_speed = np.array(ref_lineal_speed) - np.array(robot_speed_values)
error_angular_speed = np.array(ref_angular_speed) - np.array(robot_angle_values)

# Stop the robot
stop_robot()

# Save robot data to a CSV file
data_to_save = list(zip(robot_speed_values, robot_angle_values, ref_lineal_speed, ref_angular_speed, error_lineal_speed, error_angular_speed, tiempo))
column_names = ["Robot Speed", "Robot Angle", "Lineal Speed Reference", "Angular Speed Reference", "Error in Lineal Speed", "Error in Angular Speed", "Tiempo"]
filename = generate_data_filename()
save_data_to_file(column_names, data_to_save, filename)

# Create subplots
plt.figure(figsize=(8, 6))

# Plot robot speed
plt.subplot(2, 2, 1)
plt.plot(tiempo, robot_speed_values, label="Robot Speed", linewidth=2)
plt.plot(tiempo, ref_lineal_speed, label="Reference", color="r", linewidth=2)
plt.ylabel("Lineal Speed (m/s)")
plt.legend()
plt.grid()

# Plot robot angle
plt.subplot(2, 2, 2)
plt.plot(tiempo, robot_angle_values, label="Robot Angle", linewidth=2)
plt.plot(tiempo, ref_angular_speed, label="Reference", color="r", linewidth=2)
plt.ylabel("Angular Speed (rad/s)")
plt.legend()
plt.grid()

# Plot error in lineal speed
plt.subplot(2, 2, 3)
plt.plot(tiempo, error_lineal_speed, label="Error", linewidth=2)
plt.xlabel("Time(s)")
plt.ylabel("Error")
plt.grid()

# Plot error in angular speed
plt.subplot(2, 2, 4)
plt.plot(tiempo, error_angular_speed, label="Error", linewidth=2)
plt.xlabel("Time(s)")
plt.ylabel("Error")
plt.grid()

plt.tight_layout()
plt.show()
