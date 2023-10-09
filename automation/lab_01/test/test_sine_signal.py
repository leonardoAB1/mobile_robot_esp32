import requests
import json
import time
import numpy as np

def sine_wave_generator(amplitude, period):
    start_time = time.time()  # Record the start time
    while True:
        current_time = time.time()  # Get the current time
        elapsed_time = current_time - start_time  # Calculate elapsed time
        t = elapsed_time  # Use elapsed time as the time parameter
        # Implement the step function logic
        if t % period <= period / 2:
            yield amplitude

reqUrl = "http://192.168.0.188/reference/set"

headersList = {
    "Accept": "*/*",
    "User-Agent": "Thunder Client (https://www.thunderclient.com)",
    "Adc-Resolution": "10",
    "Controller": "PID",
    "Content-Type": "application/json"
}

# Configure the sine wave generator with the desired parameters
signal_generator = sine_wave_generator(amplitude=150, period=1)  # Adjust amplitude and frequency as needed

try:
    while True:
        signal_value = next(signal_generator)
        payload = json.dumps({
            "referenceSignal": signal_value
        })
        response = requests.post(reqUrl, data=payload, headers=headersList)
        print(f"Sent signal: {signal_value}, Response: {response.text}")
        time.sleep(0.1)  # Add a time delay to control the update rate (adjust as needed)
except KeyboardInterrupt:
    print("Script terminated by the user.")
