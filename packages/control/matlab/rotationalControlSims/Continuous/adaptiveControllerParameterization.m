%helps determine parameterization matrix

%note that this doesn't include buoyancy or drag

syms H11 H12 H13 H22 H23 H33 wdr1 wdr2 wdr3 w1 w2 w3 wr1 wr2 wr3 real

H=[H11 H12 H13; H12 H22 H23; H13 H23 H33];

wdr=[wdr1 wdr2 wdr3]';

wr=[wr1 wr2 wr3]';

w=[w1 w2 w3]';

H*wdr-S(H*w)*wr

