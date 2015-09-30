function [P] = fittingfn_plane(X)

  [rows,npts] = size(X);    
  
  if rows ~=3
    error('data is not 3D');
  end
  
  if npts < 3
    error('too few points to fit plane');
  end
  
  % Set up constraint equations of the form  AB = 0,
  % where B is a column vector of the plane coefficients
  % in the form   b(1)*X + b(2)*Y +b(3)*Z + b(4) = 0.
  
   o = mean(X');
   points_temp = X';

   for i = 1:size(points_temp, 1),
      points_temp(i, :) = points_temp(i, :) - o;
   end

   [v, d] = eig(points_temp' * points_temp);
   [m, i] = min(diag(d));

   P(1:3) = (v(:, i)' / norm(v(:, i)));
   P(4) = -sum(o .* P(1:3)); 

