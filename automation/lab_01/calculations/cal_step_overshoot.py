import re
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

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

# Function to calculate overshoot in percentage
def calculate_overshoot(data):
    max_speed = max(data['SPEED'])
    max_reference = max(data['REFERENCE'])
    overshoot_percentage = ((max_speed - max_reference) / max_reference) * 100
    return overshoot_percentage

# Function to plot the extracted data in a subplot with shaded regions
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

# Load data from the text file
filename = "../test/data/20230924214918.txt"
data = extract_data_from_file(filename)

# Convert data into a Pandas DataFrame
df = pd.DataFrame(data)

# Calculate and print the overshoot percentage
overshoot_percentage = calculate_overshoot(df)
print(f"Overshoot Percentage: {overshoot_percentage:.2f}%")

# Plot the data
plot_data(df)
