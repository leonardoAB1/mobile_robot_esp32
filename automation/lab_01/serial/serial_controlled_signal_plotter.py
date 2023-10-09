import serial
import re
import matplotlib.pyplot as plt
from collections import deque
import time

# Configure the serial port (replace 'COMX' with your actual serial port)
serial_port = serial.Serial('COM5', baudrate=115200, timeout=1)

# Initialize lists to store time, speed, and reference values
time_values = []
speed_values = []
reference_values = []

# Initialize a deque to store the last N data points for plotting
N = 50
speed_buffer = deque(maxlen=N)
reference_buffer = deque(maxlen=N)

# Set the duration of script execution in seconds
duration_seconds = 100

# Get the start time
start_time = time.time()

# Create a single subplot
fig, ax = plt.subplots()
fig.suptitle('Real-Time Data Plot')

ax.set_xlabel('Time')
ax.grid(True)

try:
    while True:
        data = serial_port.readline().decode('utf-8').strip()

        speed_match = re.search(r'SPEED:(\d+\.\d+)', data)
        reference_match = re.search(r'REFERENCE:(\d+\.\d+)', data)

        if speed_match:
            speed = float(speed_match.group(1))
            time_values.append(len(speed_values))
            speed_values.append(speed)
            speed_buffer.append(speed)

        if reference_match:
            reference = float(reference_match.group(1))
            reference_values.append(reference)
            reference_buffer.append(reference)

        # Clear the subplot and plot both speed and reference data
        ax.clear()
        ax.plot(time_values, speed_values, "k", linewidth=2, label='Speed (RPM)')
        ax.plot(time_values[-len(reference_values):], reference_values, "--r", linewidth=2, label='Reference')
        ax.legend()
        plt.grid()

        plt.pause(0.1)

        current_time = time.time()
        if current_time - start_time >= duration_seconds:
            break

except KeyboardInterrupt:
    print("Serial communication interrupted.")
finally:
    serial_port.close()
    plt.close()
