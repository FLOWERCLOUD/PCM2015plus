%-----------------------------------------------------------
% Plot Clusters sizes in a histogram form
%
% Usage:
%
% showClustersModelsSizeHistogram(T, totdbin)
%
% Arguments:
%     T                - Clustered points Labels
%   
% Authors: R.Toldo A.Fusiello, department of computer science - University of Verona.
% Reference Paper: R. Toldo, A. Fusiello. Robust Multiple Structures Estimation with J-linkage. Proceeding of the European Conference on Computer Vision, 2008.
%-----------------------------------------------------------
function showClustersModelsSizeHistogram(T)
    nTPts = zeros(max(T),1);
    nT = zeros(size(T));
    for i=1:max(T)
        nTPts(i) = length(find(T == i));    
    end

    [nTPts I] = sort(nTPts,'descend');
    for i=1:max(T)
        nT(find(T == I(i))) = i;
    end
    hist(nT,max(nT))

end