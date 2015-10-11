function compatible = primitiveCompositefn(Model1, Model2, intersectingPts)
    compatible = false;
    if(strcmp(Model1.modelType,'Cylinder') && strcmp(Model2.modelType,'Plane'))
       cyl_radius =  Model1.params(7);
       cyl_axis_direction = Model1.params(4:6);
       cyl_start_direction = Model1.params(1:3);
       plane_normal = Model2.params(1:3);
       plane_d = Model2.params(4);
       dMax = -inf;
       dMin = inf;
       
       for i=1:size(intersectingPts,2)
            
             d = sum((intersectingPts(:,i) - cyl_start_direction') .* cyl_axis_direction');
             if(d>dMax)
                 dMax = d;
             end
             if(d<dMin)
                 dMin = d;
             end
       end
       line_start = cyl_start_direction + cyl_axis_direction * dMin;
       line_end = cyl_start_direction + cyl_axis_direction * dMax;
       
       
       % line-plane distance
       d = distfn_plane(Model2.params, [line_start;line_end]');
       
       % line-plane intersection
       t = (-plane_d - dot(line_start, plane_normal)) / (dot((line_end- line_start), plane_normal));
       if(abs(d(1) - cyl_radius) < Model1.inliersThreshold && ...
           abs(d(2) - cyl_radius) < Model1.inliersThreshold && ...
           (t > 1 || t < 0))
           compatible = true;
       end
       

    end

end