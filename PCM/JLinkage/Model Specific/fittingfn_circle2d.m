function P = fittingfn_circle2d(X)
x=X(1,:)'; y=X(2,:)';
a=[x y ones(size(x))]\[-(x.^2+y.^2)];
P(1) = -.5*a(1);
P(2) = -.5*a(2);
P(3)  =  sqrt((a(1)^2+a(2)^2)/4-a(3));

end