%------------------------------------------------------------------------
% Function to calculate distances between a cylinder and an array of points.
function [dist, P] = distfn_cylinder(P, X)

cylinder_axis_start = P(1:3);
cylinder_axis_direction = P(4:6);
cylinder_radius = P(7);

[dimension npts] = size(X);
dist = zeros(npts,1);
for i = 1:npts
    dist(i) = norm(X(:,i) - proj_cylinder(cylinder_axis_start, cylinder_axis_direction, cylinder_radius, X(:,i)));
end

end