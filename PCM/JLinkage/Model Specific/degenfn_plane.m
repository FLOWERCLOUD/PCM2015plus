%------------------------------------------------------------------------
% Function to determine whether a set of 3 points are in a degenerate
% configuration. In this case they are degenerate if they are colinear.
function r = degenfn_plane(X)
        r =  norm(cross(X(:,2)-X(:,1), X(:,3)-X(:,1))) < eps;
end