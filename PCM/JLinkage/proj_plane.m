function [p] = proj_plane(plane_normal, plane_distance, point)

   p = point;

   n = plane_normal / norm(plane_normal);
   d = sum(point .* n) - plane_distance;
   p = point - n * d;
