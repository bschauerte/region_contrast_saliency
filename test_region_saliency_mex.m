% Visualize the different region contrast variants, i.e. with and without
% center bias.
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

%% Allow for different center bias combinations
%    CB_LINEAR  = 0,
%    CB_PRODUCT = 1
%    CB_MAX     = 2
%    CB_MIN     = 3
cbctids = {'CB_LINEAR','CB_PRODUCT','CB_MAX','CB_MIN'};                                              % the types
f_cbctids_idx = @(s) (sum(strcmp(s,{'CB_LINEAR','CB_PRODUCT','CB_MAX','CB_MIN'}) .* [1 2 3 4]) - 1); % get the index of the string

%% define the methods that you want to compare here
smethods = { ...
    {'RC'} ...   % Region Contrast
    {'LDRC'} ... % Locally Debiased Region Contrast
    ... % in the following, LCRF with an added/multiplied/min'ed/max'ed center bias 
    {'LDRCCB',0.5,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_PRODUCT')} ...
    {'LDRCCB',0.5,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_LINEAR')} ...
    {'LDRCCB',0.5,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_MIN')} ...
    {'LDRCCB',0.5,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_MAX')} ...
  };
nmethods = numel(smethods);

%% calculate the saliency maps for all methods
img_dir = 'examples';
files   = dir(fullfile(img_dir,'*.jpg'));
diffs   = zeros(nmethods,nmethods,length(files));
figure('name','image');
for i=1:length(files)
  I_path=fullfile(img_dir,files(i).name);
  
  I=imread(I_path);
  I_orig=I;
  I=imresize(I,[400 NaN]);
  IS=im2single(I);

  smaps=cell(1,numel(smethods));
  
  subplot(3,ceil((nmethods+1)/3),1); imshow(I)
  for m=1:nmethods
    tic;
    S=region_saliency_mex(IS,smethods{m}{:});
    t=toc;
    subplot(3,ceil((nmethods+1)/3),1+m); imshow(mat2gray(S)); colormap hot; title([smethods{m}{1} ' (' num2str(t) ')']);
    smaps{m}=S;
  end
  drawnow;
end
