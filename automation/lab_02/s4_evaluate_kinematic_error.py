import os
import pandas as pd
import re

def calculate_average_error(folder_path):
    # Define the file name pattern regex
    file_name_pattern = r'\d{14}_robot_kinematics\.csv'

    # Get a list of CSV files in the folder that match the file name pattern
    csv_files = [file for file in os.listdir(folder_path) if re.match(file_name_pattern, file)]

    if not csv_files:
        print("No CSV files matching the specified file name pattern found in the folder.")
        return

    total_error_linear = 0
    total_error_angular = 0
    total_records = 0

    for file in csv_files:
        file_path = os.path.join(folder_path, file)
        df = pd.read_csv(file_path)

        if 'Error in Lineal Speed' in df.columns and 'Error in Angular Speed' in df.columns:
            error_linear = df['Error in Lineal Speed'].mean()
            error_angular = df['Error in Angular Speed'].mean()
            total_records += len(df)
            total_error_linear += error_linear * len(df)
            total_error_angular += error_angular * len(df)

    if total_records == 0:
        print("No matching columns found in the CSV files.")
    else:
        average_error_linear = total_error_linear / total_records
        average_error_angular = total_error_angular / total_records
        print(f"Average Error in Linear Speed: {average_error_linear} m/s")
        print(f"Average Error in Angular Speed: {average_error_angular} rad/s")

if __name__ == "__main__":
    calculate_average_error("./data")
