#include "updatepatches.h"
using namespace cv;
using namespace std;

UpdatePatches::UpdatePatches()
{
}
//  this function find out object regions using templates available
bool UpdatePatches::TemplateMatching(TemplatePatches &Input)
{
    cv::Mat ref,tpl,baseImage;
    baseImage=Input.Image.clone();

    for(int Index=Input.PatchNumber;Index<Input.Patches.size();Index++)
    {
        ref=Input.Image.clone();
        tpl=Input.Patches[Index].clone();

        if (ref.empty() || tpl.empty())
        {
            cout << "Error reading file(s)!"<< endl;
            return -1;
        }
        for(int Index=0;Index<Input.RoiOfPatches.size();Index++)
        {
            Mat Out;
            Out=FillPatches(ref(Input.RoiOfPatches[Index]));
            Out.copyTo(ref(Input.RoiOfPatches[Index]));
        }
        Input.Image=ref.clone();
       // imshow("input image to template match", ref);
       // imshow("template", tpl);
        waitKey(10);
        Mat res_32f(ref.rows - tpl.rows + 1, ref.cols - tpl.cols + 1, CV_32FC1);
        matchTemplate(ref, tpl, res_32f, CV_TM_CCOEFF_NORMED);//parameter to tune

        Mat res,whiteCount;
        res_32f.convertTo(res, CV_8U, 255.0);

        int size = ((tpl.cols + tpl.rows) / 4) * 2 + 1; //force size to be odd
        //////////////////////////////////////////////////////////////////////
        //additional hard threshold
        threshold(res,res,120,255,CV_THRESH_BINARY);
        //////////////////////////////////////////////////////////////////////
        // imshow("result_thresh_added", res);
        //        adaptiveThreshold(res, res, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, size, -128);
        //        imshow("result_thresh", res);
        findNonZero(res,whiteCount);
        if(whiteCount.rows==0)
        {
            continue;
        }
        while (true)
        {
            double minval, maxval, threshold = 0.8;
            Point minloc, maxloc;

            minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);
            Rect Roi(maxloc, Point(maxloc.x + tpl.cols, maxloc.y + tpl.rows));

            if (maxval >= threshold)
            {
                // rectangle(ref, maxloc, Point(maxloc.x + tpl.cols, maxloc.y + tpl.rows), CV_RGB(0,255,0), 2);
                Input.candidateRois.push_back(Roi);
                floodFill(res, maxloc, 0); //mark drawn blob

            }
            else
                break;
        }
        RemoveOverlapedRois(Input.candidateRois,.5);

        for(int RoiIndex=0;RoiIndex<Input.candidateRois.size();RoiIndex++)
        {
            Mat Patch=baseImage(Input.candidateRois[RoiIndex]).clone();
            bool Matched=IsPatchMatched(Patch,Input.InitialPatch);
            if(Matched)
            {
                Input.candidatePatches.push_back(Patch);
                Input.RoiOfPatches.push_back(Input.candidateRois[RoiIndex]);
                rectangle(ref,Input.candidateRois[Index],CV_RGB(0,255,0),2);
            }
            else
            {
                Input.candidateRois.erase( Input.candidateRois.begin()+RoiIndex);
            }
        }

        //cout<<"size of candidate="<<
        Input.candidateRois.clear();
    }

    if(Input.candidatePatches.size()==0)
    {
        return false;
    }

    return true;
}

//
// This Function removes overlaped Rois using distance threshold
//

bool UpdatePatches::RemoveOverlapedRois(std::vector<cv::Rect> &RoiArray,float ScaleFactor)
{
    if(RoiArray.empty())
    {
        return false;
    }
    int RowDist=0,ColDist=0;
    int xDiff=0,yDiff=0;
    for(int Index=0;Index<RoiArray.size();Index++)
    {
        RowDist=RoiArray[Index].height;
        RowDist*=ScaleFactor;
        ColDist=RoiArray[Index].width;
        ColDist*=ScaleFactor;

        for(int current=Index+1;current<RoiArray.size();current++)
        {
            xDiff=abs(RoiArray[Index].x-RoiArray[current].x);
            yDiff=abs(RoiArray[Index].y-RoiArray[current].y);
            if(((xDiff<=ColDist)&&(yDiff<=RowDist)))
            {
                RoiArray.erase(RoiArray.begin()+current);
                Index--;
            }
        }
    }
    return true;
}
//this function find out the center pixel value
bool  UpdatePatches::IsPatchMatched(cv::Mat CandidatePatch,cv::Mat InitialPatch)
{
    Point MidPoint=Point(CandidatePatch.cols/2,CandidatePatch.rows/2);
    BlueAvg=0;
    GreenAvg=0;
    RedAvg=0;
    int BackPixelCount=0;
    Mat NumPixels;
    Mat_<Vec3b> _I = InitialPatch;
    int b = _I(MidPoint.y,MidPoint.x)[0];
    int g = _I(MidPoint.y,MidPoint.x)[1];
    int r = _I(MidPoint.y,MidPoint.x)[2];
    int coTh=25;
    Mat SimOut;
    inRange(CandidatePatch, Scalar(b-coTh,g-coTh,r-coTh), Scalar(b+coTh,g+coTh,r+coTh), SimOut);

//    imshow("inrange", SimOut);
//    imshow("cand", CandidatePatch);
//    waitKey(10);

    findNonZero(SimOut,NumPixels);
    int MatchingPixelCount = NumPixels.rows;
    float percMatch = MatchingPixelCount/(float)(CandidatePatch.rows*CandidatePatch.cols);
    for(int row=0;row<CandidatePatch.rows;row++)
    {
        for(int col=0;col<CandidatePatch.cols;col++)
        {
            if(SimOut.at<uchar>(row,col)==0)
            {BackPixelCount++;
                BlueAvg+=_I(row,col)[0];
                GreenAvg+=_I(row,col)[1];
                RedAvg+=_I(row,col)[2];
            }
        }
    }
    BlueAvg/=BackPixelCount;
    GreenAvg/=BackPixelCount;
    RedAvg/=BackPixelCount;

    if(percMatch > 0.05)
    {
        cout<<"Matched"<<endl;
        return true;
    }
    else
    {
        cout<<"Not Matched!!"<<endl;
        return false;
    }
}
Mat  UpdatePatches::FillPatches(cv::Mat Image)
{
    vector<Mat> Channels;
    Mat Combined;
    Mat Red(Image.rows,Image.cols,CV_8SC1,Scalar(RedAvg));
    Mat Blue(Image.rows,Image.cols,CV_8SC1,Scalar(BlueAvg));
    Mat Green(Image.rows,Image.cols,CV_8SC1,Scalar(GreenAvg));
    Channels.push_back(Blue);
    Channels.push_back(Green);
    Channels.push_back(Red);
    merge(Channels,Combined);
    Image=Combined.clone();
    //imshow("background",Image);
    return Image;
}
