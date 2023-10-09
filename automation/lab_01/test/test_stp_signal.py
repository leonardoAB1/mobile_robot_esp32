import requests
import json
import time

def alternating_signal_generator(period_seconds):
    while True:
        yield 0
        time.sleep(period_seconds)
        yield 3*(2**10)/8
        time.sleep(period_seconds)

reqUrl = "http://192.168.0.188/reference/set"

headersList = {
    "Accept": "*/*",
    "User-Agent": "Thunder Client (https://www.thunderclient.com)",
    "Adc-Resolution": "10",
    "Controller": "PID",
    "Content-Type": "application/json"
}

signal_generator = alternating_signal_generator(10)  # Configure the period in seconds here (e.g., 5 seconds)

try:
    while True:
        signal_value = next(signal_generator)
        payload = json.dumps({
            "referenceSignal": signal_value
        })
        response = requests.post(reqUrl, data=payload, headers=headersList)
        print(f"Sent signal: {signal_value}, Response: {response.text}")
except KeyboardInterrupt:
    print("Script terminated by user.")
