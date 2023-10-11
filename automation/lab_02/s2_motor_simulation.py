import numpy as np
import matplotlib.pyplot as plt
from scipy.optimize import minimize
import math

umax = 100
umin = 0
N = 2000

# Define the plant parameters
#M1
#A = 0.3156 
#B = 0.8522 
#M2
A = 0.3015  
B = 0.8477  

# Define the reference signal
yr = np.zeros(N)
yr[0:500] = 0
yr[500:1000] = 60
yr[1000:1500] = 100
yr[1500:2000] = 80

# Define the time step and create a time array
fs = 1000
Ts = 1 / fs
t = np.arange(N) * Ts

# Initialize the PID parameters randomly within the specified bounds
Kp = 0.01
Ki = 75
Kd = 5e-4

noise = np.random.normal(0, 0.1)  # Example noise

# Define the bounds for optimization
bounds = [(-10, None), (-10, None), (-10, None)]

# Define the cost function to be minimized (example: ITSE)
def cost_function_ITSE(params):
    Kp, Ki, Kd = params
    y = np.zeros(N)
    u = np.zeros(N)
    e = np.zeros(N)
    itse = 0.0  # Initialize the ITSE (Integral of Time-weighted Squared Errors)

    for k in range(2, N):
        y[k] = A * u[k - 1] + B * y[k - 1]

        e[k] = yr[k] - y[k]
        u[0] = u[k - 1] + Kp * (e[k] - e[k - 1]) + Ki * Ts * e[k] + (Kd / Ts) * (e[k] - 2 * e[k - 1] + e[k - 2])

        if u[k] > umax:
            u[k] = umax
        elif u[k] < umin:
            u[k] = umin

        # Update ITSE
        itse += (Ts / 2) * (k * Ts) * np.square(e[k])

    return itse  # Return ITSE as the cost

# Define the cost function to be minimized (IAE)
def cost_function_IAE(params):
    Kp, Ki, Kd = params
    y = np.zeros(N)
    u = np.zeros(N)
    e = np.zeros(N)
    iae = 0.0  # Initialize the IAE (Integral of Absolute Error)

    for k in range(2, N):
        y[k] = A * u[k - 1] + B * y[k - 1]

        e[k] = yr[k] - y[k]
        u[0] = u[k - 1] + Kp * (e[k] - e[k - 1]) + Ki * Ts * e[k] + (Kd / Ts) * (e[k] - 2 * e[k - 1] + e[k - 2])
        if u[k] > umax:
            u[k] = umax
        elif u[k] < umin:
            u[k] = umin

        # Update IAE
        iae += Ts * abs(e[k])

    return iae  # Return IAE as the cost

# Perform PID tuning using optimization for ITSE
initial_params = [Kp, Ki, Kd]
result_ITSE = minimize(cost_function_ITSE, initial_params, bounds=bounds)

# Extract the optimized PID parameters for ITSE
Kp_opt_ITSE, Ki_opt_ITSE, Kd_opt_ITSE = result_ITSE.x

# Perform PID tuning using optimization for IAE
result_IAE = minimize(cost_function_IAE, initial_params, bounds=bounds)

# Extract the optimized PID parameters for IAE
Kp_opt_IAE, Ki_opt_IAE, Kd_opt_IAE = result_IAE.x

# Simulate the system with the optimized parameters for ITSE
y_opt_ITSE = np.zeros(N)
u_opt_ITSE = np.zeros(N)
e_opt_ITSE = np.zeros(N)

# Simulate the system with the optimized parameters for IAE
y_opt_IAE = np.zeros(N)
u_opt_IAE = np.zeros(N)
e_opt_IAE = np.zeros(N)

for k in range(2, N):
    # Add perturbations and noise to the plant model for ITSE
    perturbation = 0.05 * np.sin(2 * np.pi * 0.1 * t[k])  # Example perturbation
    y_opt_ITSE[k] = A * u_opt_ITSE[k - 1] + B * y_opt_ITSE[k - 1] + perturbation + noise

    e_opt_ITSE[k] = yr[k] - y_opt_ITSE[k]
    u_opt_ITSE[k] = u_opt_ITSE[k - 1] + Kp_opt_ITSE * (e_opt_ITSE[k] - e_opt_ITSE[k - 1]) + \
                    Ki_opt_ITSE * Ts * e_opt_ITSE[k] + (Kd_opt_ITSE / Ts) * (e_opt_ITSE[k] - 2 * e_opt_ITSE[k - 1] + e_opt_ITSE[k - 2])

    if u_opt_ITSE[k] > umax:
        u_opt_ITSE[k] = umax
    elif u_opt_ITSE[k] < umin:
        u_opt_ITSE[k] = umin

    # Add perturbations and noise to the plant model for IAE
    y_opt_IAE[k] = A * u_opt_IAE[k - 1] + B * y_opt_IAE[k - 1] + perturbation + noise

    e_opt_IAE[k] = yr[k] - y_opt_IAE[k]
    u_opt_IAE[k] = u_opt_IAE[k - 1] + Kp_opt_IAE * (e_opt_IAE[k] - e_opt_IAE[k - 1]) + \
                    Ki_opt_IAE * Ts * e_opt_IAE[k] + (Kd_opt_IAE / Ts) * (e_opt_IAE[k] - 2 * e_opt_IAE[k - 1] + e_opt_IAE[k - 2])

    if u_opt_IAE[k] > umax:
        u_opt_IAE[k] = umax
    elif u_opt_IAE[k] < umin:
        u_opt_IAE[k] = umin

# Plot the results
plt.figure(figsize=(10, 5))
plt.subplot(3, 1, 1)
plt.plot(t, y_opt_ITSE, "k", linewidth=2)
plt.plot(t, yr, "--r", linewidth=2)
plt.legend(["y(k)", "yr(k)"])
plt.ylabel("Velocidad (RPM)")
plt.grid()

plt.subplot(3, 1, 2)
plt.plot(t, e_opt_ITSE, "k", linewidth=2)
plt.ylabel("Error (RPM)")
plt.grid()

plt.subplot(3, 1, 3)
plt.plot(t, u_opt_ITSE, "k", linewidth=2)
plt.ylabel("Control (%)")
plt.grid()
plt.show()

# Plot results for ITSE
'''plt.subplot(3, 2, 1)
plt.plot(t, y_opt_ITSE, "k", linewidth=2)
plt.plot(t, yr, "--r", linewidth=2)
plt.legend(["y(k)", "yr(k)"])
plt.grid()
plt.title("System Response with Optimized PID (ITSE)")

plt.subplot(3, 2, 3)
plt.plot(t, e_opt_ITSE, "k", linewidth=2)
plt.legend(["e(k)"])
plt.grid()
plt.title("Error (ITSE)")

plt.subplot(3, 2, 5)
plt.plot(t, u_opt_ITSE, "k", linewidth=2)
plt.legend(["u(k)"])
plt.grid()
plt.title("Control (ITSE)")

# Plot results for IAE
plt.subplot(3, 2, 2)
plt.plot(t, y_opt_IAE, "k", linewidth=2)
plt.plot(t, yr, "--r", linewidth=2)
plt.legend(["y(k)", "yr(k)"])
plt.grid()
plt.title("System Response with Optimized PID (IAE)")

plt.subplot(3, 2, 4)
plt.plot(t, e_opt_IAE, "k", linewidth=2)
plt.legend(["e(k)"])
plt.grid()
plt.title("Error (IAE)")

plt.subplot(3, 2, 6)
plt.plot(t, u_opt_IAE, "k", linewidth=2)
plt.legend(["u(k)"])
plt.grid()
plt.title("Control (IAE)")

plt.tight_layout()
plt.show()'''

# Print the optimized parameters for both cost functions
print("Optimized PID Parameters (ITSE):")
print(f"Kp = {Kp_opt_ITSE}, Ki = {Ki_opt_ITSE}, Kd = {Kd_opt_ITSE}")

print("Optimized PID Parameters (IAE):")
print(f"Kp = {Kp_opt_IAE}, Ki = {Ki_opt_IAE}, Kd = {Kd_opt_IAE}")

print(f"ITSE: u[0] = u[1] + {Kp_opt_ITSE} * (e[0] - e[1]) + {Ki_opt_ITSE * Ts} * e[0] + {(Kd_opt_ITSE / Ts)} * (e[0] - 2 * e[1] + e[2]);")

print(f"IAE: u[0] = u[1] + {Kp_opt_IAE} * (e[0] - e[1]) + {Ki_opt_IAE * Ts} * e[0] + {(Kd_opt_IAE / Ts)} * (e[0] - 2 * e[1] + e[2]);")
