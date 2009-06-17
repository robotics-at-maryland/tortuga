function qtilde = quaternionProduct(q1,q2)
% qtilde = quaternionProduct(q1,q2)
%
% computes the tensor product between quaternion
% q1 and quaternion q2, effectively describes the 
% rotation from frame q2 to q1
% 
% often used to calculate the difference between quaternions
%

epsilon1=[q1(1) q1(2) q1(3)]';
eta1=q1(4);

epsilon2=[q2(1) q2(2) q2(3)]';
eta2=q2(4);


qtilde=[eta2*eye(3)-S(epsilon2) -epsilon2;
        epsilon2' eta2];
    
qtilde=qtilde*[epsilon1; eta1];
    
%normalize

qtilde=qtilde/norm(qtilde,2);