clear all;
clc;

% Define the sampling period (1 kHz)
Ts = 1e-3;

% Load your data from 'extracted_data.csv'
data = readmatrix('extracted_data.csv');  % Replace with your actual file name

% Extract SPEED and input data
speed = data(1300:2300, 1);  % Assuming the speed data is in the first column
input = data(1300:2300, 2);  % Assuming the input data is in the second column

% Create a time vector based on the length of the data
sample_number = (0:(length(speed)-1))';

% Create a time vector
time = sample_number * Ts;

% Create a data iddata object for system identification
data_id = iddata(speed, input, Ts);

% Estimate a first-order transfer function model
sys = tfest(data_id, 1);

% Display the estimated model
disp('Estimated First-Order Transfer Function Model:');
disp(sys);

% Convert the continuous-time system to a discrete-time system using ZOH
sys_discrete = c2d(sys, Ts, 'zoh');

% Display the discrete-time model
disp('Discrete-Time First-Order Transfer Function Model:');
disp(sys_discrete);

% Simulate the discrete-time model to get the model response
model_response = lsim(sys_discrete, input, time);

% Plot the response of the estimated discrete-time model compared to the data
figure;
subplot(2, 1, 1);  % Create a subplot for the time-domain plot
plot(time, speed, 'k', 'LineWidth', 2, 'DisplayName', 'Actual Data');
hold on;
plot(time, model_response, 'b--', 'LineWidth', 2, 'DisplayName', 'Model Response');
grid on;
xlabel('Time (s)');
ylabel('Speed (RPM)');
title('Comparison of Discrete-Time Model and Actual Data');
legend('Location', 'Best');

% Bode plot of the identified system
subplot(2, 1, 2);  % Create a subplot for the Bode plot
bode(sys);  % Plot the Bode plot of the identified system
grid on;
title('Bode Plot of Identified System');

% Save the estimated discrete-time model to a variable if needed
% sys_discrete_estimated = sys_discrete;
