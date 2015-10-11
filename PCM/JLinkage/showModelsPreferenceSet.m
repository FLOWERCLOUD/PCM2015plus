%-----------------------------------------------------------
% Plot preference set matrix from the output of a J-Linkage clusterization.
%
% Authors: R.Toldo A.Fusiello, department of computer science - University of Verona.
% Reference Paper: R. Toldo, A. Fusiello. Robust Multiple Structures Estimation with J-linkage. Proceeding of the European Conference on Computer Vision, 2008.
%-----------------------------------------------------------
function showModelsPreferenceSet(totdbin, T)
ClusterSize = zeros(1,max(T));

% Sort cluster
for i=1:max(T)
    ClusterSize(i) = length(find(T == i));
end
[ClusterSize I] = sort(ClusterSize,'descend');

nT = T;

for i=1:max(T)
    nT(find(T == I(i))) = i;
end
    
T = nT;
ntotdbin = totdbin;

index = 1;
for i=1:max(T)
    inCurrent = find(T==i);
    
    for j=1:length(inCurrent) %npts
        nT(index) = i;
        ntotdbin(index,:) = totdbin(inCurrent(j),:);
        index = index + 1;
    end
     
end

ClusterIntersect = ones(max(T), size(totdbin,2));

for i=1:size(totdbin,1) %npts
    ClusterIntersect(nT(i),:) = ClusterIntersect(nT(i),:) & ntotdbin(i,:);
end
% Sort Models
beginMod = 1;

totdbin = ntotdbin;

unionFM = [];
for i=1:max(T)
    finalModels = find(ClusterIntersect(i,:) == 1);
    unionFM = union(unionFM,finalModels);
    ntotdbin(:,beginMod:beginMod+length(finalModels) - 1) = totdbin(:,finalModels);
    beginMod = beginMod+length(finalModels);
end
tot = [1:size(ntotdbin,2)];
ntotdbin(:,beginMod:size(ntotdbin,2)) = totdbin(:,setDiff(tot,unionFM));
span = max(beginMod*1.1, size(ntotdbin,1)*2);
span = min(size(ntotdbin,2), span);

spy(ntotdbin(:,1:size(ntotdbin,2)),'r',1)




