import serial
import time
import re
import os
import datetime
import matplotlib.pyplot as plt

# Configure the serial port (replace 'COMX' with your actual serial port)
serial_port = serial.Serial('COM5', baudrate=115200, timeout=1)

# Create a directory for saving data files
data_directory = "data"
os.makedirs(data_directory, exist_ok=True)

# Function to generate a filename with datetime
def generate_data_filename(motor):
    current_time = datetime.datetime.now()
    filename = current_time.strftime(f"%Y%m%d%H%M%S_motor{motor}.txt")
    return os.path.join(data_directory, filename)

# Function to save data to a text file
def save_data_to_file(data, filename):
    with open(filename, "a") as file:
        file.write(data + '\n')

# Function to extract data with regex from the text file
def extract_data_from_file(filename):
    data = {'TIME_DELTA': [], 'SPEED': [], 'REFERENCE': [], 'CONTROL': [], 'ERROR': []}
    with open(filename, "r") as file:
        for line in file:
            match = re.search(r'TIME_DELTA:(\d+\.\d+),.*SPEED:(-?\d+\.\d+),REFERENCE:(-?\d+\.\d+),CONTROL:(-?\d+\.\d+),ERROR:(-?\d+\.\d+)', line)
            if match:
                data['TIME_DELTA'].append(float(match.group(1)))
                data['SPEED'].append(float(match.group(2)))
                data['REFERENCE'].append(float(match.group(3)))
                data['CONTROL'].append(float(match.group(4)))
                data['ERROR'].append(float(match.group(5)))
    return data  # Return as a dictionary, not a list

# Function to plot the extracted data in subplots for both motors
def plot_data(data1, data2):
    fig, axes = plt.subplots(3, 1, figsize=(8, 6), sharex=True)

    axes[0].plot(data1['TIME_DELTA'], data1['SPEED'], label='Motor 1', color="k", linewidth=2)
    axes[0].plot(data2['TIME_DELTA'], data2['SPEED'], label='Motor 2', color="b", linewidth=2)
    axes[0].plot(data2['TIME_DELTA'], data2['REFERENCE'], label='Referencia', color="r", linewidth=2)
    axes[0].grid()
    axes[0].legend()
    axes[0].set_ylabel("Velocidad (RPM)")

    axes[1].plot(data1['TIME_DELTA'], data1['CONTROL'], label='Motor 1', color="k", linewidth=2)
    axes[1].plot(data2['TIME_DELTA'], data2['CONTROL'], label='Motor 2', color="b", linewidth=2)
    axes[1].grid()
    axes[1].legend()
    axes[1].set_ylabel("Control (%)")

    axes[2].plot(data1['TIME_DELTA'], data1['ERROR'], label='Motor 1', color="k", linewidth=2)
    axes[2].plot(data2['TIME_DELTA'], data2['ERROR'], label='Motor 2', color="b", linewidth=2)
    axes[2].grid()
    axes[2].set_xlabel("Tiempo (s)")
    axes[2].set_ylabel("Error (%)")

    plt.tight_layout()
    plt.legend()
    plt.show()

try:
    # Generate new data files for each motor with datetime
    data_filename_motor1 = generate_data_filename(1)
    data_filename_motor2 = generate_data_filename(2)
    
    # Initialize variables to track time delta for both motors starting from 0
    time_delta_motor1 = 0.0
    time_delta_motor2 = 0.0
    last_time_motor1 = time.time()  # Initialize the last_time for motor 1
    last_time_motor2 = time.time()  # Initialize the last_time for motor 2
    
    while True:
        # Read data from the serial port
        try:
            data = serial_port.readline().decode('utf-8').strip()
        except UnicodeDecodeError:
            print("UnicodeDecodeError: Unable to decode the received data.")
            continue  # Skip this iteration and continue with the next loop iteration
        
        # Check if there is any data to print
        if data:
            # Get the current time
            current_time = time.time()
            
            # Calculate the time elapsed since the last data point for both motors
            time_elapsed_motor1 = current_time - last_time_motor1
            time_elapsed_motor2 = current_time - last_time_motor2
            
            # Update the time delta for both motors by adding the time elapsed
            time_delta_motor1 += time_elapsed_motor1
            time_delta_motor2 += time_elapsed_motor2
            
            #print data to console
            print(data)
            
            # Format the data with time delta and save it to the appropriate text file
            if "MOTOR 1" in data:
                formatted_data = f"TIME_DELTA:{time_delta_motor1:.2f},{data}"
                save_data_to_file(formatted_data, data_filename_motor1)
                last_time_motor1 = current_time
            elif "MOTOR 2" in data:
                formatted_data = f"TIME_DELTA:{time_delta_motor2:.2f},{data}"
                save_data_to_file(formatted_data, data_filename_motor2)
                last_time_motor2 = current_time

except KeyboardInterrupt:
    # Handle Ctrl+C to gracefully exit the program
    print("Program terminated by user.")
finally:
    # Close the serial port to release resources
    serial_port.close()

# Extract and plot the data after the program ends for both motors
extracted_data_motor1 = extract_data_from_file(data_filename_motor1)
extracted_data_motor2 = extract_data_from_file(data_filename_motor2)
plot_data(extracted_data_motor1, extracted_data_motor2)
