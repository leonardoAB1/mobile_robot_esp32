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
def generate_data_filename():
    current_time = datetime.datetime.now()
    filename = current_time.strftime("%Y%m%d%H%M%S.txt")
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

# Function to plot the extracted data in a subplot
def plot_data(data):
    fig, axes = plt.subplots(3, 1, figsize=(8, 6), sharex=True)

    axes[0].plot(data['TIME_DELTA'], data['SPEED'], label='SPEED', color="k", linewidth=2)
    axes[0].plot(data['TIME_DELTA'], data['REFERENCE'], "--r", label='REFERENCE', linewidth=2)
    axes[0].grid()
    axes[0].set_ylabel("Velocidad(RPM)")

    axes[1].plot(data['TIME_DELTA'], data['CONTROL'], label='CONTROL', color="k", linewidth=2)
    axes[1].grid()
    axes[1].set_ylabel("Control(%)")

    axes[2].plot(data['TIME_DELTA'], data['ERROR'], label='ERROR', color="k", linewidth=2)
    axes[2].grid()
    axes[2].set_xlabel("Tiempo(s)")
    axes[2].set_ylabel("Error(%)")

    plt.tight_layout()
    plt.show()

try:
    # Generate a new data file with datetime
    data_filename = generate_data_filename()
    
    # Initialize a variable to track time delta starting from 0
    time_delta = 0.0
    last_time = time.time()  # Initialize the last_time
    
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
            
            # Calculate the time elapsed since the last data point
            time_elapsed = current_time - last_time
            
            # Update the time delta by adding the time elapsed
            time_delta += time_elapsed
            
            # Format the data with time delta and save it to the text file
            formatted_data = f"TIME_DELTA:{time_delta:.2f},{data}"
            print(formatted_data)
            save_data_to_file(formatted_data, data_filename)
            
            # Update the last_time for the next iteration
            last_time = current_time

except KeyboardInterrupt:
    # Handle Ctrl+C to gracefully exit the program
    print("Program terminated by user.")
finally:
    # Close the serial port to release resources
    serial_port.close()

# Extract and plot the data after the program ends
extracted_data = extract_data_from_file(data_filename)
plot_data(extracted_data)

