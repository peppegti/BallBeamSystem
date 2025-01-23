m   = 0.0027; % mass [kg]
d   = 0.04;   % diameter [m]
L   = 0.6;    % length [m]
g   = 9.81;   % gravitational acceleration [m/s^2]
l1  = 0.04;   % l1 length [m]
l2  = 0.105;  % l2 length [m]
syms s        

% Inertial moment
J = (2/5) * m * d^2;

% Numerator
R_force = m * g * l1;

% Denominator
theta = L * ((J / d^2) + m) * s^2;

% Convert symbolic transfer function to MATLAB transfer function
numerator = R_force; % Numerator is constant
denominator = sym2poly(theta); % Coefficients of the denominator polynomial

% Build transfer function
P = tf(numerator, denominator);

% Plot the first transfer function response
figure(1);
disp('Transfer function P(s):');
disp(P);

% Unit step response
step(P);
title('Unit step response');
xlabel('Time (s)');
ylabel('Amplitude');
grid on;

% Values established by the specs
wt  = 2.73; % Cutoff frequency [rad/s]
phi = 57;   % Phase margin [degrees]

% Calculate PID parameters
modP = 0.484 / wt^2;
Kp = cos(deg2rad(phi)) / modP;
Kd = sin(deg2rad(phi)) / (modP * wt);
paramPID = [Kp, Kd];

% Create a PID transfer function
s = tf('s');
P = 1.343 / s^2;
PID = Kp + Kd * s;
sys = P * PID;

% Plot the PID step response
figure(2);
feed = sys / (1 + sys);
step(feed);
title('Unit step response with PID');
xlabel('Time (s)');
ylabel('Amplitude');
grid on;

