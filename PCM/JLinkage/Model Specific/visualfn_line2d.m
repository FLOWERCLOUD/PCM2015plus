function [] = visualfn_line2d(Model, dim, centre,linespec)
    
    if(Model(2) ~= 0)
        q = [0 -Model(3)/Model(2)];
    else
        q = [-Model(3)/Model(1) 0];
    end
    n = [-Model(2) Model(1)];

   if (nargin < 3)
      centre = q;
   end
   if (nargin < 4)
      linespec = 'm-';
   end

   n = n / norm(n);
   d = sum((centre - q) .* n);
   c = q + n * d;
   
   p = [c  - dim * n;
        c  + dim * n];

   plot(p(:, 1), p(:, 2), linespec,  'LineWidth', 2);
   hold on
