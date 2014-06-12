function [cgTime, cgMag, cgAcc, cgGyro, boomTime, boomMag, boomAcc, boomGyro] = parseIMULog(log)

[logRows logCols] = size(log);

cgIndex = 1;
boomIndex = 1;

cgTime = zeros(logRows, 1);
cgMag = zeros(logRows, 3);
cgAcc = zeros(logRows, 3);
cgGyro = zeros(logRows,3);

boomTime = zeros(logRows, 1);
boomMag = zeros(logRows, 3);
boomAcc = zeros(logRows, 3);
boomGyro = zeros(logRows,3);

for i = 1 : logRows
    if log(i, 1) == 0 % this means it is the CG IMU (i realize this is ugly)
        cgTime(cgIndex) = log(i, end) / 1000; % time is always last column
        cgMag(cgIndex,:) = log(i, 14:16); % yes, hardcoded indices are bad
        cgAcc(cgIndex,:) = log(i, 11:13);
        cgGyro(cgIndex,:) = log(i, 17:19);
        cgIndex = cgIndex + 1;
    end
    
    if log(i, 1) == 1 % this means it is the BOOM IMU
        boomTime(boomIndex) = log(i, end) / 1000;
        boomMag(boomIndex, :) = log(i, 14:16);
        boomAcc(boomIndex, :) = log(i, 11:13);
        boomGyro(boomIndex, :) = log(i, 17:19);
        boomIndex = boomIndex + 1;
    end
end

% truncate the arrays
cgTime = cgTime(1 : cgIndex - 1);
cgMag = cgMag(1 : cgIndex - 1, :);
cgAcc = cgAcc(1 : cgIndex - 1, :);
cgGyro = cgGyro(1 : cgIndex - 1, :);

boomTime = boomTime(1 : boomIndex - 1);
boomMag = boomMag(1 : boomIndex - 1, :);
boomAcc = boomAcc(1 : boomIndex - 1, :);
boomGyro = boomGyro(1 : boomIndex - 1, :);