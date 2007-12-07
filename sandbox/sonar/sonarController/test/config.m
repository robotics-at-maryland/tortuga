% Basic design parameters %

c                  = 1500.0;    % speed of sound in water in m/s
f                  = 30e3;      % frequency of signal in Hertz
lambda             = c / f;     % wavelength of signal in water in meters
hydro_pos_accuracy = 0.0000635; % machining tolerance in meters
ping_duration      = 1.3e-3;    % ping duration in seconds
f_s                = 1e6;       % sample rate in samples per second
tdoa_accuracy      = lambda/150;% TDOA resolution in meters
depth_accuracy     = 2*lambda;  % depth sensor resolution in meters
adc_bits           = 8;
adc_amplitude      = 1;
signal_amplitude   = 0.75;
snr                = 10;        % signal to noise ratio (unitless)
max_sensor_separation = 0.5;    % meters
max_speed = 3;                  % meters per second
max_range = 30;                 % meters
tfirstping = 0.0001;            % time of the first ping
baseline = 0.2;                 % distance between sensors in meters
pingrate = 2;                   % seconds between pings
bearing = pi/4;                 % relative bearing from to pinger in radians
trainingtime = 10;              % seconds during stationary
                                % training period

