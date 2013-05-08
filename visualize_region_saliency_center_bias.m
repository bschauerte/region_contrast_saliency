% visualize the implicit center bias of the model that is introduced
% by using a Gaussian to weight the distances
%
% To this end, on a regular grid: calculate the sum of distances of each
% point to all other points on the grid.

M=25;
N=25;
DS=zeros(M,N);
DIS=zeros(M,N);
DGS=zeros(M,N);
DAS=zeros(M,N);
DMS=zeros(M,N);
DCS=zeros(M,N);
DTS=zeros(M,N);

[I,J]=meshgrid(1:M,1:N);
for i=1:M
  for j=1:N
    D = sqrt(((I - i) / M).^2 + ((J - j) / N).^2); % euclidean distance to all points
    
    DS(i,j)  = sum(D(:));                    % Euclidean distance
    DIS(i,j) = sum(1 ./ (D(:) + 1));         % inverse of the Euclidean distance
    DGS(i,j) = sum(sum(gaussmf(D,[0.5 0]))); % in the paper
    
    DAS(i,j) = sum(sum(abs(I - i) / M + abs(J - j) / N));                                             % Manhattan
    DMS(i,j) = sum(sum(max(abs(I - i) / M,abs(J - j) / N)));                                          % Chebychev
    DCS(i,j) = sum(sum((abs(I - i) / M) ./ (abs(I + i) / M) + (abs(J - j) / N) ./ (abs(J + j) / N))); % Camberra
    
    %DTS(i,j) = DGS(i,j) ./ DGS(i,j);
    DTS(i,j) = sum(sum(gaussmf(D,[0.5 0]) ./ DGS(i,j)));
  end
end

figure('name',['Euclidean distance sum (' num2str(std(DS(:))) ')']);
imshow(mat2gray(DS));
figure('name',['Inv. Euclidean distance sum (' num2str(std(DIS(:))) ')']);
imshow(mat2gray(DIS));
figure('name',['Gaussian distance sum (' num2str(std(DGS(:))) ')']);
imshow(mat2gray(DGS));
figure('name',['Absolute distance sum (' num2str(std(DAS(:))) ')']);
imshow(mat2gray(DAS));
figure('name',['Maximum distance sum (' num2str(std(DMS(:))) ')']);
imshow(mat2gray(DMS));
figure('name',['Camberra distance sum (' num2str(std(DCS(:))) ')']);
imshow(mat2gray(DCS));
figure('name',['Test distance sum (' num2str(std(DTS(:))) ')']);
imshow(mat2gray(DTS));

save_figures=true;
figures_path='/home/bschauer/papers/ECCV.SalientObject-2012.5/figs/implicit_center_bias';
if save_figures
  image_path=fullfile(figures_path,'gaussian_distance_sum.png');
  imwrite(imresize(mat2gray(DGS),[200 200]),image_path);
end