% Demonstrate the distance bias present in the original region contrast
% algorithm.
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

if ~exist('segment_weight_sums.txt')
  fprintf('Downloading data. Please wait ... ');
  %urlwrite('https://cvhci.anthropomatik.kit.edu/~bschauer/datasets/segment_weight_sums.txt')
  unzip('https://cvhci.anthropomatik.kit.edu/~bschauer/datasets/segment_weight_sums.txt.zip')
  fprintf('done\n');
end

a=load('segment_weight_sums.txt');
  % Segment_weight_sums.txt contains the summed region weights, i.e.
  %  \forall r_k : \sum_{r_i}\hat{D}_s(r_k;r_i)
  % The first and second column contain the x- and y-coordinate, 
  % respectively, of the centroid of the region. The third column contains
  % the weight sum.
  % The data was collected using Felzenszwalb's segmentation algorithm for
  % all images in the Achanta subset of Liu's image data.

%% Visualize the weight bias in the data set
grid_size=50;
weight_grid=zeros(grid_size,grid_size);
num_grid=zeros(grid_size,grid_size);

locs=ceil(grid_size*(a(:,1:2)+eps)); % locations in the grid

for i=1:size(locs,1)
  num_grid(locs(i,1),locs(i,2)) = num_grid(locs(i,1),locs(i,2)) + 1;
  weight_grid(locs(i,1),locs(i,2)) = weight_grid(locs(i,1),locs(i,2)) + a(i,3);
end

norm_weight_grid = weight_grid ./ (num_grid + 1);
subplot(1,2,1); imshow(mat2gray(norm_weight_grid)); title('Mean Weight Sums');

%% Visualize the expected, theoretical weight bias
M=grid_size;
N=grid_size;

DGS=zeros(M,N);

[I,J]=meshgrid(1:M,1:N);
for i=1:M
  for j=1:N
    D = sqrt(((I - i) / M).^2 + ((J - j) / N).^2); % euclidean distance to all points
    
    DGS(i,j) = sum(sum(gaussmf(D,[0.5 0]))); % as in the paper by Cheng et al.
  end
end

subplot(1,2,2); imshow(mat2gray(DGS)); title('Regular Grid Weight Sum');