import numpy as np
from matplotlib.lines import Line2D
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import requests
import json
import time

class Scope:
    def __init__(self, ax, maxt=20, dt=0.02):
        self.ax = ax
        self.dt = dt
        self.maxt = maxt
        self.tdata = [0]
        self.ydata = [0]
        self.line = Line2D(self.tdata, self.ydata)
        self.ax.add_line(self.line)
        self.ax.set_ylim(0, 2 ** adc_bit_number-1)
        self.ax.set_xlim(0, self.maxt)
        self.ax.grid()
        self.ax.grid(color='green', linestyle='--', linewidth=0.5)

        # Setting the labels
        self.ax.set_xlabel('Elapsed Time')
        self.ax.set_ylabel('Y-axis')
        self.ax.set_title('Reference Generator', color='#FFF')

        # Setting the axis color
        self.ax.spines['bottom'].set_color('#FFF')
        self.ax.spines['left'].set_color('#FFF')
        self.ax.spines['right'].set_color('#FFF')
        self.ax.spines['top'].set_color('#FFF')
        self.ax.xaxis.label.set_color('#FFF')
        self.ax.yaxis.label.set_color('#FFF')
        self.ax.tick_params(axis='x', colors='#FFF')
        self.ax.tick_params(axis='y', colors='#FFF')

        self.ax.set_facecolor("#242424")

        # Initialize text annotations for displaying parameters
        self.param_text = self.ax.text(0.02, 0.79, '', transform=self.ax.transAxes, color='#FFF')

        # Initialize time measurement
        self.start_time = time.time()

    def update(self, y):
        global send_data_flag
        elapsed_time = time.time() - self.start_time
        self.tdata.append(elapsed_time)  # Use elapsed time as x-axis data
        self.ydata.append(y)
        self.line.set_data(self.tdata, self.ydata)

        # Update text annotations with current parameter values and elapsed time
        self.param_text.set_text(f'Elapsed Time: {elapsed_time:.2f} seconds\nAmplitude: {amplitude:.2f}\nPeriod: {period:.2f}\n')

        if elapsed_time >= self.tdata[0] + self.maxt:
            self.tdata.pop(0)  # Remove the first data point
            self.ydata.pop(0)
            self.ax.set_xlim(self.tdata[0], self.tdata[0] + self.maxt)
            self.ax.figure.canvas.draw()

        try:
            # Data sending
            req_url = "http://192.168.0.188/reference/set"
            headers_list = {
                "Accept": "*/*",
                "User-Agent": "Thunder Client (https://www.thunderclient.com)",
                "Adc-Resolution": f"{adc_bit_number}",
                "Content-Type": "application/json"
            }
            
            if send_data_flag:
                payload = json.dumps({
                    "referenceSignal": y
                })
                response = requests.request("POST", req_url, data=payload, headers=headers_list, timeout=3)
                if response.status_code == 200:
                    print("POST request successful:", response.text)
                else:
                    print("POST request failed with status code:", response.status_code)

        except requests.exceptions.RequestException as e:
            print("An error occurred while sending the request:", e)
            send_data_flag = not send_data_flag 

        return self.line, self.param_text

def sine_wave_generator(amplitude, period, phase, offset=0):
    start_time = time.time()  # Record the start time
    while True:
        current_time = time.time()  # Get the current time
        elapsed_time = current_time - start_time  # Calculate elapsed time
        t = elapsed_time  # Use elapsed time as the time parameter

        yield offset + amplitude * np.sin(2 * np.pi * t / period + phase)

def toggle_data_flag(event):
    global send_data_flag
    if event.key == 'a':
        send_data_flag = not send_data_flag

if __name__ == "__main__":
    send_data_flag = 0
    adc_bit_number = 16

    amplitude = (2 ** adc_bit_number-1)/2
    f = 1
    period = 20
    phase = 0.0
    offset = amplitude

    fig, ax = plt.subplots()
    fig.patch.set_facecolor("#242424")
    scope = Scope(ax)

    sine_gen = sine_wave_generator(amplitude, period, phase, offset)

    ani = animation.FuncAnimation(fig, scope.update, sine_gen,  interval=50,
                                blit=True, repeat=True, cache_frame_data=False)  # Set repeat to True
    
    # Connect a key press event to toggle data sending flag
    fig.canvas.mpl_connect('key_press_event', toggle_data_flag)

    plt.show()
