function q = quaternionFromnCb(nCb)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% q = quaternionFromnCb(nCb)
%
% as the function name cleverly indicates, this function generates a
% quaternion from a direction cosine matrix
%
% input: nCb - direction cosine matrix where [v]_n = nCb * [v]_b
%
% output: q - a quaternion with the parameterization:
%
%                   [q1] = [e1*sin(et/2)]
%               q = |q2|   |e2*sin(et/2)|
%                   |q3|   |e3*sin(et/2)|
%                   [q4]   [  cos(et/2) ] 
%
%               where euler axis = [e1,e2,e3] and euler angle = et
%
% Written by Joseph Gland on June 29, 2007
% More or less directly taken from Programmer's Toolbox by Jack Crenshaw in
% the May 1994 edition of Embedded Systems Programming
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

d = 0.5*sqrt(nCb(1,1)+nCb(2,2)+nCb(3,3)+1);
if abs(d) >= 0.125
    a = 0.25*(nCb(3,2)-nCb(2,3))/d;
    b = 0.25*(nCb(1,3)-nCb(3,1))/d;
    c = 0.25*(nCb(2,1)-nCb(1,2))/d;
else
    c = 0.5*sqrt(-nCb(1,1)-nCb(2,2)+nCb(3,3)+1);
    if abs(c) >= 0.125
        a = 0.25*(nCb(1,3)-nCb(3,1))/c;
        b = 0.25*(nCb(2,3)+nCb(3,2))/c;
        d = 0.25*(nCb(2,1)-nCb(1,2))/c;
    else
        b = 0.5*sqrt(-nCb(1,1)+nCb(2,2)-nCb(3,3)+1);
        if abs(b) >= 0.125
            a = 0.25*(nCb(1,2)+nCb(2,1))/b;
            c = 0.25*(nCb(2,3)+nCb(3,2))/b;
            d = 0.25*(nCb(1,3)-nCb(3,1))/b;
        else
            a = 0.5*sqrt(nCb(1,1)-nCb(2,2)-nCb(3,3)+1);
            b = 0.25*(nCb(1,2)+nCb(2,1))/a;
            c = 0.25*(nCb(1,3)-nCb(3,1))/a;
            d = 0.25*(nCb(3,2)-nCb(2,3))/a;
        end
    end
end

if d<0
    disp('hit case where d<0')
    a = -a;
    b = -b;
    c = -c;
    d = -d;
end

q = [a b c d]';