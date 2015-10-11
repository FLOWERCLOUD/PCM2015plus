function [] = visualfn_cylinder(Model, dim, centre,linespec)

    cylinder_axis_start = Model(1:3);
    cylinder_axis_direction = Model(4:6);
    cylinder_radius = Model(7);

   if (nargin < 3)
      centre = cylinder_axis_start;
   end

  if (nargin < 4)
      linespec = 'r-';
   end

   n = cylinder_axis_direction / norm(cylinder_axis_direction);

   if (norm(n - [1.0, 0.0, 0.0]) > norm(n - [0.0, 1.0, 0.0]))
      q = [1.0, 0.0, 0.0];
   else
      q = [0.0, 1.0, 0.0];
   end

   r = cross(n, q);
   s = cross(n, r);

   t1 = r / norm(r);
   t2 = s / norm(s);
   t3 = (t2 + t1) / norm(t2 + t1);
   t4 = (t2 - t1) / norm(t2 - t1);

   c = proj_line(cylinder_axis_start, cylinder_axis_direction, centre);

   visual_line([c - cylinder_radius * t1, cylinder_axis_direction], dim, c - cylinder_radius * t1, linespec);
   visual_line([c + cylinder_radius * t1, cylinder_axis_direction], dim, c - cylinder_radius * t1, linespec);
   visual_line([c - cylinder_radius * t2, cylinder_axis_direction], dim, c - cylinder_radius * t1, linespec);
   visual_line([c + cylinder_radius * t2, cylinder_axis_direction], dim, c - cylinder_radius * t1, linespec);
   visual_line([c - cylinder_radius * t3, cylinder_axis_direction], dim, c - cylinder_radius * t1, linespec);
   visual_line([c + cylinder_radius * t3, cylinder_axis_direction], dim, c - cylinder_radius * t1, linespec);
   visual_line([c - cylinder_radius * t4, cylinder_axis_direction], dim, c - cylinder_radius * t1, linespec);
   visual_line([c + cylinder_radius * t4, cylinder_axis_direction], dim, c - cylinder_radius * t1, linespec);

   for i = -5:5,
      d = i * dim / 5.0;
      
      p = [c - d * n + cylinder_radius * t1;
           c - d * n + cylinder_radius * t3;
           c - d * n + cylinder_radius * t2;
           c - d * n + cylinder_radius * t4;
           c - d * n - cylinder_radius * t1;
           c - d * n - cylinder_radius * t3;
           c - d * n - cylinder_radius * t2;
           c - d * n - cylinder_radius * t4;
           c - d * n + cylinder_radius * t1];

      plot3(p(:, 1), p(:, 2), p(:, 3), linespec);
   end

end

function [] = visual_line(Model, dim, centre, linespec)
    
    line_start = Model(1:3);
    line_direction = Model(4:6);
    

   if (nargin < 4)
      centre = line_start;
   end


   n = line_direction / norm(line_direction);
   c = proj_line(line_start, line_direction, centre);

   p = [c - dim * n;
        c + dim * n];

   plot3(p(:, 1), p(:, 2), p(:, 3), linespec);
   hold on

end