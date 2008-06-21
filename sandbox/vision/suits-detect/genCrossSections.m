function genCrossSections(imWidth, fname)

suitDir = '../../../packages/vision/test/data/images/testbin/';
suitExt = '.jpg';

suitNames = {'club', 'diamond', 'heart', 'spade'};

for i = 1:4
	suitName = suitNames{i};
	suitImg = imread(horzcat(suitDir, suitName, suitExt));
	#suitImg = imresize(suitImg, 0.25);
	suitRed{i} = 255 - suitImg(:,:,1);
end

file = fopen(fname, 'w');

for i = 1:4
	xProj = flattenToColumn(squashOrStretch(sum(suitRed{i}), imWidth));
	yProj = flattenToColumn(squashOrStretch(sum(suitRed{i}'), imWidth));
	
	fprintf(file, '// projection of the %s onto the horizontal axis\n', suitNames{i});
	fprintf(file, 'unsigned int xProj_%s[%d] = {', suitNames{i}, imWidth);
	for j = 1:(imWidth - 1)
		fprintf(file, '%d, ', xProj(j));
	end
	fprintf(file, '%d};\n', xProj(imWidth));
	
	fprintf(file, '// projection of the %s onto the vertical axis\n', suitNames{i});
	fprintf(file, 'unsigned int yProj_%s[%d] = {', suitNames{i}, imWidth);
	for j = 1:(imWidth - 1)
		fprintf(file, '%d, ', yProj(j));
	end
	fprintf(file, '%d};\n', yProj(imWidth));

	fprintf(file, '\n');
end

fclose(file);
