a=load('segment_weight_sums.txt');
  % Segment_weight_sums.txt contains the summed region weights, i.e.
  %  \forall r_k : \sum_{r_i}\hat{D}_s(r_k;r_i)
  % The first and second column contain the x- and y-coordinate, 
  % respectively, of the centroid of the region. The third column contains
  % the weight sum.
  % The data was collected using Felzenszwalb's segmentation algorithm for
  % all images in the Achanta subset of Liu's image data.

%%
% Visualize the weight bias in the data set
  
%grid_size=100;
grid_size=50;
weight_grid=zeros(grid_size,grid_size);
num_grid=zeros(grid_size,grid_size);

locs=ceil(grid_size*(a(:,1:2)+eps)); % locations in the grid

for i=1:size(locs,1)
  num_grid(locs(i,1),locs(i,2)) = num_grid(locs(i,1),locs(i,2)) + 1;
  weight_grid(locs(i,1),locs(i,2)) = weight_grid(locs(i,1),locs(i,2)) + a(i,3);
end

norm_weight_grid = weight_grid ./ (num_grid + 1);
figure('name','Mean Weight Sums'), imshow(mat2gray(norm_weight_grid));

%%
% Visualize the expected, theoretical weight bias
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

figure('name','Regular Grid Weight Sum'), imshow(mat2gray(DGS));

%%
% Save the figures
save_figures=true;
figures_path='/home/bschauer/papers/ECCV.SalientObject-2012.5/figs/implicit_center_bias';
if save_figures
  image_path=fullfile(figures_path,'gaussian_distance_sum.png');
  imwrite(imresize(mat2gray(DGS),[200 200]),image_path);
  
  image_path=fullfile(figures_path,'mean_distance_sum.png');
  imwrite(imresize(mat2gray(norm_weight_grid),[200 200]),image_path);
end