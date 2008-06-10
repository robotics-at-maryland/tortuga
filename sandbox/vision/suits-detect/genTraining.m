function [X, PHI] = genTraining

suitDir = '../../../packages/vision/test/data/images/testbin/';
suitExt = '.jpg';

suitNames = {'club', 'diamond', 'heart', 'spade'};

for i = 1:4
	suitName = suitNames{i};
	suitImg = imread(horzcat(suitDir, suitName, suitExt));
	suitImg = imresize(suitImg, 0.25);
	suitRed{i} = 255 - suitImg(:,:,1);
end

minAngle = -18;
maxAngle = 18;
stepAngle = 2;

angles = minAngle:stepAngle:maxAngle;
nAngles = length(angles);

imWidth = 64;
nIn = imWidth * 2 + 1;

PHI = zeros(4, nAngles);
X = zeros(nIn, nAngles);
X(nIn,:) = 1;

trainingIndex = 1;
for rotAngle = minAngle:stepAngle:maxAngle
	clf;
	for i = 1:4
		suitRot = imrotate(suitRed{i}, rotAngle);
		xProj = flattenToColumn(squashOrStretch(sum(suitRot), imWidth));
		yProj = flattenToColumn(squashOrStretch(sum(suitRot'), imWidth));
		X(1:(2*imWidth),trainingIndex) = vertcat(xProj, yProj);
		PHI(i, trainingIndex) = 1;
		trainingIndex = trainingIndex + 1;
	end
end
