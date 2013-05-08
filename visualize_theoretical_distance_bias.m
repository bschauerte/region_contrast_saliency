% Visualize the implicit center bias of the model that is introduced
% by using a Gaussian to weight the distances.
%
% To this end, on a regular grid: calculate the sum of distances of each
% point to all other points on the grid.
%
% If you use any of this work in scientific research or as part of a
% larger software system, you are requested to cite the use in any
% related publications or technical documentation. The work is based
% upon:
%
%   B. Schauerte, R. Stiefelhagen, "How the Distribution of Salient Objects
%   in Images Influences Salient Object Detection". In Proceedings of the
%   20th International Conference on Image Processing (ICIP), 2013.
%
% @author B. Schauerte
% @date   2012,2013

%% calculate different distance grids/sums
M   = 25;
N   = 25;
DS  = zeros(M,N);
DIS = zeros(M,N);
DGS = zeros(M,N);
DAS = zeros(M,N);
DMS = zeros(M,N);
DCS = zeros(M,N);
DTS = zeros(M,N);

[I,J] = meshgrid(1:M,1:N);
for i = 1:M
  for j = 1:N
    D = sqrt(((I - i) / M).^2 + ((J - j) / N).^2); % euclidean distance to all points
    
    DS(i,j)  = sum(D(:));                    % Euclidean distance
    DIS(i,j) = sum(1 ./ (D(:) + 1));         % inverse of the Euclidean distance
    DGS(i,j) = sum(sum(gaussmf(D,[0.5 0]))); % in the paper
    
    DAS(i,j) = sum(sum(abs(I - i) / M + abs(J - j) / N));                                             % Manhattan
    DMS(i,j) = sum(sum(max(abs(I - i) / M,abs(J - j) / N)));                                          % Chebychev
    DCS(i,j) = sum(sum((abs(I - i) / M) ./ (abs(I + i) / M) + (abs(J - j) / N) ./ (abs(J + j) / N))); % Camberra
    
    DTS(i,j) = sum(sum(gaussmf(D,[0.5 0]) ./ DGS(i,j)));
  end
end

%% visualize the different biases
subplot(2,4,1);
imshow(mat2gray(DS));
title(['Euclidean distance sum (' num2str(std(DS(:))) ')']);
subplot(2,4,2);
imshow(mat2gray(DIS));
title(['Inv. Euclidean distance sum (' num2str(std(DIS(:))) ')']);
subplot(2,4,3);
imshow(mat2gray(DGS));
title(['Gaussian distance sum (' num2str(std(DGS(:))) ')']);
subplot(2,4,4);
imshow(mat2gray(DAS));
title(['Absolute distance sum (' num2str(std(DAS(:))) ')']);
subplot(2,4,5);
imshow(mat2gray(DMS));
title(['Maximum distance sum (' num2str(std(DMS(:))) ')']);
subplot(2,4,6);
imshow(mat2gray(DCS));
title(['Camberra distance sum (' num2str(std(DCS(:))) ')']);
subplot(2,4,7);
imshow(mat2gray(DTS));
title(['Test distance sum (' num2str(std(DTS(:))) ')']);