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

%% Options
do_visualize_differences    = true; % do you want to visualize the differences between the models?
colormap_arguments          = {'hot'};%{'default'};% % pick your favorite color map
do_save_saliency_maps       = false; % do you want to save the saliency maps?
save_saliency_maps_folder   = 'smaps';
do_save_difference          = false; % do you want to save the difference maps?
save_difference_maps_folder = 'dmaps';

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
    ... % in the following, LDRC with an added/multiplied/min'ed/max'ed center bias
    {'LDRCCB',0.5,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_PRODUCT')} ...
    {'LDRCCB',0.5,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_LINEAR')} ...
    {'LDRCCB',0.5,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_MIN')} ...
    {'LDRCCB',0.5,50,50,0.5,0.5,0.5,0.5,f_cbctids_idx('CB_MAX')} ...
    };
nmethods = numel(smethods);

%% Calculate the saliency maps for all methods
img_dir = 'examples';
files   = dir(fullfile(img_dir,'*.jpg'));
diffs   = zeros(nmethods,nmethods,length(files));
figure('name','image');
for i = 1:length(files)
    I_path = fullfile(img_dir,files(i).name);
    
    I      = imread(I_path);
    I_orig = I;
    I      = imresize(I,[400 NaN]);
    IS     = im2single(I);
    
    smaps  = cell(1,numel(smethods));
    
    subplot(3,ceil((nmethods+1)/3),1); imshow(I)
    for m=1:nmethods
        tic;
        S=region_saliency_mex(IS,smethods{m}{:});
        t=toc;
        subplot(3,ceil((nmethods+1)/3),1+m); imshow(mat2gray(S)); colormap(colormap_arguments{:}); title([smethods{m}{1} ' (' num2str(t) ')']);
        smaps{m}=S;
    end
    drawnow;
end

%% Visualize differences
if do_visualize_differences
    figure('name','saliency map differences');
    for i = 1 %:length(files) % @note: change this if you want to visualize different/more images
        I_path   = fullfile(img_dir,files(i).name);
        I        = imread(I_path);
        I_orig   = I;
        I        = imresize(I,[400 NaN]);
        IS       = im2single(I);
        smaps    = cell(1,numel(smethods));
        for m = 1:nmethods
            smaps{m} = region_saliency_mex(IS,smethods{m}{:});
        end
        
        for m = 1:nmethods
            subplot(nmethods+1,nmethods+1,m+1); imshow(mat2gray(smaps{m})); colormap(colormap_arguments{:}); title([smethods{m}{1} ' (' num2str(t) ')']);
            subplot(nmethods+1,nmethods+1,(m+1-1)*(nmethods+1) + 1); imshow(mat2gray(smaps{m})); colormap(colormap_arguments{:}); title(smethods{m}{1});
        end
        for m = 1:nmethods
            for n = 1:nmethods
                    subplot(nmethods+1,nmethods+1,(m+1-1)*(nmethods+1) + (n+1));
                    if m == n
                        imshow(I);
                    else
                        imshow(mat2gray(smaps{m} - smaps{n})); colormap(colormap_arguments{:}); title([smethods{m}{1} ' vs '  smethods{n}{1}]);
                    end
            end
        end
    end
end

%% Save the saliency maps
if do_save_saliency_maps
    h = figure;
    if ~exist(save_saliency_maps_folder,'dir'), mkdir(save_saliency_maps_folder); end
    for i = 1:length(files)
        I_path   = fullfile(img_dir,files(i).name);
        I        = imread(I_path);
        I_orig   = I;
        I        = imresize(I,[400 NaN]);
        IS       = im2single(I);
        smaps    = cell(1,numel(smethods));
        for m = 1:nmethods
            smaps{m} = region_saliency_mex(IS,smethods{m}{:});
        end
        
        for m = 1:nmethods
            [fp,fn,fe] = fileparts(files(i).name);
            imshow(dmap); drawnow;
            %imwrite(mat2gray(smaps{m}),fullfile(save_saliency_maps_folder,sprintf('%s_%s.png',fn,smethods{m}{1})));
            imwrite(im2uint8(mat2gray(smaps{m})),colormap(colormap_arguments{:}),fullfile(save_saliency_maps_folder,sprintf('%s_%s_%d.png',fn,smethods{m}{1},m)));
        end
    end
    close(h);
end

%% Save the difference maps
if do_save_difference
    h = figure;
    if ~exist(save_difference_maps_folder,'dir'), mkdir(save_difference_maps_folder); end
    for i = 1:length(files)
        I_path   = fullfile(img_dir,files(i).name);
        I        = imread(I_path);
        I_orig   = I;
        I        = imresize(I,[400 NaN]);
        IS       = im2single(I);
        smaps    = cell(1,numel(smethods));
        for m = 1:nmethods
            smaps{m} = region_saliency_mex(IS,smethods{m}{:});
        end
        
        for m = 1:nmethods
            for n = 1:nmethods
                if m == n
                    continue;
                end
                [fp,fn,fe] = fileparts(files(i).name);
                %dmap = im2uint8(mat2gray(mat2gray(smaps{m}) - mat2gray(smaps{n})));
                dmap = im2uint8(mat2gray(smaps{m} - smaps{n}));
                imshow(dmap); drawnow;
                imwrite(dmap,colormap(colormap_arguments{:}),fullfile(save_difference_maps_folder,sprintf('%s_%s_vs_%s.png',fn,smethods{m}{1},smethods{n}{1})));
            end
        end
    end
    close(h);
end