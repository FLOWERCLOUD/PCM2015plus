function P = fittingfn_line2d(X)
N = size(X, 2);
A = [X; ones(1, N)]';
[U S V] = svd(A);
P = V(:, 3);

end