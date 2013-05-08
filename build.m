include_pathes = {'/usr/local/include/opencv/','/usr/local/include/'};
lib_pathes     = {'/usr/local/lib/'};
lib_names      = {'opencv_core','opencv_imgproc'};
cpp_pathes     = {'region_saliency_mex.cpp','region_saliency.cpp','segment-image.cpp'};
ext_defines    = {'__MEX'};
other_options  = {'-O'};

% further settings
arch=computer('arch'); % target architecture
outdir='';
outname='';

%%%
% create an options cell array as input for mex
%%%
mex_options=cell(1,numel(include_pathes)+numel(lib_pathes)+numel(lib_names)+numel(cpp_pathes)+numel(ext_defines));
c=1;

% external defines
for i=1:numel(ext_defines)
  mex_options{c} = sprintf('-D%s',ext_defines{i});
  c = c+1;
end

% .c/.cpp files
for i=1:numel(cpp_pathes)
  mex_options{c} = sprintf('%s',cpp_pathes{i});
  c = c+1;
end

% -I
for i=1:numel(include_pathes)
  mex_options{c} = sprintf('-I%s',include_pathes{i});
  c = c+1;
end

% -L
for i=1:numel(lib_pathes)
  mex_options{c} = sprintf('-L%s',lib_pathes{i});
  c = c+1;
end

% -l
for i=1:numel(lib_names)
  mex_options{c} = sprintf('-l%s',lib_names{i});
  c = c+1;
end

% some further settings
further_options={};
if ~isempty(arch)
  further_options = [further_options {sprintf('-%s',arch)}];
end
if ~isempty(outdir)
  further_options = [further_options {'-outdir'} {sprintf('%s',outdir)}];
end
if ~isempty(outname)
  further_options = [further_options {'-output'} {sprintf('%s',outname)}];
end

mex_options = [other_options further_options mex_options];

mex(mex_options{:});