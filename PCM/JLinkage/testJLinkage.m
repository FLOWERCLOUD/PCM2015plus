%-----------------------------------------------------------
% Test J-Linkage algorithm
%
% Authors: R.Toldo A.Fusiello, department of computer science - University of Verona.
% Reference Paper: R. Toldo, A. Fusiello. Robust Multiple Structures Estimation with J-linkage. Proceeding of the European Conference on Computer Vision, 2008.
%-----------------------------------------------------------
addpath(genpath('./'));
load JLinkageExamples.mat

numberOfTrials = 5000;
inliersThreshold = 0.05;
sigmaExp = 0.2;
ClusterThreshold = 25;

%%%%%%% Star5_S00075_O50 Example - Lines

% Generate an exponential Cumulative distribution function(cdf)
% needed to generate a non-uniform sampling
[nearPtsTab] = calcNearPtsTab(Star5_S00075_O50, 'exp', sigmaExp);

% Generate Hypothesis(random sampling)
[totm, totd] = generateHypothesis(Star5_S00075_O50, @fittingfn_line2d, @distfn_line2d, @degenfn_line2d, 2, 3, 100,numberOfTrials, nearPtsTab);

% Perform J-Linkage clusterization
[T, Z, Y, totdbin] = clusterPoints(totd, inliersThreshold);

% Plot results
figure(1);clf;
ShowResults(Star5_S00075_O50, @visualfn_line2d, @fittingfn_line2d, ClusterThreshold, T);
% Plot model sizes histogram
figure(2);clf;
showClustersModelsSizeHistogram(T);
% Plot preference set matrix
figure(3);clf;
showModelsPreferenceSet(totdbin, T);

pause;

%%%%%%% Castelvecchio Example - Planes
ClusterThreshold = 75;
inliersThreshold = 0.1;
sigmaExp = 0.5;

% Generate an exponential Cumulative distribution function(cdf)
% needed to generate a non-uniform sampling
[nearPtsTab] = calcNearPtsTab(CastelVecchio, 'exp', sigmaExp);

% Generate Hypothesis(random sampling)
[totm, totd] = generateHypothesis(CastelVecchio, @getfn_plane, @distfn_plane, @degenfn_plane, 3, 4, 100,numberOfTrials, nearPtsTab);

% Perform J-Linkage clusterization
[T, Z, Y, totdbin] = clusterPoints(totd, inliersThreshold);

% Plot results
figure(1);clf;
ShowResults(CastelVecchio, @visualfn_plane, @fittingfn_plane, 100, T);
% Plot model sizes histogram
figure(2);clf;
showClustersModelsSizeHistogram(T);
% Plot preference set matrix
figure(3);clf;
showModelsPreferenceSet(totdbin, T);
