% Set vector of potential threshold values. 
v = [0:0.005:1.0];

% Create file to write
fileID = fopen('thresholds.dat', 'w');
nrows = size(v, 2);

% Write values for thresholds
for i = 1:nrows
   fprintf(fileID, "%f\n", v(i)); 
end

% Close file
fclose(fileID);