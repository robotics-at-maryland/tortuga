function rerror = range_error(source_pos, hydro_pos, hydro_pos_accuracy, tdoa_accuracy)
cr_lowerbound = crlb(source_pos, hydro_pos, hydro_pos_accuracy, tdoa_accuracy);
rerror = sqrt(dot(diag(cr_lowerbound),diag(cr_lowerbound)));
