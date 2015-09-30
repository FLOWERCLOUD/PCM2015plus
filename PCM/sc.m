function [cluster_labels] = sc(data, num_neighbors, block_size, sigma, num_clusters)

%GEN_NN_DISTANCE Generate (t-nearest-neighbor) sparse distance matrix.
%
%   Input  : data         : N-by-D data matrix, where N is the number of data,
%                           D is the number of dimensions
%            num_neighbors: number of nearest neighbors
%            block_size   : block size for partitioning data matrix
%            save_type    : 0 for .mat file, 1 for .txt file, 2, for both
%
%   Output : Either .mat or .txt or both files
%
%   Author : Wen-Yen Chen (wychen@alumni.cs.ucsb.edu)
%      Chih-Jen Lin (cjlin@csie.ntu.edu.tw)

%
% Divide data into blocks, process each block sequentially to alleviate memory use
%
tic;
disp('Computing non-symmetric distances matrix...')
n = size(data, 1);
num_iter = ceil(n/block_size);
disp(['Number of iterations: ', num2str(num_iter)]);
A = sparse(n, n);
dataT = data';

% For Euclidean distance, computing data.*data outside the loop to save time
tmp = full(sum(data.*data, 2));
aa = tmp(:, ones(block_size, 1));
clear tmp;

for i = 1:num_iter
  if (mod(i,100) == 0)
    disp(i);
    toc;
  end
  start_index = 1 + (i-1)*block_size;
  end_index = min(i*block_size, n);
  num_data = end_index - start_index + 1;

  % Select a block of data, fetching in column order is faster
  block = dataT(:, start_index:end_index);

  % Compute Euclidean distance between block and data
  if (num_data < block_size)
    aa = aa(:, 1:num_data);
  end
  tmp = full(sum(block.*block, 1));
  bb = tmp(ones(n, 1), :);
  clear tmp;
  ab = full(data*block);
  dist = aa + bb - 2*ab;
  clear bb ab block;
  dist = max(dist, 0);

  % Find nearest neighbors
  [value index] = sort(dist, 1);
  tempindex = index(2:num_neighbors+1, :);
  rowindex = reshape(tempindex, size(tempindex, 1)*size(tempindex, 2), 1);
  tempindex = repmat(1:num_data, num_neighbors, 1);
  columnindex = reshape(tempindex, size(tempindex, 1)*size(tempindex, 2), 1);
  tempvalue = value(2:num_neighbors+1, :);
  value = reshape(tempvalue, size(tempvalue, 1)*size(tempvalue, 2), 1);
  value = sqrt(max(value, 1.0e-12));
  A(:, start_index:end_index) = sparse(rowindex, columnindex, double(value), n, num_data);
end
clear data dataT tempindex rowindex columnindex tempvalue value;
toc;

%
% Make the sparse distance matrix symmetric
%
disp('Computing symmetric distance matrix...')
A1 = triu(A);
A1 = A1 + A1';
A2 = tril(A);
A2 = A2 + A2';
clear A;
max_num = 100000;
if (n < max_num)
  A = max(A1, A2);
else % Do 'max' function sequentially for very large data
  num_iter = ceil(n/max_num);
  B = sparse([]);
  for i = 1:num_iter
    disp(i);
    start_index = 1 + (i-1)*max_num;
    end_index = min(i*max_num, n);
    B = max(A1(:, start_index:end_index), A2(:, start_index:end_index));
    % temp matrix for saving memory use
    tmpfile = ['tmp_', num2str(i), '.mat'];
    save(tmpfile, 'B');
    clear B;
  end
end
clear A1 A2;
toc;

%
% Concatenate all temp matrices
%
disp('Concatenating temp matrices...');
if (n > max_num)
  A = sparse([]);
  for i = 1:num_iter
    tmpfile = ['tmp_', num2str(i), '.mat'];
    load(tmpfile); % temp matrix 'B'
    A = [A B];
    clear B;
  end
end
delete tmp*;

%
% Force symmetric matrix's diagonal to be 0
%
n = size(A, 1);
B = spdiags(diag(A), 0, n, n);
A = A - B;




%SC Spectral clustering using a sparse similarity matrix (t-nearest-neighbor).
%
%   Input  : A              : N-by-N sparse distance matrix, where
%                             N is the number of data
%            sigma          : sigma value used in computing similarity,
%                             if 0, apply self-tunning technique
%            num_clusters   : number of clusters
%
%   Output : cluster_labels : N-by-1 vector containing cluster labels
%            evd_time       : running time for eigendecomposition
%            kmeans_time    : running time for k-means
%            total_time     : total running time
%
%   Author : Wen-Yen Chen (wychen@alumni.cs.ucsb.edu)
%			 Chih-Jen Lin (cjlin@csie.ntu.edu.tw)

%
% Convert the sparse distance matrix to a sparse similarity matrix,
% where S = exp^(-(A^2 / 2*sigma^2)).
% Note: This step can be ignored if A is sparse similarity matrix.
%
disp('Converting distance matrix to similarity matrix...');
tic;
n = size(A, 1);

if (sigma == 0) % Selftuning spectral clustering
  % Find the count of nonzero for each column
  disp('Selftuning spectral clustering...');
  col_count = sum(A~=0, 1)';
  col_sum = sum(A, 1)';
  col_mean = col_sum ./ col_count;
  [x y val] = find(A);
  A = sparse(x, y, -val.*val./col_mean(x)./col_mean(y)./2);
  clear col_count col_sum col_mean x y val;
else % Fixed-sigma spectral clustering
  disp('Fixed-sigma spectral clustering...');
  A = A.*A;
  A = -A/(2*sigma*sigma);
end

% Do exp function sequentially because of memory limitation
num = 2000;
num_iter = ceil(n/num);
S = sparse([]);
for i = 1:num_iter
  start_index = 1 + (i-1)*num;
  end_index = min(i*num, n);
  S1 = spfun(@exp, A(:,start_index:end_index)); % sparse exponential func
  S = [S S1];
  clear S1;
end
clear A;
toc;

%
% Do laplacian, L = D^(-1/2) * S * D^(-1/2)
%
disp('Doing Laplacian...');
D = sum(S, 2) + (1e-10);
D = sqrt(1./D); % D^(-1/2)
D = spdiags(D, 0, n, n);
L = D * S * D;
clear D S;
time1 = toc;

%
% Do eigendecomposition, if L =
%   D^(-1/2) * S * D(-1/2)    : set 'LM' (Largest Magnitude), or
%   I - D^(-1/2) * S * D(-1/2): set 'SM' (Smallest Magnitude).
%
disp('Performing eigendecomposition...');
OPTS.disp = 0;
[V, val] = eigs(L, num_clusters, 'LM', OPTS);
time2 = toc;

%
% Do k-means
%
disp('Performing kmeans...');
% Normalize each row to be of unit length
sq_sum = sqrt(sum(V.*V, 2)) + 1e-20;
U = V ./ repmat(sq_sum, 1, num_clusters);
clear sq_sum V;
cluster_labels = k_means(U, [], num_clusters);
total_time = toc;

%
% Calculate and show time statistics
%
evd_time = time2 - time1
kmeans_time = total_time - time2
total_time
disp('Finished!');
