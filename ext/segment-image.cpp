#include "stdafx.h"

#include "segment-graph.h"
#include "segment-image.h"

// dissimilarity measure between pixels
static inline float diff(const Mat &img3f, int x1, int y1, int x2, int y2)
{
	const Vec3f &p1 = img3f.at<Vec3f>(y1, x1);
	const Vec3f &p2 = img3f.at<Vec3f>(y2, x2);
	return sqrt(sqr(p1[0] - p2[0]) + sqr(p1[1] - p2[1]) + sqr(p1[2] - p2[2]));
}

int SegmentImage(const Mat &_src3f, Mat &pImgInd, double sigma, double c, int min_size)
{
	CV_Assert(_src3f.type() == CV_32FC3);
	int width(_src3f.cols), height(_src3f.rows);
	Mat smImg3f;
	GaussianBlur(_src3f, smImg3f, Size(), sigma, 0, BORDER_REPLICATE);

	// build graph
	edge *edges = new edge[width*height*4];
	int num = 0;
	{
		for (int y = 0; y < height; y++) {
			for (int x = 0; x < width; x++) {
				if (x < width-1) {
					edges[num].a = y * width + x;
					edges[num].b = y * width + (x+1);
					edges[num].w = diff(smImg3f, x, y, x+1, y);
					num++;
				}

				if (y < height-1) {
					edges[num].a = y * width + x;
					edges[num].b = (y+1) * width + x;
					edges[num].w = diff(smImg3f, x, y, x, y+1);
					num++;
				}

				if ((x < width-1) && (y < height-1)) {
					edges[num].a = y * width + x;
					edges[num].b = (y+1) * width + (x+1);
					edges[num].w = diff(smImg3f, x, y, x+1, y+1);
					num++;
				}

				if ((x < width-1) && (y > 0)) {
					edges[num].a = y * width + x;
					edges[num].b = (y-1) * width + (x+1);
					edges[num].w = diff(smImg3f, x, y, x+1, y-1);
					num++;
				}
			}
		}
	}
	
	// segment
	universe *u = segment_graph(width*height, num, edges, (float)c);

	// post process small components
	for (int i = 0; i < num; i++) {
		int a = u->find(edges[i].a);
		int b = u->find(edges[i].b);
		if ((a != b) && ((u->size(a) < min_size) || (u->size(b) < min_size)))
			u->join(a, b);
	}
	delete [] edges;

	// pick random colors for each component
	map<int, int> marker;
	pImgInd.create(smImg3f.size(), CV_32S);

	int idxNum = 0;
	for (int y = 0; y < height; y++) {
		int *imgIdx = pImgInd.ptr<int>(y);
		for (int x = 0; x < width; x++) {
			int comp = u->find(y * width + x);
			if (marker.find(comp) == marker.end())
				marker[comp] = idxNum++;

			int idx = marker[comp];
			imgIdx[x] = idx;
		}
	}  
	delete u;

	return idxNum;
}
