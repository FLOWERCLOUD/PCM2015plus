function [] = visualfn_circle2d(Model, dim, centre,linespec)
    Center = Model(1:2);
    Radius = Model(3);
    
   if (nargin < 4)
      linespec = 'm-';
   end
    
   th = linspace(0,2*pi,20)';
   xe = Radius*cos(th)+Center(1); ye = Radius*sin(th)+Center(2);
   
   plot(xe, ye, linespec,  'LineWidth', 2);
   hold on
