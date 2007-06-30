function convertedMeasuredState = CppMeasuredState(MeasuredState, too_from)

% We are converting from Matlab to CPP format
if too_from == 1
    a = MeasuredState.linearAcceleration;
    q = MeasuredState.quaternion;
    w = MeasuredState.angularRate;

    % Convert static array to individual numbers (HACK!)
    convertedMeasuredState = struct('depth',MeasuredState.depth, ...
        'a0', a(1), 'a1', a(2), 'a2', a(3), ...
        'q0', q(1), 'q1', q(2), 'q2', q(3), 'q3', q(4),  ...
        'w0', w(1), 'w1', w(2), 'w2', w(3));
    
else
    % Converting from CPP to Matlab
    convertedMeasuredState = struct('depth', MeasuredState.depth, ...
        'linearAcceleration', [MeasuredState.a0 MeasuredState.a1 MeasuredState.a2]',...
        'quaternion', [MeasuredState.q0 MeasuredState.q1 MeasuredState.q2 MeasuredState.q3]',...
        'angularRate',[MeasuredState.w0 MeasuredState.w1 MeasuredState.w2]');
end