%-----------------------------------------------------------
% Cluster Points using the J-Linkage algorithm
%
% Usage:
%
% [T, Z, Y, totdbin] = clusterPoints(Points,inliersThreshold,totd)
%
% Arguments:
%     totd             - Point-Model distance matrix
%     inliersThreshold - Ransac inlier Threshold value.
%
% Returns:
%     T         - Clustered points Labels
%     Z         - Hierarchical clustering tree
%     Y         - Pairwise Jaccard Distance
%     totdbin   - Consensus/Preference set matrix
%
% Authors: R.Toldo A.Fusiello, department of computer science - University of Verona.
% Reference Paper: R. Toldo, A. Fusiello. Robust Multiple Structures Estimation with J-linkage. Proceeding of the European Conference on Computer Vision, 2008.
%-----------------------------------------------------------
function [T, Z, Y, totdbin] = clusterPoints(totd, inliersThreshold)

corM = find(totd < inliersThreshold);
totdbin = false(size(totd));
totdbin(corM) = true;

Y = pDistJaccard(totdbin');
Z = linkageIntersect(Y, totdbin);
T = cluster(Z,'cutoff',1-(1/(size(totdbin,1)))+eps,'criterion','distance');

