%-----------------------------------------------------------
% Generate a Cumulative distribution function(cdf) table needed by the
% generateHypotesis algorithm
%
% Usage:
%
% [nearPtsTab, range, minpwmean] = calcNearPtsTab(Points, method, par1, par2)
%
% Arguments:
%     Points             - Data set
%     method             - method / function used
%     par1               - first parameter used by specified 'method'
%     par2               - optional: second parameter used by specified 'method'
%     D                  - distance matrix
%
%     Accepted method are:
%           'exp'            Exponential cdf function where 'par1' is alpha 
%                            of p(x|y) = exp(-(dist(x,y))/alpha)
%           'max'            The cdf is a box uniform function 
%                            bounded by the minimun distance for each
%                            points that contains a maximum of 'par1' neighboard
%           'max':           The cdf is a box uniform function 
%                            bounded by the minimun distance for each
%                            points that contains a maximum of 'par1'
%                            neighboard, excluding 'par2' points.
%           'min':           The cdf is a box uniform function 
%                            bounded by the minimun distance for each
%                            points that contains at least 'par1' neighboard
%           'mean'           The cdf is a box uniform function 
%                            bounded by the minimun distance for each
%                            points that contains a mean number of 'par1' neighboards 
%
% Authors: R.Toldo A.Fusiello, department of computer science - University of Verona.
% Reference Paper: R. Toldo, A. Fusiello. Robust Multiple Structures Estimation with J-linkage. Proceeding of the European Conference on Computer Vision, 2008.
%-----------------------------------------------------------
function [nearPtsTab, range, minpwmean] = calcNearPtsTab(Points, method, par1, par2,D)

if(nargin < 2)
    return;
end

if(nargin < 5)
    K = pdist([Points]', 'euclidean');
    D = squareform(K);
    K = [];
end
range = [];
minpwmean = [];

switch(method)
    case 'max'
        sortedD = sort(D(1:size(Points,2),1:size(Points,2)),1,'ascend');
        range = max(sortedD(par1,:));
        nearPtsTab = zeros(size(D,1), size(D,2));
        nearPtsTab(find(D < range)) = 1;
        minpwmean = mean(sortedD(2,:));
    case 'min'
        sortedD = sort(D(1:size(Points,2),1:size(Points,2)),1,'ascend');
        range = min(sortedD(par1,:));
        nearPtsTab = zeros(size(D,1), size(D,2));
        nearPtsTab(find(D < range)) = 1;
        minpwmean = mean(sortedD(2,:));
    case 'mean'
        sortedD = sort(D(1:size(Points,2),1:size(Points,2)),1,'ascend');
        range = mean(sortedD(par1,:));
        nearPtsTab = zeros(size(D,1), size(D,2));
        nearPtsTab(find(D < range)) = 1;
        minpwmean = mean(sortedD(2,:));
    case 'maxkout'
       sortedD = sort(D(1:size(Points,2),1:size(Points,2)),1,'ascend');
       sortCol = sort(sortedD(par1,:),'descend');
        range = sortCol(par2);
        nearPtsTab = zeros(size(D,1), size(D,2));
        nearPtsTab(find(D < range)) = 1;
        minpwmean = mean(sortedD(2,:));
    case 'exp'
        nearPtsTab = exp(-(D.^2)/par1);
    otherwise
        nearPtsTab = exp(-(D.^2)/par1);
end

for i=1:size(D,1)
    nearPtsTab(i,i) = 0;
    nearPtsTab(i,:) = nearPtsTab(i,:) / sum(nearPtsTab(i,:));   
    nearPtsTab(i,:) = cumsum(nearPtsTab(i,:));   
    
end


end
