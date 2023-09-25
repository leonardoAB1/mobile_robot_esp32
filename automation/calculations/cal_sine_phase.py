import re
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import correlate

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
    return data

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

def calculate_phase_difference(REFERENCE, SPEED):
    # Convert the data into NumPy arrays
    reference_signal = np.array(REFERENCE)
    speed_signal = np.array(SPEED)

    # Calculate cross-correlation
    cross_correlation = correlate(reference_signal, speed_signal, mode='full')

    # Find the index of the maximum correlation (time delay)
    time_delay = np.argmax(cross_correlation) - (len(reference_signal) - 1)

    # Calculate phase difference in radians
    phase_difference_rad = 2 * np.pi * time_delay / len(reference_signal)

    # Convert phase difference to degrees
    phase_difference_deg = np.degrees(phase_difference_rad)

    return phase_difference_deg

# Load data from the text file
filename = "../test/data/20230924212149.txt"
data = extract_data_from_file(filename)
df = pd.DataFrame(data)
phase_difference = calculate_phase_difference(df['REFERENCE'], df['SPEED'])
print("Phase Difference (degrees):", phase_difference)
plot_data(df)
