function rotateSuits

suitDir = '../../packages/vision/test/data/images/testbin/';
suitExt = '.jpg';

suitNames = {'club', 'diamond', 'heart', 'spade'};

for i = 1:4
	suitName = suitNames{i};
	suitImg = imread(horzcat(suitDir, suitName, suitExt));
	suitImg = imresize(suitImg, 0.25);
	suitRed{i} = 255 - suitImg(:,:,1);
end

for rotAngle = 0:2:90
	clf;
	for i = 1:4
		suitRot = imrotate(suitRed{i}, rotAngle);
		subplot(2,4,i);plot(sum(suitRot));
		axis off;title(horzcat(suitNames{i}, ', x-projection'));
		subplot(2,4,4+i);plot(sum(suitRot'));
		axis off;title(horzcat(suitNames{i}, ', y-projection'));
		print(sprintf('rot%d.pdf',rotAngle),'-dpdf');
	end
end
