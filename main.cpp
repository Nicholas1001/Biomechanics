#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <vector>

using namespace cv;
using namespace std;

/*int main(int argc, char** argv)
{
 // Read the image file
 Mat image = imread("/Users/louissherratt/Documents/opentest/opentest/q9.jpg");
 // Check for failure
 if (image.empty())
 {
  cout << "Could not open or find the image" << endl;
  cin.get(); //wait for any key press
  return -1;
 }
    
    Mat contours;
    //Mat gray_image;
    //change the color image to grayscale image
    cvtColor(image, image, COLOR_BGR2GRAY);
    
    Mat hist_equalized_image;
    equalizeHist(image, hist_equalized_image);
    
    hist_equalized_image = hist_equalized_image < 0.4;
    
    //Define names of windows
    String windowNameOfOriginalImage = "Original Image";
    String windowNameOfHistogramEqualized = "Histogram Equalized Image";
    
    // Create windows with the above names
    namedWindow(windowNameOfOriginalImage, WINDOW_NORMAL);
    namedWindow(windowNameOfHistogramEqualized, WINDOW_NORMAL);
    // Show images inside created windows.
    imshow(windowNameOfOriginalImage, image);
    imshow(windowNameOfHistogramEqualized, hist_equalized_image);
    Canny(image,contours,10,350);
    namedWindow("Canny");
    imshow("Canny",contours);
    waitKey(0);
    
    destroyAllWindows(); //Destroy all open windows
    return 0;
}*/
    
    
/*    //change the color image to grayscale image
      cvtColor(image, image, COLOR_BGR2GRAY);
      //equalize the histogram
      Mat hist_equalized_image;
      equalizeHist(image, hist_equalized_image);
                                                 
      //Define names of windows
      String windowNameOfOriginalImage = "Original Image";
      String windowNameOfHistogramEqualized = "Histogram Equalized Image";
      // Create windows with the above names
      namedWindow(windowNameOfOriginalImage, WINDOW_NORMAL);
      namedWindow(windowNameOfHistogramEqualized, WINDOW_NORMAL);
      // Show images inside created windows.
      imshow(windowNameOfOriginalImage, image);
      imshow(windowNameOfHistogramEqualized, hist_equalized_image);
      waitKey(0); // Wait for any keystroke in one of the windows
      destroyAllWindows(); //Destroy all open windows
      return 0;
  }*/

void detect_hand_and_fingers(Mat& src);
void detect_hand_silhoutte(Mat& src);

int main(int argc, char* argv[])
{
    Mat img = imread("/Users/louissherratt/Documents/opentest/opentest/clipart.jpeg");
    // Check for failure
    if (img.empty())
    {
     cout << "Could not open or find the image" << endl;
     cin.get(); //wait for any key press
     return -1;
    }

    // Convert RGB Mat to GRAY
    Mat gray, fingersp;
    cvtColor(img, gray, COLOR_BGR2GRAY);
    
    Mat gray_silhouette = gray.clone();

    /* Isolate Hand + Fingers */

    detect_hand_and_fingers(gray);

    imshow("Hand+Fingers", gray);
    //imwrite("hand_fingers.png", gray);

    /* Isolate Hand Sillhoute and subtract it from the other image (Hand+Fingers) */

    detect_hand_silhoutte(gray_silhouette);
    imshow("Hand", gray_silhouette);
    //imwrite("hand_silhoutte.png", gray_silhouette);

    /* Subtract Hand Silhoutte from Hand+Fingers so we get only Fingers */

    Mat fingers =  gray - gray_silhouette;
    imshow("Fingers", fingers);
    //imwrite("fingers_only.png", fingers);
    
    Mat minusfingers = gray - fingers;

    dilate(minusfingers, minusfingers, Mat(), Point(-1, -1), 2, 1, 1);
    
    imshow("F", minusfingers);
    
    RNG rng(100);
        vector<vector<Point> > contours;
        vector<Vec4i> hierarchy;
        
        findContours( minusfingers, contours, hierarchy, RETR_TREE, CHAIN_APPROX_NONE );
        Mat drawing = Mat::zeros( gray.size(), CV_8UC3 );
        
        for( size_t i = 0; i< contours.size(); i++ ){
            /*if( contours[i].size() < 10) {
               contours.erase(contours.begin()+i);
            }*/
        
            Scalar color = Scalar( rng.uniform(0, 256), rng.uniform(0,256), rng.uniform(0,256) );
            drawContours( drawing, contours, (int)i, color, 2, LINE_8, hierarchy, 0 );
        }
    /*cnt = conours.get(20);
    let epsilon = 0.1*arcLength(cnt,curve);
    let approx = approxPolyDP(cnt,epsilon,curve);*/
        
        imshow( "Contours", drawing );
    
    
    
    
    

    
    
    
    
    
    
    
    waitKey(0);
    return 0;
}

void detect_hand_and_fingers(Mat& src)
{
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(3,3), Point(1,1));
    morphologyEx(src, src, MORPH_ELLIPSE, kernel);

    int adaptiveMethod = ADAPTIVE_THRESH_GAUSSIAN_C; // CV_ADAPTIVE_THRESH_MEAN_C, CV_ADAPTIVE_THRESH_GAUSSIAN_C
    adaptiveThreshold(src, src, 255, adaptiveMethod, THRESH_BINARY, 9, -5);

    int dilate_sz = 1;
    Mat element = getStructuringElement(MORPH_ELLIPSE, Size(2*dilate_sz, 2*dilate_sz),
                                       Point(dilate_sz, dilate_sz) );
    dilate(src, src, element);
}

void detect_hand_silhoutte(Mat& src)
{
    Mat kernel = getStructuringElement(MORPH_ELLIPSE, Size(7, 7), Point(3, 3));
    morphologyEx(src, src, MORPH_ELLIPSE, kernel);

    int adaptiveMethod = ADAPTIVE_THRESH_MEAN_C; // CV_ADAPTIVE_THRESH_MEAN_C, CV_ADAPTIVE_THRESH_GAUSSIAN_C
    adaptiveThreshold(src, src, 255,
                          adaptiveMethod, THRESH_BINARY,
                          251, 5); // 251, 5

    int erode_sz = 5;
    Mat element = getStructuringElement(MORPH_ELLIPSE,
                                       Size(2*erode_sz + 1, 2*erode_sz+1),
                                       Point(erode_sz, erode_sz) );
    erode(src, src, element);

    int dilate_sz = 1;
    element = getStructuringElement(MORPH_ELLIPSE,
                                       Size(2*dilate_sz + 1, 2*dilate_sz+1),
                                       Point(dilate_sz, dilate_sz) );
    dilate(src, src, element);

    bitwise_not(src, src);
}
