import csv
import re

# Define the function to process the text data and save it to a CSV file
def convert_txt_to_csv(txt_filename):
    # Create a list to store the data
    data = []

    # Define the regex pattern
    pattern = r'TIME_DELTA:(\d+\.\d+),.*SPEED:(-?\d+\.\d+),REFERENCE:(-?\d+\.\d+),CONTROL:(-?\d+\.\d+),ERROR:(-?\d+\.\d+)'

    # Open the input text file
    with open(txt_filename, 'r') as txt_file:
        for line in txt_file:
            # Use the regex pattern to extract the data
            match = re.search(pattern, line)
            if match:
                time_delta, speed, reference, control, error = match.groups()
                data.append([time_delta, speed, reference, control, error])

    # Create a CSV filename based on the input text filename
    csv_filename = txt_filename.replace('.txt', '.csv')

    # Write the data to a CSV file
    with open(csv_filename, 'w', newline='') as csv_file:
        csv_writer = csv.writer(csv_file)
        # Write header
        csv_writer.writerow(["TIME_DELTA", "SPEED", "REFERENCE", "CONTROL", "ERROR"])
        # Write the data rows
        csv_writer.writerows(data)

    print(f"CSV file '{csv_filename}' has been created.")

# Provide the name of the input text file here
txt_filename = r"automation\lab_02\data\20231010162800_motor2.txt"

# Call the function to convert and save the data
convert_txt_to_csv(txt_filename)

