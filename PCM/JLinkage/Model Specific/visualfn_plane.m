function [] = visualfn_plane(Model, dim, centre, linespec)

   plane_normal = Model(1:3);
   plane_distance = -Model(4);
   
   if (nargin < 3)
      lines_only = 1;
   end

   if (norm(plane_normal) < 0.00001)
      disp('visual_plane: plane normal error.');
      return;
   end

   n = plane_normal / norm(plane_normal);

   if (norm(n - [1.0, 0.0, 0.0]) > norm(n - [0.0, 1.0, 0.0]))
      q = [1.0, 0.0, 0.0];
   else
      q = [0.0, 1.0, 0.0];
   end

   r = cross(n, q);
   s = cross(n, r);
   r = r / norm(r);
   s = s / norm(s);

   c = proj_plane(plane_normal, plane_distance, centre);

   idx = [1, 2, 3, 3, 4, 1];
      
   p = [c - dim * r + dim * s;
           c + dim * r + dim * s;
           c + dim * r - dim * s;
           c - dim * r - dim * s];
      
   trisurf(idx, p(:, 1), p(:, 2), p(:, 3),'FaceAlpha',0.5,'FaceColor',linespec(1));
