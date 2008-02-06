function tf = isWellPacked(hydro_pos, max_dist)
siz = size(hydro_pos);
hydro_pos2 = zeros(siz(1)+1,3);
hydro_pos2(2:end,:) = hydro_pos;
m = siz(1);
tf = true;
for i = 1:m
    dif = hydro_pos2 - circshift(hydro_pos2,3*m);
    if max(dot(dif,dif,2)) > max_dist^2
        tf = false;
        break;
    end
end
