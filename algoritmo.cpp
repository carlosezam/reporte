#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>
#include <ctime>
#include <windows.h>

//#define DEBUG


#define IMGW 640
#define IMGH 480

#define BLUR 3
#define THRESHOLD 37
#define DILATE 4


using namespace std;
//using namespace cv;
int main( int argc, char** argv ) {
  
  
  /*
  * Variables para la medición del tiemp de ejecución
  */
  
  double    time = 0;
  unsigned long  tick = 0;
  unsigned long tock = 0;



  /* 
  * Variables necesarias para la manipulacion de las imagenes
  */

  cv::Mat foreground;  // Captura actual
  cv::Mat background;  // Carretera de fondo

  cv::Mat fg_rsz;      // Captura escalada
  cv::Mat bg_rsz;      // Carretera escalada

  cv::Mat fg_blr;      // Captura desenfocada
  cv::Mat bg_blr;      // Carretera desenfocada
  
  cv::Mat img_dif;     // Resultado de aplicar diferencia absoluta a las tomas
  cv::Mat img_thd;     // Resultado de aplicar threshold
  cv::Mat img_dlt;     // Resultado de la dilatacion


  /*
   * Variables necesarias para la detección y procesamiento de los contornos
   */

  vector< vector< cv::Point > > contours;
  vector< cv::Vec4i > hierarchy;
  cv::Mat element;

  // toma de tiempo inicial
  tick = clock();
  
  // Carga de la imagen y transformacion a escala de grises
  // Para obtener las imagenes desde una camara se recomienda usar:
  // cv::createFileCapture( ID_CAMARA )
  foreground = cv::imread("foreground.png" , CV_LOAD_IMAGE_GRAYSCALE);
  background = cv::imread("background.png" , CV_LOAD_IMAGE_GRAYSCALE);

  if( !foreground.data || !background.data )
  {
      std::cout <<  "Alguna de las imagenes no se encuentra o no puede abrirse" << std::endl ;
      return -1;
  }
  
  #ifdef DEBUG
  cv::namedWindow( "Display window", cv::WINDOW_AUTOSIZE );
  #endif

  // Redimencionamiento de la imagen
  cv::resize(foreground, fg_rsz, cv::Size(IMGW,IMGH));
  cv::resize(background, bg_rsz, cv::Size(IMGW,IMGH));

  #ifdef DEBUG
  cv::imshow( "Display window", fg_rsz ); cv::waitKey(0);
  #endif

  // Desenfoque gausiano para suavisar los bordes
  cv::blur(fg_rsz, fg_blr, cv::Size(BLUR,BLUR) ); //cv::GaussianBlur(fg_rsz, fg_blr, cv::Size(21,21), 0);
  cv::blur(bg_rsz, bg_blr, cv::Size(BLUR,BLUR) ); //cv::GaussianBlur(bg_rsz, bg_blr, cv::Size(21,21), 0);
  
  #ifdef DEBUG
  cv::imshow( "Display window", fg_rsz ); cv::waitKey(0);
  #endif


  // Diferencia absoluta para detectar las direnecias entre las dos imagenes
  cv::absdiff(fg_blr, bg_blr, img_dif);

  #ifdef DEBUG
  cv::imshow( "Display window", img_dif ); cv::waitKey(0);
  #endif

  
  // Aplica threshold para crear una imagen binaria sin escalas de grises
  cv::threshold( img_dif, img_thd, THRESHOLD, 255, cv::THRESH_BINARY);

  #ifdef DEBUG
  cv::imshow( "Display window", img_thd ); cv::waitKey(0);
  #endif
  
 
  // Dilata la imagen para unificar los contornos adyacentes
  element = cv::getStructuringElement( cv::MORPH_RECT, cv::Size(DILATE*2+1,DILATE*2+1), cv::Point(DILATE,DILATE) );
  cv::dilate(img_thd, img_dlt, element);

  #ifdef DEBUG
  cv::imshow( "Display window", img_dlt ); cv::waitKey(0);
  #endif


  // Busca los contornos presentes en la imagen
  cv::findContours( img_dlt, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE, cv::Point(0,0) );

  #ifdef DEBUG
  vector< vector< cv::Point > > contours_poly( contours.size() );
  vector< cv::Rect > boundRect( contours.size() );
  #endif

  //vector<Point2f> center( contours.size() );
  //vector<float> radius( contours.size() );
  int cont = 0;

  for( int i = 0; i < contours.size(); ++i )
  {
    
    //cv::minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );

    auto c = contours[ i ];

    if( cv::contourArea( c ) > 12000 )
    {
      ++cont;

      #ifdef DEBUG

      cv::Scalar color( 255, 000, 000);
      //cv::drawContours( fg_rsz, contours, i, color, 2, 8, hierarchy, 0 , Point() );
      
      cv::approxPolyDP( cv::Mat(contours[i]), contours_poly[i], 3, true);
      boundRect[ i ] = cv::boundingRect( cv::Mat(contours_poly[i]) );
      cv::rectangle( fg_rsz, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);

      //cv::circle( fg_rsz, center[i], (int)radius[i], color, 2, 8, 0);
      #endif

    }
    
  }


  tock = clock();
  time = ((double)(tock-tick)/CLOCKS_PER_SEC);

  std::cout << "time: " << time << ", clocks per sec: "<< CLOCKS_PER_SEC <<std::endl;
  std::cout << "contornos: " << cont << std::endl;  
  


  #ifdef DEBUG
  cv::imshow( "Display window", fg_rsz );
  cv::waitKey(0);
  #endif
  //cv::imshow( "Display window", dst );
  //cv::waitKey(0);

 

  return 0;
}
