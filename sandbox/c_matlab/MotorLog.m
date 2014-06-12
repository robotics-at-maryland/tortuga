[i1 i2 i3 i4 i5 i6 s1 s2 s3 s4 s5 s6 t] = textread('/tmp/currents.log', '%n %n %n %n %n %n %n %n %n %n %n %n %n');

s1 = abs(s1);
s2 = abs(s2);
s3 = abs(s3);
s4 = abs(s4);
s5 = abs(s5);
s6 = abs(s6);


%figure; plot(t, i2, '.k'); hold on; plot(t, s2*(9/1024), '.g');
is = [i1' i2' i3' i4' i5' i6']';
ss = [s1' s2' s3' s4' s5' s6']';

m = [ss.*ss, ss, ones(length(ss), 1)];
r = inv(m'*m)*m'*is; a = r(1); b = r(2); c = r(3);

xs = (0:1024);
ys = (a*(xs.*xs)) + (b*xs) + c;

figure

plot(ss, is, '.');
hold on
plot(xs, ys, 'g-');