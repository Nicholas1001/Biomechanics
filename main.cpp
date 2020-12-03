#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;


Mat src_gray;
int thresh = 100;
RNG rng(12345);
void thresh_callback(int, void* );

int main( int argc, char** argv )
{
    
    Mat src = imread("/Users/louissherratt/Documents/opentest/opentest/q3.jpg");
    // Check for failure
    if (src.empty())
    {
     cout << "Could not open or find the image" << endl;
     cin.get(); //wait for any key press
     return -1;
    }
    
    cvtColor( src, src_gray, COLOR_BGR2GRAY );
    //blur( src_gray, src_gray, Size(3,3) );
    const char* source_window = "Source";
    namedWindow( source_window );
    imshow( source_window, src );
    const int max_thresh = 255;
    createTrackbar( "Canny thresh:", source_window, &thresh, max_thresh, thresh_callback );
    thresh_callback( 0, 0 );
    waitKey();
    return 0;
}
void thresh_callback(int, void* )
{
    Mat canny_output;
    Canny( src_gray, canny_output, thresh, thresh*2 );
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    const char* source_window = "canny";
    namedWindow( source_window );
    imshow( source_window, canny_output );
    findContours( canny_output, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE );
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    
    for( size_t i = 0; i< contours.size(); i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
        drawContours( drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0 );
    }
    imshow( "Contours", drawing );
}

