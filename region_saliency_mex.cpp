#include "stdafx.h"
#include "region_saliency.h"
#include "ext/opencv_matlab.hpp"

#ifdef __MEX
#define __CONST__ const
#include "matrix.h"
#include "mex.h"
#endif

#ifdef __MEX
template <typename T>
void
_mexFunction(int nlhs, mxArray* plhs[],
             int nrhs, const mxArray* prhs[])
{
  // get input data pointers
  __CONST__ mxArray *mimage = prhs[0];
  __CONST__ T* image = (T*)mxGetData(mimage);
  const mwSize *image_dims = mxGetDimensions(mimage);
  
  if (mxIsComplex(mimage))
    mexErrMsgTxt("Only real image data allowed.");
  std::string method_s("FT");
  if (nrhs > 1)
    method_s = std::string(mxArrayToString(prhs[1]));

  // run OpenCV algorithm
  
  // 1. convert from Matlab to cv::Mat
  
  cv::Mat ocvimage(image_dims[0],image_dims[1],CV_32FC3); // @todo: THIS ONLY WORKS FOR FLOAT AS INPUT
  om::copyMatrixFromMatlab(image,ocvimage);
  // 2. calculate the saliency map
  cv::Mat ocvsaliency;
  if (method_s == std::string("FT")) // Achanta "Frequency Tuned"
    ocvsaliency = RegionSaliency::GetFT(ocvimage);
  else if (method_s == std::string("SR")) // Hou "Spectral Residual"
    ocvsaliency = RegionSaliency::GetSR(ocvimage);
  else if (method_s == std::string("RC")) // Region Contrast
  {
    if (nrhs > 2)
    {
      if (nrhs != 6)
        mexErrMsgTxt("Wrong number of input arguments. Input arguments: image 'RC' sigmaDist segK segMinSize segSigma");
      double sigmaDist             = (double)mxGetScalar(prhs[2]);
      double segK                  = (double)mxGetScalar(prhs[3]);
      int segMinSize               = (int)mxGetScalar(prhs[4]);
      double segSigma              = (double)mxGetScalar(prhs[5]);
      ocvsaliency = RegionSaliency::GetRC(ocvimage,sigmaDist,segK,segMinSize,segSigma);
    }
    else
      ocvsaliency = RegionSaliency::GetRC(ocvimage);
  }
  else if (method_s == std::string("RCNCC")) // Region Contrast - no color conversion (i.e., RC without the internal rgb2lab conversion)
  {
    if (nrhs > 2)
    {
      if (nrhs != 6)
        mexErrMsgTxt("Wrong number of input arguments. Input arguments: image 'RC' sigmaDist segK segMinSize segSigma");
      double sigmaDist             = (double)mxGetScalar(prhs[2]);
      double segK                  = (double)mxGetScalar(prhs[3]);
      int segMinSize               = (int)mxGetScalar(prhs[4]);
      double segSigma              = (double)mxGetScalar(prhs[5]);
      ocvsaliency = RegionSaliency::GetRCNoColorConversion(ocvimage,sigmaDist,segK,segMinSize,segSigma);
    }
    else
      ocvsaliency = RegionSaliency::GetRCNoColorConversion(ocvimage);
  }
  else if (method_s == std::string("DRC")) // Debiased Region Contrast
    ocvsaliency = RegionSaliency::GetRCDebiased(ocvimage);
  else if (method_s == std::string("HC")) // Histogram Contrast
    ocvsaliency = RegionSaliency::GetHC(ocvimage);
  else if (method_s == std::string("LC")) // Luminance Contrast
    ocvsaliency = RegionSaliency::GetLC(ocvimage);
  else if (method_s == std::string("RCCB")) // Region Contrast - Center-Biased
  {
    if (nrhs > 2)
    {
      if (nrhs != 10)
        mexErrMsgTxt("Wrong number of input arguments. Input arguments: image 'RCCB' sigmaDist segK segMinSize segSigma centerBiasWeight centerBiasHeightSigma centerBiasWidthSigma centerBiasCombinationTypeID");
      double sigmaDist             = (double)mxGetScalar(prhs[2]);
      double segK                  = (double)mxGetScalar(prhs[3]);
      int segMinSize               = (int)mxGetScalar(prhs[4]);
      double segSigma              = (double)mxGetScalar(prhs[5]);
      double centerBiasWeight      = (double)mxGetScalar(prhs[6]);
      double centerBiasHeightSigma = (double)mxGetScalar(prhs[7]);
      double centerBiasWidthSigma  = (double)mxGetScalar(prhs[8]);
      RegionSaliency::CenterBiasCombinationType_t cbct = (RegionSaliency::CenterBiasCombinationType_t)mxGetScalar(prhs[9]);
      ocvsaliency = RegionSaliency::GetRCCB(ocvimage,sigmaDist,segK,segMinSize,segSigma,centerBiasWeight,centerBiasHeightSigma,centerBiasWidthSigma,cbct);
    }
    else
      ocvsaliency = RegionSaliency::GetRCCB(ocvimage);
  }
  else if (method_s == std::string("DRCCB")) // Debiased Region Contrast - Center-Biased
  {
    if (nrhs > 2)
    {
      if (nrhs != 9)
        mexErrMsgTxt("Wrong number of input arguments. Input arguments: image 'DRCCB' sigmaDist segK segMinSize segSigma centerBiasWeight centerBiasHeightSigma centerBiasWidthSigma");
      double sigmaDist             = (double)mxGetScalar(prhs[2]);
      double segK                  = (double)mxGetScalar(prhs[3]);
      int segMinSize               = (int)mxGetScalar(prhs[4]);
      double segSigma              = (double)mxGetScalar(prhs[5]);
      double centerBiasWeight      = (double)mxGetScalar(prhs[6]);
      double centerBiasHeightSigma = (double)mxGetScalar(prhs[7]);
      double centerBiasWidthSigma  = (double)mxGetScalar(prhs[8]);
      ocvsaliency = RegionSaliency::GetRCCBDebiased(ocvimage,sigmaDist,segK,segMinSize,segSigma,centerBiasWeight,centerBiasHeightSigma,centerBiasWidthSigma);
    }
    else
      ocvsaliency = RegionSaliency::GetRCCBDebiased(ocvimage);
  }
  else if (method_s == std::string("LDRC")) // Locally Debiased Region Contrast
  {
    if (nrhs > 2)
    {
      if (nrhs != 6)
        mexErrMsgTxt("Wrong number of input arguments. Input arguments: image 'LDRC' sigmaDist segK segMinSize segSigma");
      double sigmaDist             = (double)mxGetScalar(prhs[2]);
      double segK                  = (double)mxGetScalar(prhs[3]);
      int segMinSize               = (int)mxGetScalar(prhs[4]);
      double segSigma              = (double)mxGetScalar(prhs[5]);
      ocvsaliency = RegionSaliency::GetRCLocallyDebiased(ocvimage,sigmaDist,segK,segMinSize,segSigma);
    }
    else
      ocvsaliency = RegionSaliency::GetRCLocallyDebiased(ocvimage);
  }
  else if (method_s == std::string("LDRCCB")) // Debiased Region Contrast - Center-Biased
  {
    if (nrhs > 2)
    {
      if (nrhs != 10)
        mexErrMsgTxt("Wrong number of input arguments. Input arguments: image 'DRCCB' sigmaDist segK segMinSize segSigma centerBiasWeight centerBiasHeightSigma centerBiasWidthSigma centerBiasCombinationTypeID");
      double sigmaDist             = (double)mxGetScalar(prhs[2]);
      double segK                  = (double)mxGetScalar(prhs[3]);
      int segMinSize               = (int)mxGetScalar(prhs[4]);
      double segSigma              = (double)mxGetScalar(prhs[5]);
      double centerBiasWeight      = (double)mxGetScalar(prhs[6]);
      double centerBiasHeightSigma = (double)mxGetScalar(prhs[7]);
      double centerBiasWidthSigma  = (double)mxGetScalar(prhs[8]);
      RegionSaliency::CenterBiasCombinationType_t cbct = (RegionSaliency::CenterBiasCombinationType_t)mxGetScalar(prhs[9]);
      ocvsaliency = RegionSaliency::GetRCCBLocallyDebiased(ocvimage,sigmaDist,segK,segMinSize,segSigma,centerBiasWeight,centerBiasHeightSigma,centerBiasWidthSigma,cbct);
    }
    else
      ocvsaliency = RegionSaliency::GetRCCBLocallyDebiased(ocvimage);
  }
  else
    mexErrMsgTxt("Unknown saliency method");
  // 3. create Matlab output data
  mwSize outdims[3];
  outdims[0] = image_dims[0];
  outdims[1] = image_dims[1];
  outdims[2] = 1;
  mxArray *moutdata = mxCreateNumericArray(3,
                                           outdims,
                                           mxSINGLE_CLASS,
                                           mxREAL);
  plhs[0] = moutdata; 
  float* outdata = (float*)mxGetData(moutdata);
  // 4. convert from cv::Mat to Matlab
  om::copyMatrixFromOpencv(ocvsaliency,outdata);
}

void
mexFunction(int nlhs, mxArray* plhs[],
            int nrhs, const mxArray* prhs[])
{
  // check number of input parameters
  //if (nrhs < 1 || nrhs > 2)
  if (nrhs < 1)
    mexErrMsgTxt("Wrong number of input arguments. Input arguments: image [method] [... method parameters ...]");

  // Check number of output parameters
  if (nlhs > 1) 
    mexErrMsgTxt("Wrong number of output arguments. Output argument: saliency-map");

  const mwSize* indims=mxGetDimensions(prhs[0]);
  if (mxGetNumberOfDimensions(prhs[0]) < 3 || indims[2] > 3)
    mexErrMsgTxt("The image has to be a MxNx3 matrix.");

  // only float and double are currently supported
  if (!mxIsDouble(prhs[0]) && !mxIsSingle(prhs[0])) 
  	mexErrMsgTxt("Only float and double input arguments are supported.");
  
  switch (mxGetClassID(prhs[0]))
  {
//     case mxDOUBLE_CLASS:
//       _mexFunction<double>(nlhs,plhs,nrhs,prhs);
//       break;
    case mxSINGLE_CLASS:
      _mexFunction<float>(nlhs,plhs,nrhs,prhs);
      break;
    default:
      // this should never happen
      mexErrMsgTxt("Unsupoorted image class.");
      break;
  }
}
#endif