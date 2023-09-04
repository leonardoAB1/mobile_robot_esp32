import numpy as np
from matplotlib.lines import Line2D
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import requests
import time

class Scope(object):
    def __init__(self, ax, maxt=20, dt=0.02):
        self.ax = ax
        self.dt = dt
        self.maxt = maxt
        self.tdata = [0]
        self.ydata = [0]
        self.line = Line2D(self.tdata, self.ydata)
        self.ax.add_line(self.line)
        self.ax.set_ylim(0, 100)
        self.ax.set_xlim(0, self.maxt)
        self.ax.grid()
        self.ax.grid(color='green', linestyle='--', linewidth=0.5)

        # Setting the labels
        self.ax.set_xlabel('Elapsed Time')
        self.ax.set_ylabel('Y-axis')
        self.ax.set_title('Signal Plotter', color='#FFF')

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

        # Initialize time measurement
        self.start_time = time.time()

    def update(self, y):
        elapsed_time = time.time() - self.start_time
        self.tdata.append(elapsed_time)  # Use elapsed time as x-axis data
        self.ydata.append(y)
        self.line.set_data(self.tdata, self.ydata)

        if elapsed_time > self.tdata[0] + self.maxt:  # reset the arrays
            self.tdata.pop(0)  # Remove the first data point
            self.ydata.pop(0)
            self.ax.set_xlim(self.tdata[0], self.tdata[0] + self.maxt)
            self.ax.figure.canvas.draw()

        return self.line,

def plotter(reqUrl, headersList, duration, interval):
    start_time = time.time()
    end_time = start_time + duration
    while time.time() < end_time:
        response = requests.get(reqUrl, headers=headersList)
        data = response.json()

        # Extract value from the response
        value = data.get("reference_state")
        if value is not None:
            yield value
        else:
            yield 0

        time.sleep(interval)

reqUrl = "http://192.168.0.188/reference/get"
headersList = {
    "Accept": "*/*",
    "User-Agent": "Thunder Client (https://www.thunderclient.com)",
}
duration = float('inf')  # Plot until the matplotlib window is closed
interval = 0.5

fig, ax = plt.subplots()
fig.patch.set_facecolor("#242424")

scope = Scope(ax)
ani = animation.FuncAnimation(fig, scope.update, plotter(reqUrl, headersList, duration, interval),
                              blit=True, interval=interval*1000, cache_frame_data=False)  # Convert interval to milliseconds
plt.show()
