import re
import numpy as np
import matplotlib.pyplot as plt

# Define the filename with the data
filename = r'automation\lab_02\data\20231010162800_motor2.txt'

# Lists to store the time, speed, and reference data
time_data = []
speed_data = []
reference_data = []

# Regular expression pattern to extract TIME_DELTA, SPEED, and REFERENCE
pattern = r'TIME_DELTA:(\d+\.\d+),.*SPEED:([-+]?\d+\.\d+),REFERENCE:([-+]?\d+\.\d+)'

# Read the data from the file and extract relevant information
with open(filename, 'r') as file:
    for line in file:
        match = re.search(pattern, line)
        if match:
            time = float(match.group(1))
            speed = float(match.group(2))
            reference = float(match.group(3))
            time_data.append(time)
            speed_data.append(speed)
            reference_data.append(reference)

# Create a figure with two subplots for step and speed response
fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 6))

# Plot the speed response
ax1.plot(time_data, speed_data, label='Speed Response', color='k', linewidth=2)
ax1.set_xlabel('Tiempo (s)')
ax1.set_ylabel('Velocidad(RPM)')
ax1.grid(True)

# Plot the step response
ax2.plot(time_data, reference_data, label='Step Input', color='r', linewidth=2)
ax2.set_xlabel('Tiempo (s)')
ax2.set_ylabel('Entrada(%)')
ax2.grid(True)

plt.tight_layout()
plt.show()
