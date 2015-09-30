%------------------------------------------------------------------------
% Function to define a plane given 3 data points. The plane is defined directly by
% the three points.
function P = getfn_plane(X)
    n = cross(X(:,2)-X(:,1), X(:,3)-X(:,1)); % Plane normal.
    d = - (n(1) * X(1,1)) - (n(2) * X(2,1)) - (n(3) * X(3,1)); 
    normN = norm(n);                           % Make it a unit vector
    n = n/normN;
    d = d/normN;
    P = [n' d];
end 
