%------------------------------------------------------------------------
% Function to determine whether a set of 7 points are in a degenerate
% configuration. 
function r = degenfn_cylinder(X)
        r =  all(pdist(X) < eps);
end