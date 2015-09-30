addpath(genpath('./'));
load JLinkageExamples.mat;

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


Points = Star5_S00075_O50;
Lines = totm;
