# define COSCALE 0x11616E8E	/* 291597966 = 0.2715717684432241 * 2^30, valid for j>13 */
#  define MAXITER 16	/* the resolution of the arctan table */
#  define QUARTER ((long)(3.141592654 / 2.0 * (1L << 16)))
static long arctantab[] = {
	72558, 51472, 30386, 16055, 8150, 4091, 2047, 1024,
	512, 256, 128, 64, 32, 16, 8, 4,
	2, 1
};


/* To rotate a vector through an angle of theta, we calculate:
 *
 *	x' = x cos(theta) - y sin(theta)
 *	y' = x sin(theta) + y cos(theta)
 *
 * The rotate() routine performs multiple rotations of the form:
 *
 *	x[i+1] = cos(theta[i]) { x[i] - y[i] tan(theta[i]) }
 *	y[i+1] = cos(theta[i]) { y[i] + x[i] tan(theta[i]) }
 *
 * with the constraint that tan(theta[i]) = pow(2, -i), which can be
 * implemented by shifting. We always shift by either a positive or
 * negative angle, so the convergence has the ringing property. Since the
 * cosine is always positive for positive and negative angles between -90
 * and 90 degrees, a predictable magnitude scaling occurs at each step,
 * and can be compensated for instead at the end of the iterations by a
 * composite scaling of the product of all the cos(theta[i])'s.
 */

static void
PseudoPolarize(long *argx, long *argy)
{
	register long theta;
	register long yi, i;
	register long x, y;
	register long *arctanptr;

	x = *argx;
	y = *argy;

	/* Get the vector into the right half plane */
	theta = 0;
	if (x < 0) {
		x = -x;
		y = -y;
		theta = 2 * QUARTER;
	}

	if (y > 0)
		theta = - theta;
	//for some reason, the first rotation is posetive, I think it has to do with how it gets set up
	arctanptr = arctantab;
	if (y < 0) {	/* Rotate positive */
		yi = y + (x << 1);
		x  = x - (y << 1);
		y  = yi;
		theta -= *arctanptr++;	/* Subtract angle */
	}
	else {		/* Rotate negative */
		yi = y - (x << 1);
		x  = x + (y << 1);
		y  = yi;
		theta += *arctanptr++;	/* Add angle */
	}

	for (i = 0; i <= MAXITER; i++) {
		if (y < 0) {	/* Rotate positive */
			yi = y + (x >> i);
			x  = x - (y >> i);
			y  = yi;
			theta -= *arctanptr++;
		}
		else {		/* Rotate negative */
			yi = y - (x >> i);
			x  = x + (y >> i);
			y  = yi;
			theta += *arctanptr++;
		}
	}

	return(theta);
}
