%------------------------------------------------------------------------
% Function to calculate distances between a plane and an array of points.
function [dist, P] = distfn_plane(P, X)
    
    n = P(1:3);
    d = P(4);
    
    [dummy npts] = size(X);
    dist = zeros(npts,1);
    for i = 1:npts
        dist(i) = abs(dot(n',  X(:,i)) + d);        % Distance
    end
    
end