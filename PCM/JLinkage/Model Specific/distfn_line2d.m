%------------------------------------------------------------------------
% Function to calculate distances between a 2d line and an array of points.
function [dist, P] = distfn_line2d(P, X)
        
    [dummy npts] = size(X);
    dist = zeros(npts,1);
    for i = 1:npts
        dist(i) = sqrt((P(1:2)'*X(:,i)+P(3)).^2  / (P(1)^2+P(2)^2));
    end
    
end