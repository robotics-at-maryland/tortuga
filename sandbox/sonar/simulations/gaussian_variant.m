function tdoas = gaussian_variant(source_pos, hydro_pos, hydro_pos_accuracy, tdoa_accuracy)
% GAUSSIAN_VARIANT
%    Return simulated noisy TDOAs

% If the matrix begins with a zero-filled row, strip it out
if all(hydro_pos(1,:)==[0,0,0])
  hydro_pos = hydro_pos(2:end,:);
end

m = size(hydro_pos);
m = m(1);

% construct random variants for hydro_pos and tdoas

% hydro_pos_real = hydro_pos+randn_renormalized(0, hydro_pos_accuracy, [m,3]);
hydro_pos_real = hydro_pos;
range0 = sqrt(dot(source_pos, source_pos));
range = hydro_pos_real - repmat(source_pos,m,1);
range = sqrt(dot(range,range,2));
range0 = range0 + randn_renormalized(0, tdoa_accuracy, 1);
range = range + randn_renormalized(0, tdoa_accuracy, [m,1]);
tdoas = range - range0;
tdoas = tdoas';
