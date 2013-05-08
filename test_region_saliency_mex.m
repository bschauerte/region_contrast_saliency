% Center-Bias Combination Type IDs
%  CB_LINEAR  = 0,
%  CB_PRODUCT = 1
%  CB_MAX     = 2
%  CB_MIN     = 3
cbctids={'CB_LINEAR','CB_PRODUCT','CB_MAX','CB_MIN'};                                        % the types
f_cbctids_idx=@(s) sum(strcmp(s,{'CB_LINEAR','CB_PRODUCT','CB_MAX','CB_MIN'}) .* [1 2 3 4]); % get the index of the string

%smethods={'FT','SR','RC','HC','LC'};
% smethods={ ...
%   {'DRCCB',0.5,50,50,0.5,0.5,0.5,0.5}, ...
%   {'RCCB',0.5,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_PRODUCT')-1}, ...
%   {'RCCB',0.5,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_LINEAR')-1}, ...
%   {'RCCB',0.5,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_MIN')-1}, ...
%   {'RCCB',0.5,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_MAX')-1}, ...
%   {'LDRC'}, ... % Locally Debiased Region Contrast
%   {'DRC'}, ...  % Debiased Region Contrast
%   {'RC'}, ...   % Region Contrast (original)
%   {'HC'}, ...   % Histogram Contrast (original)
%   {'LC'} ...    % Luminance Contrast (original)
%   {'LDRCCB',0.4,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_PRODUCT')-1}, ... % Locally Debiased Region Contrast - Center Biased
%   {'LDRCCB',0.4,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_LINEAR')-1}, ... % Locally Debiased Region Contrast - Center Biased
%   {'LDRCCB',0.4,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_MAX')-1}, ... % Locally Debiased Region Contrast - Center Biased
%   {'LDRCCB',0.4,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_MIN')-1}, ... % Locally Debiased Region Contrast - Center Biased
% %  ... % {'DRCCB',0.5,50,60,0.5,0.7,0.25,0.25}, ... % double sigmaDist = 0.4, double segK = 50, int segMinSize = 50, double segSigma = 0.5, double centerBiasWeight = 0.5, double centerBiasHeightSigma = 0.5, double centerBiasWidthSigma = 0.5
%   };
% smethods={ ...
%   {'RCCB',0.4,50,50,0.5,0.4,0.5,0.5,f_cbctids_idx('CB_LINEAR')-1}, ...
%   {'LDRC'}, ... % Locally Debiased Region Contrast
%   {'RC'}, ...   % Region Contrast (original)
%   {'LDRCCB',0.9,50,50,0.5,0.55,0.5,0.5,f_cbctids_idx('CB_LINEAR')-1}, ... % Locally Debiased Region Contrast - Center Biased
%   };
smethods={ ...
  {'LDRC'}, ... % Locally Debiased Region Contrast
  {'RC'}, ...   % Region Contrast (original)
  {'RCNCC'}, ...   % Region Contrast (original)
  };
nmethods=numel(smethods);

% use this to generate images
img_dir='/home/bschauer/data/salient-objects/liu/images/A/0/';
files=dir('/home/bschauer/data/salient-objects/liu/images/A/0/*.jpg');
files=files(1:200); % only take a subset
%files(1).name='0_10_10536.jpg';
%files(2).name='0_15_15264.jpg';
diffs=zeros(nmethods,nmethods,length(files));
figure('name','image');
for i=1:length(files)
  I_path=fullfile(img_dir,files(i).name);
  
  %I=imread('/home/bschauer/private/wallpaper-hot/1307787974833.jpg');
  %I=imread('/home/bschauer/private/wallpaper-super-hot/1323631757135.jpg');
  %I_path='/home/bschauer/data/salient-objects/liu/images/A/0/0_3_3327.jpg';
  I=imread(I_path);
  I_orig=I;
  %I=imread('/home/bschauer/private/wallpaper-super-hot/1324005305613.jpg');
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
  
  % calculate the pairwise difference between the maps
  for m=1:nmethods
    for n=1:nmethods
      diffs(m,n,i)=mean(abs(reshape(smaps{m},1,[]) - reshape(smaps{n},1,[])));
    end
  end
  
  save_figures=false;
  %figures_path='/home/bschauer/papers/ECCV.SalientObject-2012.5/figs/examples';
  figures_path='/home/bschauer/papers/CVPR.SalientObject-2013.2/figs/examples';
  if save_figures
    [fp,fn,fe]=fileparts(I_path);
    
    image_path=fullfile(figures_path,sprintf('%s.png',fn));
    imwrite(I_orig,image_path);
    for m=1:nmethods
      smap_path=fullfile(figures_path,sprintf('%s_smap_%s.png',fn, smethods{i}{1}));
      S=region_saliency_mex(IS,smethods{m}{:});
      imwrite(mat2gray(S),smap_path);
    end
  end
end

%%
% Show only the images with the biggest saliency difference
method_1_idx=2; % compare this method's map ...
method_2_idx=3; % ... vs this method's map
[d,idcs]=sort(diffs(method_1_idx,method_2_idx,:),'descend');
sorted_files=files(idcs);
figure;
nexamples=5;
X=1:5;
while max(X) <= length(files)
  c=1;
  for i=X
    I_path=fullfile(img_dir,sorted_files(i).name);
    I=imread(I_path);
    I_orig=I;
    I=imresize(I,[400 NaN]);
    IS=im2single(I);
    
    method_1_S=region_saliency_mex(IS,smethods{method_1_idx}{:});
    method_2_S=region_saliency_mex(IS,smethods{method_2_idx}{:});
    
    subplot(nexamples,3,c); imshow(I); c=c+1; title(sprintf('%f (%d)',num2str(d(i),i)));
    subplot(nexamples,3,c); imshow(mat2gray(method_1_S)); colormap hot; title(smethods{method_1_idx}{1}); c=c+1;
    subplot(nexamples,3,c); imshow(mat2gray(method_2_S)); colormap hot; title(smethods{method_2_idx}{1}); c=c+1;
  end
  waitforbuttonpress
  X=X+nexamples;
end