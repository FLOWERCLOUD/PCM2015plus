%------------------------------------------------------------------------
% Function to define a cylinder given 6 data points. The cylinder is
% defined by a least square non linear solution
function [P] = fittingfn_cylinder(X)

   [s, d,od] = fit_line(X');
   if(size(X,2) < 7)
       options = optimset('Display', 'Off','LargeScale','off');
   else
       options = optimset('Display', 'Off','LargeScale','on');       
   end
%    N = d;
%    sinT = N(3); cosT =sqrt(1 - sinT^2);
%    sinO = N(1) / cosT; cosO = N(2) / cosT;
%    angleT = atan2(sinT, cosT);
%    angleO = atan2(sinO, cosO);    
% 
%   parameters = lsqnonlin(@lsq_cylinder, [s(1), s(2), s(3), angleT, angleO, norm(X(:,1) - s')], [], [], options, X);
%    cylinder_axis_start = parameters(1:3);
%    angleT = parameters(4);
%    angleO = parameters(5);
%    cylinder_radius = parameters(6);
%    cylinder_axis_direction = [sin(angleO) * cos(angleT), cos(angleO) * cos(angleT),  sin(angleT)];
%    cylinder_axis_direction = cylinder_axis_direction / norm(cylinder_axis_direction);
%P = [cylinder_axis_start cylinder_axis_direction cylinder_radius];
   P = lsqnonlin(@lsq_cylinder, [s(1), s(2), s(3), d(1), d(2),d(3), od], [], [], options, X);
   P(4:6) = P(4:6) / norm(P(4:6));

end


%------------------------------------------------------------------------
% Fit a line using covariance matrix(LSQ)
function [line_start, line_direction, od] = fit_line(points)

   o = mean(points);
   points_temp = points;

   for i = 1:size(points_temp, 1),
      points_temp(i, :) = points_temp(i, :) - o;
   end

   % covariance matrix
   [v, d] = eig(points_temp' * points_temp);
   
   % largest eigenvector is in the last column
   col = size(v, 2);  %get the number of columns

   % get the last eigenvector column and the last eigenvalue
    eVec = v(:, col);
    eVal = d(col, col);
   
   line_start = o' - sqrt(eVal)*eVec;
   line_direction = o' + sqrt(eVal)*eVec;
   od = sqrt(d(1,1))/2;
end

%------------------------------------------------------------------------
% Least square solution of the fit cylinder function
function [err] = lsq_cylinder(parameters, points)

%    cylinder_axis_start = parameters(1:3);
%    angleT = parameters(4);
%    angleO = parameters(5);
%    cylinder_radius = parameters(6);
%    cylinder_axis_direction = [sin(angleO) * cos(angleT), cos(angleO) * cos(angleT),  sin(angleT)];
%    cylinder_axis_direction = cylinder_axis_direction / norm(cylinder_axis_direction);
%       
%    err = distfn_cylinder([cylinder_axis_start cylinder_axis_direction cylinder_radius], points);
err = distfn_cylinder(parameters, points);

   
end
