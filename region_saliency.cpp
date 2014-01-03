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

#include "stdafx.h"
#include "region_saliency.h"

#include <stdio.h>

#ifdef __MEX
#define __CONST__ const
#include "matrix.h"
#include "mex.h"
#endif

#ifndef SQR
#define SQR(x) ((x)*(x))
#endif

typedef unsigned char byte_t;

Mat RegionSaliency::GetRCCB(const Mat &img3f, double sigmaDist, double segK, int segMinSize, double segSigma, 
        double centerBiasWeight, double centerBiasHeightSigma, double centerBiasWidthSigma, const CenterBiasCombinationType_t cbct)
{
	Mat regIdx1i, colorIdx1i, regSal1v, tmp, _img3f, color3fv;
	if (Quantize(img3f, colorIdx1i, color3fv, tmp) <= 2) // Color quantization
		return Mat::zeros(img3f.size(), CV_32F);
	cvtColor(img3f, _img3f, CV_BGR2Lab);
	cvtColor(color3fv, color3fv, CV_BGR2Lab);
	int regNum = SegmentImage(_img3f, regIdx1i, segSigma, segK, segMinSize);	
	vector<Region> regs(regNum);
	BuildRegions(regIdx1i, regs, colorIdx1i, color3fv.cols);
	RegionContrast(regs, color3fv, regSal1v, sigmaDist);
  
  float* regsCenterBias = new float[regNum]; // the center-bias for each region
  float w0 = (float)centerBiasWidthSigma;    // std. dev. of the Gaussian (width)
  float h0 = (float)centerBiasHeightSigma;   // std. dev. of the Gaussian (height)
  for (int i = 0; i < regNum; i++)
  {
    const float x0 = 0.5;
    const float y0 = 0.5;

    regsCenterBias[i] = ( exp((-SQR(regs[i].centroid.x-x0))/SQR(w0)) * exp((-SQR(regs[i].centroid.y-y0))/SQR(h0)) );
  }

	Mat sal1f = Mat::zeros(img3f.size(), CV_32F);
	cv::normalize(regSal1v, regSal1v, 0, 1, NORM_MINMAX, CV_32F);
	float* regSal = (float*)regSal1v.data;
	for (int r = 0; r < img3f.rows; r++)
  {
		const int* regIdx = regIdx1i.ptr<int>(r);
		float* sal = sal1f.ptr<float>(r);
		for (int c = 0; c < img3f.cols; c++)
    {
      switch (cbct)
      {
        case CB_LINEAR:
          sal[c] = (1-centerBiasWeight)*regSal[regIdx[c]] + centerBiasWeight*regsCenterBias[regIdx[c]];
          break;
        case CB_PRODUCT:
          sal[c] = regSal[regIdx[c]] * regsCenterBias[regIdx[c]]; // weighting in this case would have no influence
          break;
        case CB_MAX:
          sal[c] = std::max((1-centerBiasWeight)*regSal[regIdx[c]], centerBiasWeight*regsCenterBias[regIdx[c]]);
          break;
        case CB_MIN:
          sal[c] = std::min((1-centerBiasWeight)*regSal[regIdx[c]], centerBiasWeight*regsCenterBias[regIdx[c]]);
          break;
        default:
          assert(false);
          exit(-1);
      }
    }
	}
	GaussianBlur(sal1f, sal1f, Size(3, 3), 0);
  
  delete [] regsCenterBias;
  
	return sal1f;
}

Mat RegionSaliency::GetRCCBLocallyDebiased(const Mat &img3f, double sigmaDist, double segK, int segMinSize, double segSigma, 
                                    double centerBiasWeight, double centerBiasHeightSigma, double centerBiasWidthSigma, const CenterBiasCombinationType_t cbct)
{
	Mat regIdx1i, colorIdx1i, regSal1v, tmp, _img3f, color3fv;
	if (Quantize(img3f, colorIdx1i, color3fv, tmp) <= 2) // Color quantization
		return Mat::zeros(img3f.size(), CV_32F);
	cvtColor(img3f, _img3f, CV_BGR2Lab);
	cvtColor(color3fv, color3fv, CV_BGR2Lab);
	int regNum = SegmentImage(_img3f, regIdx1i, segSigma, segK, segMinSize);	
	vector<Region> regs(regNum);
	BuildRegions(regIdx1i, regs, colorIdx1i, color3fv.cols);
	RegionContrastLocallyDebiased(regs, color3fv, regSal1v, sigmaDist); // use debiased metric here; everything else should be exactly the same as RegionSaliency::GetRCCB
  
  float* regsCenterBias = new float[regNum]; // the center-bias for each region
  float w0 = (float)centerBiasWidthSigma;    // std. dev. of the Gaussian (width)
  float h0 = (float)centerBiasHeightSigma;   // std. dev. of the Gaussian (height)
  for (int i = 0; i < regNum; i++)
  {
    const float x0 = 0.5;
    const float y0 = 0.5;

    regsCenterBias[i] = ( exp((-SQR(regs[i].centroid.x-x0))/SQR(w0)) * exp((-SQR(regs[i].centroid.y-y0))/SQR(h0)) );
  }

	Mat sal1f = Mat::zeros(img3f.size(), CV_32F);
	cv::normalize(regSal1v, regSal1v, 0, 1, NORM_MINMAX, CV_32F);
	float* regSal = (float*)regSal1v.data;
	for (int r = 0; r < img3f.rows; r++)
  {
		const int* regIdx = regIdx1i.ptr<int>(r);
		float* sal = sal1f.ptr<float>(r);
		for (int c = 0; c < img3f.cols; c++)
    {
      switch (cbct)
      {
        case CB_LINEAR:
          sal[c] = (1-centerBiasWeight)*regSal[regIdx[c]] + centerBiasWeight*regsCenterBias[regIdx[c]];
          break;
        case CB_PRODUCT:
          sal[c] = regSal[regIdx[c]] * regsCenterBias[regIdx[c]]; // weighting in this case would have no influence
          break;
        case CB_MAX:
          sal[c] = std::max((1-centerBiasWeight)*regSal[regIdx[c]], centerBiasWeight*regsCenterBias[regIdx[c]]);
          break;
        case CB_MIN:
          sal[c] = std::min((1-centerBiasWeight)*regSal[regIdx[c]], centerBiasWeight*regsCenterBias[regIdx[c]]);
          break;
        default:
          assert(false);
          exit(-1);
      }
    }
	}
	GaussianBlur(sal1f, sal1f, Size(3, 3), 0);
  
  delete [] regsCenterBias;
  
	return sal1f;
}

Mat RegionSaliency::GetRCLocallyDebiased(const Mat &img3f, double sigmaDist, double segK, int segMinSize, double segSigma)
{
	Mat regIdx1i, colorIdx1i, regSal1v, tmp, _img3f, color3fv;
	if (Quantize(img3f, colorIdx1i, color3fv, tmp) <= 2) // Color quantization
		return Mat::zeros(img3f.size(), CV_32F);
	cvtColor(img3f, _img3f, CV_BGR2Lab);
	cvtColor(color3fv, color3fv, CV_BGR2Lab);
	int regNum = SegmentImage(_img3f, regIdx1i, segSigma, segK, segMinSize);	
	vector<Region> regs(regNum);
	BuildRegions(regIdx1i, regs, colorIdx1i, color3fv.cols);
	RegionContrastLocallyDebiased(regs, color3fv, regSal1v, sigmaDist);

	Mat sal1f = Mat::zeros(img3f.size(), CV_32F);
	cv::normalize(regSal1v, regSal1v, 0, 1, NORM_MINMAX, CV_32F);
	float* regSal = (float*)regSal1v.data;
	for (int r = 0; r < img3f.rows; r++){
		const int* regIdx = regIdx1i.ptr<int>(r);
		float* sal = sal1f.ptr<float>(r);
		for (int c = 0; c < img3f.cols; c++)
			sal[c] = regSal[regIdx[c]];
	}
	GaussianBlur(sal1f, sal1f, Size(3, 3), 0);
	return sal1f;
}

// the weight sum is 1 for all regions
void RegionSaliency::RegionContrastLocallyDebiased(const vector<Region> &regs, const Mat &color3fv, Mat& regSal1d, double sigmaDist)
{	
  // color distance cache
	Mat_<float> cDistCache1f = Mat::zeros(color3fv.cols, color3fv.cols, CV_32F);
  {
		Vec3f* pColor = (Vec3f*)color3fv.data;
		for(int i = 0; i < cDistCache1f.rows; i++)
			for(int j= i+1; j < cDistCache1f.cols; j++)
				cDistCache1f[i][j] = cDistCache1f[j][i] = vecDist3(pColor[i], pColor[j]);
	}
  
  // spatial distance cache
  int regNum = (int)regs.size();
  Mat_<double> sDistCache1d = Mat::zeros(regNum, regNum, CV_64F);
  Mat_<double> sDistCache1dSum = Mat::zeros(regNum, 1, CV_64F);
  {
    for (int i = 0; i < regNum; i++)
    {
      for (int j = 0; j < regNum; j++)
      {
        sDistCache1d[j][i]     = sDistCache1d[i][j] = exp(-pntSqrDist(regs[i].centroid, regs[j].centroid)/sigmaDist); 
        sDistCache1dSum[i][0] += sDistCache1d[j][i];
      }
    }
  }
  // normalize the distance sum
  {
    for (int i = 0; i < regNum; i++)
    {
      for (int j = 0; j < regNum; j++)
      {
        sDistCache1d[j][i] /= sDistCache1dSum[i][0];
      }
    }
  }

	Mat_<double> rDistCache1d = Mat::zeros(regNum, regNum, CV_64F);
	regSal1d = Mat::zeros(1, regNum, CV_64F);
	double* regSal = (double*)regSal1d.data;
	for (int i = 0; i < regNum; i++){
		for (int j = 0; j < regNum; j++){
			if(i<j) {
				double dd = 0;
				const vector<CostfIdx> &c1 = regs[i].freIdx, &c2 = regs[j].freIdx;
				for (size_t m = 0; m < c1.size(); m++)
					for (size_t n = 0; n < c2.size(); n++)
						dd += cDistCache1f[c1[m].second][c2[n].second] * c1[m].first * c2[n].first;
				rDistCache1d[j][i] = rDistCache1d[i][j] = dd * sDistCache1d[i][j]; 
			}
			regSal[i] += regs[j].pixNum * rDistCache1d[i][j];
		}
	}
}

Mat RegionSaliency::GetRC(const Mat &img3f)
{
	return GetRC(img3f, 0.4, 50, 50, 0.5);
}

Mat RegionSaliency::GetRC(const Mat &img3f, double sigmaDist, double segK, int segMinSize, double segSigma)
{
	Mat regIdx1i, colorIdx1i, regSal1v, tmp, _img3f, color3fv;
	if (Quantize(img3f, colorIdx1i, color3fv, tmp) <= 2) // Color quantization
		return Mat::zeros(img3f.size(), CV_32F);
	cvtColor(img3f, _img3f, CV_BGR2Lab);
	cvtColor(color3fv, color3fv, CV_BGR2Lab);
	int regNum = SegmentImage(_img3f, regIdx1i, segSigma, segK, segMinSize);	
	vector<Region> regs(regNum);
	BuildRegions(regIdx1i, regs, colorIdx1i, color3fv.cols);
	RegionContrast(regs, color3fv, regSal1v, sigmaDist);

	Mat sal1f = Mat::zeros(img3f.size(), CV_32F);
	cv::normalize(regSal1v, regSal1v, 0, 1, NORM_MINMAX, CV_32F);
	float* regSal = (float*)regSal1v.data;
	for (int r = 0; r < img3f.rows; r++){
		const int* regIdx = regIdx1i.ptr<int>(r);
		float* sal = sal1f.ptr<float>(r);
		for (int c = 0; c < img3f.cols; c++)
			sal[c] = regSal[regIdx[c]];
	}
	GaussianBlur(sal1f, sal1f, Size(3, 3), 0);
	return sal1f;
}

Mat RegionSaliency::GetRCNoColorConversion(const Mat &img3f)
{
	return GetRC(img3f, 0.4, 50, 50, 0.5);
}

Mat RegionSaliency::GetRCNoColorConversion(const Mat &img3f, double sigmaDist, double segK, int segMinSize, double segSigma)
{
	Mat regIdx1i, colorIdx1i, regSal1v, tmp, _img3f, color3fv;
	if (Quantize(img3f, colorIdx1i, color3fv, tmp) <= 2) // Color quantization
		return Mat::zeros(img3f.size(), CV_32F);
  _img3f = img3f.clone();
// 	cvtColor(img3f, _img3f, CV_BGR2Lab);
// 	cvtColor(color3fv, color3fv, CV_BGR2Lab);
	int regNum = SegmentImage(_img3f, regIdx1i, segSigma, segK, segMinSize);	
	vector<Region> regs(regNum);
	BuildRegions(regIdx1i, regs, colorIdx1i, color3fv.cols);
	RegionContrast(regs, color3fv, regSal1v, sigmaDist);

	Mat sal1f = Mat::zeros(img3f.size(), CV_32F);
	cv::normalize(regSal1v, regSal1v, 0, 1, NORM_MINMAX, CV_32F);
	float* regSal = (float*)regSal1v.data;
	for (int r = 0; r < img3f.rows; r++){
		const int* regIdx = regIdx1i.ptr<int>(r);
		float* sal = sal1f.ptr<float>(r);
		for (int c = 0; c < img3f.cols; c++)
			sal[c] = regSal[regIdx[c]];
	}
	GaussianBlur(sal1f, sal1f, Size(3, 3), 0);
	return sal1f;
}

void RegionSaliency::BuildRegions(const Mat& regIdx1i, vector<Region> &regs, const Mat &colorIdx1i, int colorNum)
{
	int rows = regIdx1i.rows, cols = regIdx1i.cols, regNum = (int)regs.size();
	Mat_<int> regColorFre1i = Mat_<int>::zeros(regNum, colorNum); // region color frequency
	for (int y = 0; y < rows; y++){
		const int *regIdx = regIdx1i.ptr<int>(y);
		const int *colorIdx = colorIdx1i.ptr<int>(y);
		for (int x = 0; x < cols; x++, regIdx++, colorIdx++){
			Region &reg = regs[*regIdx];
			reg.pixNum ++;
			reg.centroid.x += x;
			reg.centroid.y += y;
			regColorFre1i(*regIdx, *colorIdx)++;
		}
	}

	for (int i = 0; i < regNum; i++){
		Region &reg = regs[i];
		reg.centroid.x /= reg.pixNum * cols;
		reg.centroid.y /= reg.pixNum * rows;
		int *regColorFre = regColorFre1i.ptr<int>(i);
		for (int j = 0; j < colorNum; j++){
			float fre = (float)regColorFre[j]/(float)reg.pixNum;
			if (regColorFre[j])
				reg.freIdx.push_back(make_pair(fre, j));
		}
	}
}

void RegionSaliency::RegionContrast(const vector<Region> &regs, const Mat &color3fv, Mat& regSal1d, double sigmaDist)
{	
	Mat_<float> cDistCache1f = Mat::zeros(color3fv.cols, color3fv.cols, CV_32F);{
		Vec3f* pColor = (Vec3f*)color3fv.data;
		for(int i = 0; i < cDistCache1f.rows; i++)
			for(int j= i+1; j < cDistCache1f.cols; j++)
				cDistCache1f[i][j] = cDistCache1f[j][i] = vecDist3(pColor[i], pColor[j]);
	}

	int regNum = (int)regs.size();
	Mat_<double> rDistCache1d = Mat::zeros(regNum, regNum, CV_64F);
	regSal1d = Mat::zeros(1, regNum, CV_64F);
	double* regSal = (double*)regSal1d.data;
	for (int i = 0; i < regNum; i++){
		for (int j = 0; j < regNum; j++){
			if(i<j) {
				double dd = 0;
				const vector<CostfIdx> &c1 = regs[i].freIdx, &c2 = regs[j].freIdx;
				for (size_t m = 0; m < c1.size(); m++)
					for (size_t n = 0; n < c2.size(); n++)
						dd += cDistCache1f[c1[m].second][c2[n].second] * c1[m].first * c2[n].first;
				rDistCache1d[j][i] = rDistCache1d[i][j] = dd * exp(-pntSqrDist(regs[i].centroid, regs[j].centroid)/sigmaDist); 
			}
			regSal[i] += regs[j].pixNum * rDistCache1d[i][j];
		}
	}
}

int RegionSaliency::Quantize(const Mat& img3f, Mat &idx1i, Mat &_color3f, Mat &_colorNum, double ratio)
{
	static const int clrNums[3] = {12, 12, 12};
	static const float clrTmp[3] = {clrNums[0] - 0.0001f, clrNums[1] - 0.0001f, clrNums[2] - 0.0001f};
	static const int w[3] = {clrNums[1] * clrNums[2], clrNums[2], 1};

	CV_Assert(img3f.data != NULL);
	idx1i = Mat::zeros(img3f.size(), CV_32S);
	int rows = img3f.rows, cols = img3f.cols;
	if (img3f.isContinuous() && idx1i.isContinuous())
	{
		cols *= rows;
		rows = 1;
	}

	// Build color pallet
	map<int, int> pallet;
	for (int y = 0; y < rows; y++)
	{
		const float* imgData = img3f.ptr<float>(y);
		int* idx = idx1i.ptr<int>(y);
		for (int x = 0; x < cols; x++, imgData += 3)
		{
			idx[x] = (int)(imgData[0]*clrTmp[0])*w[0] + (int)(imgData[1]*clrTmp[1])*w[1] + (int)(imgData[2]*clrTmp[2]);
			pallet[idx[x]] ++;
		}
	}

	// Find significant colors
	int maxNum = 0;
	{
		int count = 0;
		vector<pair<int, int> > num; // (num, color) pairs in num
		num.reserve(pallet.size());
		for (map<int, int>::iterator it = pallet.begin(); it != pallet.end(); it++)
			num.push_back(pair<int, int>(it->second, it->first)); // (color, num) pairs in pallet
		sort(num.begin(), num.end(), std::greater<pair<int, int> >());

		maxNum = (int)num.size();
		int maxDropNum = cvRound(rows * cols * (1-ratio));
		for (int crnt = num[maxNum-1].first; crnt < maxDropNum && maxNum > 1; maxNum--)
			crnt += num[maxNum - 2].first;
		maxNum = min(maxNum, 256); // To avoid very rarely case
		if (maxNum < 10)
			maxNum = min((int)pallet.size(), 100);
		pallet.clear();
		for (int i = 0; i < maxNum; i++)
			pallet[num[i].second] = i; 

		vector<Vec3i> color3i(num.size());
		for (unsigned int i = 0; i < num.size(); i++)
		{
			color3i[i][0] = num[i].second / w[0];
			color3i[i][1] = num[i].second % w[0] / w[1];
			color3i[i][2] = num[i].second % w[1];
		}

		for (unsigned int i = maxNum; i < num.size(); i++)
		{
			int simIdx = 0, simVal = INT_MAX;
			for (int j = 0; j < maxNum; j++)
			{
				int d_ij = vecSqrDist3(color3i[i], color3i[j]);
				if (d_ij < simVal)
					simVal = d_ij, simIdx = j;
			}
			pallet[num[i].second] = pallet[num[simIdx].second];
		}
	}

	_color3f = Mat::zeros(1, maxNum, CV_32FC3);
	_colorNum = Mat::zeros(_color3f.size(), CV_32S);

	Vec3f* color = (Vec3f*)(_color3f.data);
	int* colorNum = (int*)(_colorNum.data);
	for (int y = 0; y < rows; y++) 
	{
		const Vec3f* imgData = img3f.ptr<Vec3f>(y);
		int* idx = idx1i.ptr<int>(y);
		for (int x = 0; x < cols; x++)
		{
			idx[x] = pallet[idx[x]];
			color[idx[x]] += imgData[x];
			colorNum[idx[x]] ++;
		}
	}
	for (int i = 0; i < _color3f.cols; i++)
  {
    color[i] *= (1.0f/((float)colorNum[i]));
 		//color[i] /= ((float)colorNum[i]); // original code that caused trouble with newer versions; it seems like the division operator is ill defined
  }

	return _color3f.cols;
}
