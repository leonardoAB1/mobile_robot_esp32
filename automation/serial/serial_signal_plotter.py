import serial
import re
import matplotlib.pyplot as plt
from matplotlib.ticker import MultipleLocator
from collections import deque
import time  # Import the time library

# Configure the serial port (replace 'COMX' with your actual serial port)
serial_port = serial.Serial('COM5', baudrate=115200, timeout=1)

# Initialize lists to store time, speed, and reference values
time_values = []
speed_values = []
reference_values = []  # Add a list for reference values

# Initialize a deque to store the last N data points for plotting
N = 80  # Number of data points to display in the plot
speed_buffer = deque(maxlen=N)
reference_buffer = deque(maxlen=N)  # Add a buffer for reference values

# Set the duration of script execution in seconds
duration_seconds = 100  # Replace with your desired duration

# Get the start time
start_time = time.time()

# Create subplots and adjust spacing
fig, (speed_plot, reference_plot) = plt.subplots(2, 1, sharex=True)
fig.suptitle('Real-Time Data Plot')

speed_plot.set_title('Speed (RPM)')
speed_plot.set_xlabel('Time')
speed_plot.set_ylabel('Speed (RPM)')
speed_plot.grid(True)

reference_plot.set_title('Reference')
reference_plot.set_xlabel('Time')
reference_plot.set_ylabel('Reference %')
reference_plot.grid(True)

plt.tight_layout()  # Automatically adjust subplot parameters to prevent overlap

try:
    while True:
        # Read and print the data from the serial port
        try:
            data = serial_port.readline().decode('utf-8').strip()
            print(data)
        except UnicodeDecodeError:
            print("UnicodeDecodeError: Unable to decode the received data.")
            continue  # Skip this iteration and continue with the next loop iteration


        # Use regular expressions to extract speed and reference values
        speed_match = re.search(r'SPEED:(\d+\.\d+)', data)
        reference_match = re.search(r'REFERENCE:(\d+\.\d+)', data)  # Extract reference values
        if speed_match and reference_match:
            if speed_match:
                speed = float(speed_match.group(1))

                # Add the current time and speed value to the lists
                time_values.append(len(speed_values))
                speed_values.append(speed)

                # Append the speed value to the buffer
                speed_buffer.append(speed)

                # Plot the updated speed data
                speed_plot.clear()
                speed_plot.plot(time_values[-len(speed_values):], speed_values, "k", linewidth=2, label='Speed (RPM)')
                speed_plot.legend()
                speed_plot.grid(True, linewidth=0.5, alpha=0.5)  # Adjust linewidth and alpha for the grid lines
                speed_plot.xaxis.set_major_locator(plt.MultipleLocator(10))  # Major grid every 10 data points
                speed_plot.xaxis.set_minor_locator(plt.MultipleLocator(1))  # Minor grid every 1 data point
                speed_plot.set_xlim(0, min(len(reference_values), len(speed_values)))

            if reference_match:
                reference = float(reference_match.group(1))
                if float(reference_match.group(1))<50:
                    reference *= 2

                # Add the current time and reference value to the lists
                reference_values.append(reference)

                # Append the reference value to the buffer
                reference_buffer.append(reference)

                # Plot the updated reference data
                reference_plot.clear()
                reference_plot.plot(time_values[-len(reference_values):], reference_values, "--r", linewidth=2, label='Reference')
                reference_plot.legend()
                reference_plot.grid(True, linewidth=0.5, alpha=0.5)  # Adjust linewidth and alpha for the grid lines
                reference_plot.xaxis.set_major_locator(plt.MultipleLocator(10))  # Major grid every 10 data points
                reference_plot.xaxis.set_minor_locator(plt.MultipleLocator(1))  # Minor grid every 1 data point
                reference_plot.set_xlim(0, min(len(reference_values), len(speed_values)))

        # Pause for a short time to update the plot
        plt.pause(0.01)

        # Check if the script has run for the desired duration
        current_time = time.time()
        if current_time - start_time >= duration_seconds:
            break

except KeyboardInterrupt:
    print("Serial communication interrupted.")
finally:
    #serial_port.close()
    plt.close()  # Close the plot when done

# Plot the full data after the script execution
# Create a figure with two subplots
fig, axs = plt.subplots(2, 1, figsize=(8, 6), sharex=True)
fig.suptitle("Open Loop Step Response")
# Plot the first subplot (top subplot)
axs[0].plot(time_values, speed_values, "k", linewidth=2,)
axs[0].set_ylabel('Speed (RPM)')
axs[0].grid(True)
# Plot the second subplot (bottom subplot)
axs[1].plot(time_values, reference_values, linestyle='--', linewidth=2, color='r')
axs[1].set_xlabel('Time')
axs[1].set_ylabel('Input (%)')
axs[1].grid(True)
# Adjust spacing between subplots
plt.tight_layout()
plt.show()