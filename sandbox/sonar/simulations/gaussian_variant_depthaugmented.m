function [tdoas, zs_real] = gaussian_variant_depthaugmented(source_pos, hydro_pos, hydro_pos_accuracy, tdoa_accuracy, depth_accuracy)
% GAUSSIAN_VARIANT_DEPTHAUGMENTED
%    Return simulated noisy TDOAs with additional noisy depth estimate, for
%    use with LOCALIZE_DEPTHAUGMENTED

tdoas = gaussian_variant(source_pos, hydro_pos, hydro_pos_accuracy, tdoa_accuracy);
zs_real = source_pos(3) + randn_renormalized(0, depth_accuracy, 1);
