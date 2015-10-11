%------------------------------------------------------------------------
% Function to calculate distances between a 2d line and an array of points.
function [dist, P] = distfn_circle2d(P, X)
    
    [d npts] = size(X);
    dist = zeros(npts,1);

    for i = 1:npts
        dist(i) = abs(norm([P(1);P(2)]-X(:,i))-P(3));
    end
    
end