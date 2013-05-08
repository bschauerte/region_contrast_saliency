/**
 * Copyright 2012 B. Schauerte. All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions are 
 * met:
 * 
 *    1. Redistributions of source code must retain the above copyright 
 *       notice, this list of conditions and the following disclaimer.
 * 
 *    2. Redistributions in binary form must reproduce the above copyright 
 *       notice, this list of conditions and the following disclaimer in 
 *       the documentation and/or other materials provided with the 
 *       distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY B. SCHAUERTE ''AS IS'' AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL B. SCHAUERTE OR CONTRIBUTORS BE LIABLE 
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR 
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *  
 * The views and conclusions contained in the software and documentation
 * are those of the authors and should not be interpreted as representing 
 * official policies, either expressed or implied, of B. Schauerte.
 */

/**
 * If you use any of this work in scientific research or as part of a larger
 * software system, you are kindly requested to cite the use in any related 
 * publications or technical documentation. The work is based upon:
 *
 * [1] B. Schauerte, R. Stiefelhagen, "How the Distribution of Salient 
 *     Objects in Images Influences Salient Object Detection". In 
 *     Proceedings of the 20th International Conference on Image Processing
 *     (ICIP), 2013.
 */

/**
 * Original code base and approach by:
 *   Ming-Ming Cheng, Guo-Xin Zhang, Niloy J. Mitra, Xiaolei Huang, Shi-Min
 *   Hu, "Global Contrast based Salient Region Detection," IEEE CVPR, 2011.
 */

#pragma once

#include <vector>
#include <string>

typedef std::vector<std::string> vecS;
typedef std::vector<int> vecI;
typedef std::vector<float> vecF;
typedef std::vector<double> vecD;
typedef std::pair<double, int> CostIdx;
typedef std::pair<float, int> CostfIdx;

struct RegionSaliency
{
  typedef enum {
    CB_LINEAR = 0,
    CB_PRODUCT,
    CB_MAX,
    CB_MIN,
    CB_NUM
  } CenterBiasCombinationType_t;

	// Region Contrast 
	static cv::Mat GetRC(const cv::Mat &img3f);
	static cv::Mat GetRC(const cv::Mat &img3f, double sigmaDist, double segK, int segMinSize, double segSigma);
  static cv::Mat GetRCLocallyDebiased(const cv::Mat &img3f, double sigmaDist = 0.4, double segK = 50, int segMinSize = 50, double segSigma = 0.5);

  // Region Contrast but without intrinsic rgb2lab conversion
  static cv::Mat GetRCNoColorConversion(const cv::Mat &img3f);
  static cv::Mat GetRCNoColorConversion(const cv::Mat &img3f, double sigmaDist, double segK, int segMinSize, double segSigma);
  
	// Region Contrast - Center-Biased
	static cv::Mat GetRCCB(const cv::Mat &img3f, 
          double sigmaDist = 0.4, 
          double segK = 50, 
          int segMinSize = 50, 
          double segSigma = 0.5, 
          double centerBiasWeight = 0.5, 
          double centerBiasHeightSigma = 0.5, 
          double centerBiasWidthSigma = 0.5,
          const CenterBiasCombinationType_t cbct = CB_LINEAR);

  // Region Contrast (locally debiased) - Center-Biased
	static cv::Mat GetRCCBLocallyDebiased(const cv::Mat &img3f, 
          double sigmaDist = 0.4, 
          double segK = 50, 
          int segMinSize = 50, 
          double segSigma = 0.5, 
          double centerBiasWeight = 0.5, 
          double centerBiasHeightSigma = 0.5, 
          double centerBiasWidthSigma = 0.5,
          const CenterBiasCombinationType_t cbct = CB_LINEAR);

	// Color quantization
	static int Quantize(const cv::Mat& img3f, cv::Mat &idx1i, cv::Mat &_color3f, cv::Mat &_colorNum, double ratio = 0.95);

private:
	static const int SAL_TYPE_NUM = 5;
	typedef cv::Mat (*GET_SAL_FUNC)(const cv::Mat &);
	static const char* SAL_TYPE_DES[SAL_TYPE_NUM];
	static const GET_SAL_FUNC gFuns[SAL_TYPE_NUM];

	static void SmoothSaliency(const cv::Mat &binColor3f, cv::Mat &sal1d, float delta, const std::vector<std::vector<CostfIdx> > &similar);

	struct Region{
		Region() { pixNum = 0;}
		int pixNum;                    // Number of pixels
		std::vector<CostfIdx> freIdx;  // Frequency of each color and its index
		Point2d centroid;
	};
	static void BuildRegions(const cv::Mat& regIdx1i, std::vector<Region> &regs, const cv::Mat &colorIdx1i, int colorNum);
	static void RegionContrast(const std::vector<Region> &regs, const cv::Mat &color3fv, cv::Mat& regSal1d, double sigmaDist);
  static void RegionContrastDebiased(const std::vector<Region> &regs, const cv::Mat &color3fv, cv::Mat& regSal1d, double sigmaDist);
  static void RegionContrastLocallyDebiased(const vector<Region> &regs, const Mat &color3fv, Mat& regSal1d, double sigmaDist);
};