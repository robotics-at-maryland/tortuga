function qtilde = quaternionProduct(q1,q2)
% qtilde = quaternionCrossProduct(q1,q2)
%
% computes the product between quaternion
% q1 and quaternion q2, effectively describes the 
% rotation from frame q2 to q1
% 
% often used to calculate the difference between quaternions
%

qtilde=[q2(4)*eye(3)-S(q2(1:3)) -q2(1:3);
        -q2(1:3)' q2(4)]*q1;
    
%normalize

qtilde=qtilde/norm(qtilde,2);