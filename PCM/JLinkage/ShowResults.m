%-----------------------------------------------------------
% Plot models found by J-Linkage algorithm
%
% Usage:
%
% ShowResults(XYZ, visualfn, fittingfn, T)   
%
% Arguments:
%     XYZ              - Bidimensional or tridimensional point set.
%     fittingfn        - Handle to a function that fits a model to s
%                      data from x. 
%     visualfn         - Handle to a function that visualize a model s, fitted by
%                      the fittingfn.
%     clusterThreshold - Only plot models with a set of inliers greater
%                        than clusterThreshold
%     T                - Clustered points Labels
%
% Returns:
%               - Plotted models
%
%
% Authors: R.Toldo A.Fusiello, department of computer science - University of Verona.
% Reference Paper: R. Toldo, A. Fusiello. Robust Multiple Structures Estimation with J-linkage. Proceeding of the European Conference on Computer Vision, 2008.
%-----------------------------------------------------------
function ShowResults(XYZ, visualfn, fittingfn, clusterThreshold, T)
[dimension, nPoints] = size(XYZ);


hold off;
if(dimension == 3)
    plot3(XYZ(1,:), XYZ(2,:), XYZ(3,:), 'k+', 'MarkerSize', 2);
end
if(dimension == 2)
    plot(XYZ(1,:), XYZ(2,:), 'ko', 'MarkerSize', 2);
end
hold on;

for j=1:max(T)
    switch j
        case 1
            color = 'b*';
        case 2
            color = 'g*';
        case 3
            color = 'r*';
        case 4
            color = 'c*';
        case 5
            color = 'm*';
        case 6
            color = 'y*';
        case 7
            color = 'k*';
        case 8
            color = 'go';
        case 9
            color = 'ro';
        case 10
            color = 'co';
        case 11
            color = 'mo';
        case 12
            color = 'yo';
        case 13
            color = 'b+';
        case 14
            color = 'g+';
        case 15
            color = 'r+';
        case 16
            color = 'c+';
        case 17
            color = 'm+';
        case 18
            color = 'y+';

        otherwise
            color = 'b*';
    end


    inCurrentModel = find(T == j);
    if(length(inCurrentModel) > clusterThreshold)


        if(dimension == 3)
            plot3(XYZ(1,inCurrentModel), XYZ(2,inCurrentModel), XYZ(3,inCurrentModel), color, 'MarkerSize', 5);
        end
        if(dimension == 2)
            plot(XYZ(1,inCurrentModel),XYZ(2,inCurrentModel), color, 'MarkerSize', 5);
        end
        distances = pdist(XYZ(:,inCurrentModel)', 'euclidean');
        maxDist = max(distances) * sqrt(2) / 2;
        Centre = mean(XYZ(:,inCurrentModel), 2)';
        % Find model params from current inliers
        params = feval(fittingfn,XYZ(:,inCurrentModel));
        % Plot model
        feval(visualfn, params, maxDist, Centre,color(1));
        hold on;
    end
end

