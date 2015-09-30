%------------------------------------------------------------------------
function r = degenfn_line2d(X)
        r =  any(pdist(X) < eps);
end