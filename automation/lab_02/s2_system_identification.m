clear all;
clc;
% Load your data
data = readmatrix('automation/lab_02/data/20231010162800_motor2.csv');  % Replace with your actual file name

% Extract SPEED and input data
time =  data(:, 1); 
speed = data(:, 2);  
input = data(:, 3); 

% Calculate Ts by getting the average difference between samples in time vector
Ts = mean(diff(time));
% Create a time vector so that it has venly spaced time samples
sample_number = (0:(length(speed)-1))';
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
xlabel('Tiempo (s)');
ylabel('Velocidad (RPM)');
title('Comparación del Modelo Discreto y Datos Reales');
legend('Location', 'Best');

% Bode plot of the identified system
subplot(2, 1, 2);  % Create a subplot for the Bode plot
bode(sys);  % Plot the Bode plot of the identified system
grid on;
title('Bode Plot M2');

% Save the estimated discrete-time model to a variable if needed
% sys_discrete_estimated = sys_discrete;
