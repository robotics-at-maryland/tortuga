function rerror = range_error(source_pos, hydro_pos)
cr_lowerbound = crlb(source_pos, hydro_pos);
rerror = sqrt(dot(diag(cr_lowerbound),diag(cr_lowerbound)));
