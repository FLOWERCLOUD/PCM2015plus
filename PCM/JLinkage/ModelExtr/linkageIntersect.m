%-----------------------------------------------------------
% Generate Create hierarchical cluster tree Z using the J-Linkage algorith,
% OBSOLETE: slow matlab version, please use the c(mex) version
% Usage:
%
% Z = linkageIntersect(Y, totd)
%
% Arguments:
%     y 	 -  Pairwise Jaccard Distance 
%    totd       - Consensus/Preference set matrix
%
% Returns:
%     Z          - hierarchical cluster tree
%
%
% Authors: R.Toldo A.Fusiello, department of computer science - University of Verona.
% Reference Paper: R. Toldo, A. Fusiello. Robust Multiple Structures Estimation with J-linkage. Proceeding of the European Conference on Computer Vision, 2008.
%-----------------------------------------------------------
function Z = linkageIntersect(Y, totd)

[k, n] = size(Y);
m = ceil(sqrt(2*n)); % (1+sqrt(1+8*n))/2, but works for large n
Z = zeros(m-1,3); % allocate the output matrix.

% during updating clusters, cluster index is constantly changing, R is
% a index vector mapping the original index to the current (row, column)
% index in Y.  N denotes how many points are contained in each cluster.
N = zeros(1,2*m-1);
N(1:m) = 1;
n = m; % since m is changing, we need to save m in n.
R = 1:n;

hw  =   waitbar(0,'Clustering Points ...');

for s = 1:(n-1)
    waitbar(s/n,hw);
   [v, k] = min(Y);

   i = floor(m+1/2-sqrt(m^2-m+1/4-2*(k-1)));
   j = k - (i-1)*(m-i/2)+i;

   Z(s,:) = [R(i) R(j) v]; % update one more row to the output matrix A

   % Update Y. In order to vectorize the computation, we need to compute
   % all the indices corresponding to cluster i and j in Y, denoted by I
   % and J.
   I1 = 1:(i-1); I2 = (i+1):(j-1); I3 = (j+1):m; % these are temp variables
   U = [I1 I2 I3];
   I = [I1.*(m-(I1+1)/2)-m+i i*(m-(i+1)/2)-m+I2 i*(m-(i+1)/2)-m+I3];
   J = [I1.*(m-(I1+1)/2)-m+j I2.*(m-(I2+1)/2)-m+j j*(m-(j+1)/2)-m+I3];

   newCol = totd(i,:) & totd(j,:);
   totd(i,:) = newCol;
   Y(I) = distJaccard(totd(i,:), totd(U,:));
    
   J = [J i*(m-(i+1)/2)-m+j];
   Y(J) = [];
   totd(j,:) = [];
   
   % update m, N, R
   m = m-1;
   N(n+s) = N(R(i)) + N(R(j));
   R(i) = n+s;
   R(j:(n-1))=R((j+1):n);
end

close(hw);

Z(:,[1 2])=sort(Z(:,[1 2]),2);
