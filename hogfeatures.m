%% Extract and Plot HOG Features

homeFolder = pwd;
homeFolder = [homeFolder '/']
numIterations = 10;
for i=1:numIterations
cd(homeFolder);
display(['Doing Iteration: ' num2str(i)]);

% Name of file to write
filename = ['96x160matlabfeaturesIteration' num2str(i) '.dat'];
fileID = fopen(filename,'w');

% Target folders for images
%posTargetFolder = '/home/csnesummer/Documents/HOGDescriptorScripts/INRIA_Test_IMG/pos/';
%negTargetFolder = '/home/csnesummer/Documents/HOGDescriptorScripts/INRIA_Test_IMG/neg/';
posTargetFolder = ['/home/csnesummer/Documents/TrainTestIterations/Iteration' num2str(i) '/Train/pos/'];
negTargetFolder = ['/home/csnesummer/Documents/TrainTestIterations/Iteration' num2str(i) '/Train/neg/'];
cd(posTargetFolder);
posfnames = dir('*.png');
numposfids = length(posfnames);

% Read from positive files
for k=1:numposfids
   img = imread(posfnames(k).name); 
   [featureVector,hogVisualization] = extractHOGFeatures(img);
   fprintf(fileID, "%s ", "+1");
   for j=1:size(featureVector, 2) 
       fprintf(fileID, "%d:%.7f ", j, featureVector(j));
   end
   fprintf(fileID, "\n");
end

% Go to negative target folder
cd(negTargetFolder);
negfnames = dir('*.png');
numnegfids = length(negfnames);

% Read negative images
for k=1:numnegfids
    img = imread(negfnames(k).name);
    [featureVector,hogVisualization] = extractHOGFeatures(img);
    fprintf(fileID, "%s ", "-1");
    for j=1:size(featureVector, 2)
        fprintf(fileID, "%d:%.7f ", j, featureVector(j));
    end
    fprintf(fileID, "\n");
end

fclose(fileID);
end