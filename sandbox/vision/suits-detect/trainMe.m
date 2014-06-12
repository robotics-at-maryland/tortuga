function [A,B] = trainMe(X, PHI)

nIn = size(X,1);
nOut = size(PHI,1);
nHidden = 12;
m = size(X,2);

[A, B] = ann_initialize(nIn, nHidden, nOut);

nGenerationsPerEpoch = 40;
nEpochs = 20;
mu = 0.0001;
k = 4;
for generation = 1:1000
  [A,B] = ann_trainDescend(X, A, B, transfer_logsig, PHI, mu, k);
  Y = ann_evaluate(X, A, B, transfer_logsig);
  err(generation) = sqrt(sum(sum((Y - PHI).^2))) / m;
  [scratch maxYindices] = max(Y);
  [scratch maxPHIindices] = max(PHI);
  nCorrect = sum(maxYindices == maxPHIindices);
  fracCorrect = nCorrect / m;
  [A,B] = ann_perturb(A, B, 0.00001*(err(generation)/100)^2, 0);
  plot(err(max(1,end-nGenerationsPerEpoch):end),'LineWidth',2);
  title(sprintf('Generation %d, %f%% correct',
		generation, 100*fracCorrect));
  drawnow;
  if fracCorrect > 0.9925
    return;
  end
end
