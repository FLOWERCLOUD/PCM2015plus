function [p] = proj_line(line_start, line_direction, point)

   n = line_direction / norm(line_direction);
   d = sum((point - line_start) .* n);
   p = line_start + n * d;
