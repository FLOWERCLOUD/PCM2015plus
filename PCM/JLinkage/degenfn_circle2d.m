%------------------------------------------------------------------------
function r = degenfn_circle2d(X)
        r =  all(pdist(X) < eps);
end