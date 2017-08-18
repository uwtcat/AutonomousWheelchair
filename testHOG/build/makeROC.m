%% INRIA Data Output
numIterations = 10;
% 201 is hard-coded right now for # of thresholds
sensitivity_TPR_INRIA_arr = zeros(201, numIterations);
specificity_FPR_INRIA_arr = zeros(201, numIterations);
for i=1:numIterations
    filename = ['INRIAIteration' num2str(i) 'Output.txt'];
T = readtable(filename);
C = table2array(T);
threshold_col = C(:, 1);
threshold = reshape(threshold_col, [1 size(threshold_col,1)]);

TP_col = C(:, 2);
size(TP_col);
TP = reshape(TP_col, [1 size(TP_col,1)]);

TN_col = C(:, 3);
TN = reshape(TN_col, [1 size(TN_col,1)]);

FP_col = C(:, 4);
FP = reshape(FP_col, [1 size(FP_col,1)]);

FN_col = C(:, 5);
FN = reshape(FN_col, [1 size(FN_col,1)]);

% Mean sensitivity & 1 - specificity for each threshold
sensitivity_TPR_INRIA_arr(:, i) = TP./(TP+FN);
specificity_FPR_INRIA_arr(:, i) = FP./(FP+TN);
end

sensitivity_TPR_INRIA = mean(sensitivity_TPR_INRIA_arr, 2);
specificity_FPR_INRIA = mean(specificity_FPR_INRIA_arr, 2);
%% ZED Data Curated
sensitivity_TPR_ZED_arr = zeros(201, numIterations);
specificity_FPR_ZED_arr = zeros(201, numIterations);
for i=1:numIterations
T = readtable(['ZEDDataIteration' num2str(i) 'Output.txt']);
C = table2array(T);
threshold_col = C(:, 1);
threshold = reshape(threshold_col, [1 size(threshold_col,1)]);

TP_col = C(:, 2);
size(TP_col);
TP = reshape(TP_col, [1 size(TP_col,1)]);

TN_col = C(:, 3);
TN = reshape(TN_col, [1 size(TN_col,1)]);

FP_col = C(:, 4);
FP = reshape(FP_col, [1 size(FP_col,1)]);

FN_col = C(:, 5);
FN = reshape(FN_col, [1 size(FN_col,1)]);

sensitivity_TPR_ZED_arr(:, i) = TP./(TP+FN);
specificity_FPR_ZED_arr(:, i) = FP./(FP+TN);

end

sensitivity_TPR_ZED = mean(sensitivity_TPR_ZED_arr, 2);
specificity_FPR_ZED = mean(specificity_FPR_ZED_arr, 2);

%% ZED Data All
sensitivity_TPR_ZED_arr_all = zeros(201, numIterations);
specificity_FPR_ZED_arr_all = zeros(201, numIterations);
for i=1:numIterations
T = readtable(['ZEDDataIteration' num2str(i) 'Output_FullSet.txt']);
C = table2array(T);
threshold_col = C(:, 1);
threshold = reshape(threshold_col, [1 size(threshold_col,1)]);

TP_col = C(:, 2);
size(TP_col);
TP = reshape(TP_col, [1 size(TP_col,1)]);

TN_col = C(:, 3);
TN = reshape(TN_col, [1 size(TN_col,1)]);

FP_col = C(:, 4);
FP = reshape(FP_col, [1 size(FP_col,1)]);

FN_col = C(:, 5);
FN = reshape(FN_col, [1 size(FN_col,1)]);

sensitivity_TPR_ZED_arr_all(:, i) = TP./(TP+FN);
specificity_FPR_ZED_arr_all(:, i) = FP./(FP+TN);

end


sensitivity_TPR_ZED_all = mean(sensitivity_TPR_ZED_arr_all, 2);
specificity_FPR_ZED_all = mean(specificity_FPR_ZED_arr_all, 2);

%% Overlapping plot

fig = figure;
%plot(specificity_FPR_INRIA, sensitivity_TPR_INRIA, 'b', specificity_FPR_ZED, sensitivity_TPR_ZED, 'r', threshold, threshold, '--k');
plot(specificity_FPR_INRIA, sensitivity_TPR_INRIA, 'b', specificity_FPR_ZED, sensitivity_TPR_ZED, 'r', specificity_FPR_ZED_all, sensitivity_TPR_ZED_all, 'g');
legend('INRIA test data', 'ZED test data outside only', 'ZED test data all', 'Location', 'northwest');
title('ROC Curve', 'FontSize', 24);
xlabel('False Positive Rate (1-Specificity)', 'FontSize', 20);
xlim([0 max(specificity_FPR_INRIA)]);
ylabel('True Positive Rate (Sensitivity)', 'FontSize', 20);
print(fig, 'ROC_Figure_Output_All','-dpdf','-bestfit')