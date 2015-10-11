%-----------------------------------------------------------
% Generate hypotesis for the J-Linkage algorithm
%
% Usage:
%
% [totm, totd] = generateHypothesis(x, fittingfn, distfn, degenfn, s, maxDataTrials, maxTrials, nearPtsTab)   
%
% Arguments:
%     x         - Data sets
%     fittingfn - Handle to a function that fits a model to s
%                 data from x. 
%     distfn    - Handle to a function that evaluates the
%                 distances from the model to data x.
%     degenfn   - Handle to a function that determines whether a
%                 set of datapoints will produce a degenerate model.
%     s         - The minimum number of samples from x required by
%                 fittingfn to fit a model.
%     d         - Number of parameters needed by the selected model.
%     maxDataTrials - Maximum number of attempts to select a non-degenerate
%                     data set.
%     maxTrials - Numbers of total hypotesys 
%     nearPtsTab - optional. Table containing the points neighborhood.
%                 Element (i,j) is 0 if i is not close enough to j, 1
%                 otherwise. If not specified the algorithm will not use
%                 neighboorhood information to generate hypotesis.
%
% Returns:
%     totd      - Residuals
%     totm      - Hypoyesis generated
%
%
% Authors: R.Toldo A.Fusiello, department of computer science - University of Verona.
% Reference Paper: R. Toldo, A. Fusiello. Robust Multiple Structures Estimation with J-linkage. Proceeding of the European Conference on Computer Vision, 2008.
%-----------------------------------------------------------
function [totm, totd] = generateHypothesis(x, fittingfn, distfn, degenfn, s, d, maxDataTrials, maxTrials, nearPtsTab)    

  
  [dimension, npts] = size(x);                 

  totd = zeros(npts,maxTrials); % contains the distance points-hypotesys
  totm = zeros(d,maxTrials); % contains all the hypotesys generated

  if(nargin == 7)
      nearPtsTab = ones(npts, npts);
  end
  
  trialcount = 1;
 
  hw  =   waitbar(0,'Generating Hypotesis ...');

  while trialcount <= maxTrials
      
    
    waitbar(trialcount/maxTrials,hw);
    % Select at random s datapoints to form a trial model, M.
    degenerate = 1;
    count = 1;
    while degenerate && count <= maxDataTrials;
        
      % Generate 1 random index in the range 1..npts
      ind = randsample(npts, 1);
      
      nearPointsCdf = nearPtsTab(ind,:);
      
      rnd = rand(s,1);
      [dum, ind] = histc(rnd,[0 nearPointsCdf]);
      curModSample = x(:,ind);
      degenerate = 1;
      if(length(ind) == length(unique(ind)))
        degenerate = feval(degenfn, curModSample);
      end
      % Test that these points are not a degenerate configuration.
      	    
      if ~degenerate;
        % Fit model to this random selection of data points.
        M = feval(fittingfn, curModSample);
      end

      count = count + 1;
    end

    if degenerate;
      warning ( 'MATLAB:ransac:Output', ...
                'Unable to select a nondegenerate data set!' );
      break;
    end

    % Evaluate distances between points and model returning the indices
    % of elements in x that are inliers. Hold in totm the generated hypotesis 
    % and in totd the distance points-hypotesis
    totd(:,trialcount) = feval(distfn, M, x);
    totm(:,trialcount) = M;
    trialcount = trialcount + 1;  
    
  end

  close(hw);
end