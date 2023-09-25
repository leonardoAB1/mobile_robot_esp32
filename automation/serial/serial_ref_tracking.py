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
    data = {'SPEED': [], 'REFERENCE': []}
    with open(filename, "r") as file:
        for line in file:
            match = re.search(r'SPEED:(\d+\.\d+),REFERENCE:(\d+\.\d+)', line)
            if match:
                data['SPEED'].append(float(match.group(1)))
                data['REFERENCE'].append(float(match.group(2)))
    return data  # Return as a dictionary, not a list

# Function to plot the extracted data in a subplot
def plot_data(data):
    plt.figure(figsize=(8, 6))
    plt.plot(data['SPEED'], label='SPEED')
    plt.plot(data['REFERENCE'], label='REFERENCE')
    plt.grid()
    plt.ylabel("Velocidad(RPM)")
    plt.legend()
    plt.tight_layout()
    plt.show()

try:
    # Generate a new data file with datetime
    data_filename = generate_data_filename()
    
    while True:
        # Read data from the serial port
        try:
            data = serial_port.readline().decode('utf-8').strip()
            print(data)
        except UnicodeDecodeError:
            print("UnicodeDecodeError: Unable to decode the received data.")
            continue  # Skip this iteration and continue with the next loop iteration
        
        # Check if there is any data to print
        if data:
            # Print the data to the console
            print(data)
            
            # Save data to the text file
            save_data_to_file(data, data_filename)

except KeyboardInterrupt:
    # Handle Ctrl+C to gracefully exit the program
    print("Program terminated by user.")
finally:
    # Close the serial port to release resources
    serial_port.close()

# Extract and plot the data after the program ends
extracted_data = extract_data_from_file(data_filename)
plot_data(extracted_data)
