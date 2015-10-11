function [p] = proj_cylinder(cylinder_axis_start, cylinder_axis_direction, cylinder_radius, point)

   p = proj_line(cylinder_axis_start, cylinder_axis_direction, point')';
   n = point - p;

   if (norm(n) < eps)
      if (norm(cylinder_axis_direction - [1.0, 0.0, 0.0]) > norm(cylinder_axis_direction - [0.0, 1.0, 0.0]))
         n = cross(cylinder_axis_direction, [1.0, 0.0, 0.0]);
      else
         n = cross(cylinder_axis_direction, [0.0, 1.0, 0.0]);
      end
   end

   p = p + n / norm(n) * cylinder_radius;
