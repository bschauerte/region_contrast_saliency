/**
 * Ming-Ming Cheng, Guo-Xin Zhang, Niloy J. Mitra, Xiaolei Huang, Shi-Min Hu. Global Contrast based Salient Region Detection. IEEE CVPR, p. 409-416, Colorado Springs, Colorado, USA, June 21-23, 2011.
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
  
// 	// Get saliency values of a group of images.
// 	// Input image names and directory name for saving saliency maps.
// 	static void Get(const string &imgNameW, const string &salDir);
// 
// 	// Evaluate saliency detection methods. Input ground truth file names and saliency map directory
// 	static void Evaluate(const string gtImgW, const string &salDir, const string &resName);

	// Frequency Tuned [1].
	static cv::Mat GetFT(const cv::Mat &img3f);

	// Histogram Contrast of [3]
	static cv::Mat GetHC(const cv::Mat &img3f);

	// Region Contrast 
	static cv::Mat GetRC(const cv::Mat &img3f);
	static cv::Mat GetRC(const cv::Mat &img3f, double sigmaDist, double segK, int segMinSize, double segSigma);
  static cv::Mat GetRCDebiased(const cv::Mat &img3f, double sigmaDist = 0.4, double segK = 50, int segMinSize = 50, double segSigma = 0.5);
  static cv::Mat GetRCLocallyDebiased(const cv::Mat &img3f, double sigmaDist = 0.4, double segK = 50, int segMinSize = 50, double segSigma = 0.5);

  // Region Contrast but without intrinsic rgb2lab conversion
  static cv::Mat GetRCNoColorConversion(const cv::Mat &img3f);
  static cv::Mat GetRCNoColorConversion(const cv::Mat &img3f, double sigmaDist, double segK, int segMinSize, double segSigma);
  
	// Region Contrast - Center-Biased
	//static cv::Mat GetRCCB(const cv::Mat &img3f);
	static cv::Mat GetRCCB(const cv::Mat &img3f, 
          double sigmaDist = 0.4, 
          double segK = 50, 
          int segMinSize = 50, 
          double segSigma = 0.5, 
          double centerBiasWeight = 0.5, 
          double centerBiasHeightSigma = 0.5, 
          double centerBiasWidthSigma = 0.5,
          const CenterBiasCombinationType_t cbct = CB_LINEAR);

  // Region Contrast (debiased) - Center-Biased
	static cv::Mat GetRCCBDebiased(const cv::Mat &img3f, 
          double sigmaDist = 0.4, 
          double segK = 50, 
          int segMinSize = 50, 
          double segSigma = 0.5, 
          double centerBiasWeight = 0.5, 
          double centerBiasHeightSigma = 0.5, 
          double centerBiasWidthSigma = 0.5);

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
  
	// Luminance Contrast [2]
	static cv::Mat GetLC(const cv::Mat &img3f);

	// Spectral Residual [4]
	static cv::Mat GetSR(const cv::Mat &img3f);

	// Color quantization
	static int Quantize(const cv::Mat& img3f, cv::Mat &idx1i, cv::Mat &_color3f, cv::Mat &_colorNum, double ratio = 0.95);

private:
	static const int SAL_TYPE_NUM = 5;
	typedef cv::Mat (*GET_SAL_FUNC)(const cv::Mat &);
	static const char* SAL_TYPE_DES[SAL_TYPE_NUM];
	static const GET_SAL_FUNC gFuns[SAL_TYPE_NUM];

	// Histogram based Contrast
	static void GetHC(const cv::Mat &binColor3f, const cv::Mat &weights1f, cv::Mat &colorRegionSaliency);

	//static void Evaluate(const string& resultW, const string &gtImgW, vecD &precision, vecD &recall);
	//static int PrintVector(FILE *f, const vecD &v, const string &name, int maxNum = 1000);

	static void SmoothSaliency(const cv::Mat &binColor3f, cv::Mat &sal1d, float delta, const std::vector<std::vector<CostfIdx> > &similar);

	static void AbsAngle(const cv::Mat& cmplx32FC2,cv::Mat& mag32FC1, cv::Mat& ang32FC1);
	static void GetCmplx(const cv::Mat& mag32F, const cv::Mat& ang32F, cv::Mat& cmplx32FC2);


	struct Region{
		Region() { pixNum = 0;}
		int pixNum;  // Number of pixels
		std::vector<CostfIdx> freIdx;  // Frequency of each color and its index
		Point2d centroid;
	};
	static void BuildRegions(const cv::Mat& regIdx1i, std::vector<Region> &regs, const cv::Mat &colorIdx1i, int colorNum);
	static void RegionContrast(const std::vector<Region> &regs, const cv::Mat &color3fv, cv::Mat& regSal1d, double sigmaDist);
  static void RegionContrastDebiased(const std::vector<Region> &regs, const cv::Mat &color3fv, cv::Mat& regSal1d, double sigmaDist);
  static void RegionContrastLocallyDebiased(const vector<Region> &regs, const Mat &color3fv, Mat& regSal1d, double sigmaDist);
};

/************************************************************************/
/*[1]R. Achanta, S. Hemami, F. Estrada and S. Susstrunk, Frequency-tuned*/
/*   Salient Region Detection, IEEE International Conference on Computer*/
/*	 Vision and Pattern Recognition (CVPR), 2009.						*/
/*[2]Y. Zhai and M. Shah. Visual attention detection in video sequences */
/*   using spatiotemporal cues. In ACM Multimedia, pages 815�C824. ACM, */
/*   2006.																*/
/*[3]Our paper															*/
/*[4]X. Hou and L. Zhang. RegionSaliency detection: A spectral residual		*/
/*   approach. In IEEE Conference on Computer Vision and Pattern		*/
/*	 Recognition, 2007. CVPR��07, pages 1�C8, 2007.						*/
/************************************************************************/
