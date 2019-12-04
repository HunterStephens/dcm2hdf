/**
 * CT Class Implementation
 * @author Hunter Stephens
 * @version 0.0.1
*/

#include "ct.h"

using namespace boost::filesystem;


/**
 * Constructor
 * @param none
 * @return none
 */
 ct::ct(){
     N_rows = 0;
     N_cols = 0;
     N_slices = 0;
     data = new double**;
     data[0] = new double*;
     data[0][0] = new double;
     x = new double;
     y = new double;
     z = new double;
 }

/**
 * Parametrized Constructor
 * @param std::string path to DICOM directory
 */
 ct::ct(std::string pth){
     /**
      * loop through directory looing for DICOM CT files
      */
      path p = pth;
      pth_name = pth;
      std::vector<std::string> fnames;
      directory_iterator it{p};
      while (it != directory_iterator{}) {
          std::string str = (*it).path().string();
          std::istringstream ss(str);
          std::string file_name;
          while(std::getline(ss, file_name, '/')){}

          std::istringstream name(file_name);
          std::string type;
          std::getline(name, type, '.');
          if(type == "CT"){
              fnames.push_back(str);
          }
          it++;
      }
      N_slices = fnames.size(); //set number of slices to number of files

      /*
       * Get DICOM Data
       */

      // initialze size of CT stack
      DicomImage *image_ref = new DicomImage(fnames[0].c_str());
      N_rows = image_ref->getHeight();
      N_cols = image_ref->getWidth();
      double ***temp = new double**[N_rows];
      for(int i=0; i<N_rows;i++){
          temp[i] = new double*[N_cols];
          for(int j=0; j<N_cols; j++){
              temp[i][j] = new double[N_slices];
          }
      }
      data = temp;  // point data towards initialized CT stack

      /*
       * Init CT coordinates
       */
      DcmFileFormat init_file;
      DcmDataset* init_set;
      OFCondition status = init_file.loadFile(fnames[0].c_str());
      init_set = init_file.getDataset();
      OFString mstr;
      init_set->findAndGetOFString(DCM_SliceThickness,mstr);
      double dz = atof(mstr.c_str());
      init_set->findAndGetOFStringArray(DCM_PixelSpacing,mstr);
      std::istringstream st(mstr.c_str());
      double deltaj, deltai;
      std::string test;
      std::getline(st, test, '\\');
      deltaj = atof(test.c_str());
      std::getline(st, test, '\\');
      deltai = atof(test.c_str());

      init_set->findAndGetOFStringArray(DCM_ImagePositionPatient,mstr);
      std::istringstream st2(mstr.c_str());
      double xnot, ynot;
      std::string test2;
      std::getline(st2, test2, '\\');
      xnot = atof(test2.c_str());
      std::getline(st2, test2, '\\');
      ynot = atof(test2.c_str());

      OFString orient_str;
      init_set->findAndGetOFStringArray(DCM_ImageOrientationPatient, orient_str);
      std::istringstream st3(orient_str.c_str());
      double Xx, Xy, Xz, Yx, Yy, Yz;
      std::string test3;
      std::getline(st3, test3, '\\');
      Xx = atof(test3.c_str());
      std::getline(st3, test3, '\\');
      Xy = atof(test3.c_str());
      std::getline(st3, test3, '\\');
      Xz = atof(test3.c_str());
      std::getline(st3, test3, '\\');
      Yx = atof(test3.c_str());
      std::getline(st3, test3, '\\');
      Yy = atof(test3.c_str());
      std::getline(st3, test3, '\\');
      Yz = atof(test3.c_str());

      double **X;
      double **Y;
      X = new double*[N_rows];
      Y = new double*[N_rows];
      for(int i=0; i<N_rows; i++){
          X[i] = new double[N_cols];
          Y[i] = new double[N_cols];
          for(int j=0; j<N_cols; j++){
              Y[i][j] = deltai*Xx*(double)i+deltaj*Yx*(double)j+ynot;
              X[i][j] = deltai*Xy*(double)i+deltaj*Yy*(double)j+xnot;
          }
      }
      x = new double[N_cols];
      y = new double[N_rows];
      z = new double[N_slices];
      // create voxel center coordinates
      double dx = X[0][1] - X[0][0];
      double dy = Y[1][0] - Y[0][0];
      for(int i=0;i<N_cols; i++){x[i] = X[0][i] + dx/2 ;}
      for(int i=0;i<N_rows; i++){y[i] = Y[i][0] + dy/2;}

      delete image_ref;

      /*
       * Fill stack with CT values
       */
      DcmFileFormat file_format;
      DcmDataset* dataset;
      OFString str;
      double m,b;
      int slice;
      for(int i=0; i< fnames.size(); i++){
          DicomImage *image = new DicomImage(fnames[i].c_str());
          OFCondition status = file_format.loadFile(fnames[i].c_str());
          dataset = file_format.getDataset();

          // get HU intercept
          dataset->findAndGetOFString(DCM_RescaleIntercept,str);
          b = atof(str.c_str());
          // get HU slope
          dataset->findAndGetOFString(DCM_RescaleSlope,str);
          m = atof(str.c_str());
          // get slice number
          dataset->findAndGetOFString(DCM_InstanceNumber,str);
          slice = atoi(str.c_str())-1;
          // get zpos
          dataset->findAndGetOFStringArray(DCM_ImagePositionPatient,str);
          std::istringstream istr(str.c_str());
          std::string str_pos;
          std::getline(istr, str_pos,'\\');
          std::getline(istr, str_pos,'\\');
          std::getline(istr, str_pos,'\\');
          z[slice] = atof(str_pos.c_str()) + dz/2;
          double val;
          int row, col;
          if (image != NULL){
              if (image->getStatus() == EIS_Normal){
                  if (image->isMonochrome()){
                      const DiPixel* pix = image->getInterData();
                      unsigned long num_data = pix->getCount();
                      EP_Representation rep = pix->getRepresentation();
                      if(rep == EPR_Uint8){
                          Uint8 *pixelData = (Uint8*)(pix->getData());
                          for(int i=0; i < num_data; i++) {
                              //get pixel value
                              val = b + (float) pixelData[i] * m;
                              //get indices
                              row = i / N_cols;
                              col = i % N_cols;
                              data[row][col][slice] = val;
                          }
                      }
                      else if(rep == EPR_Sint8){
                          Sint8 *pixelData = (Sint8*)(pix->getData());
                          for(int i=0; i < num_data; i++) {
                              //get pixel value
                              val = b + (float) pixelData[i] * m;

                              //get indices
                              row = i / N_cols;
                              col = i % N_cols;
                              data[row][col][slice] = val;
                          }
                      }
                      else if(rep == EPR_Uint16){
                          Uint16 *pixelData = (Uint16*)(pix->getData());
                          for(int i=0; i < num_data; i++){
                              //get pixel value
                              val = b + (float)pixelData[i]*m;

                              //get indices
                              row = i / N_cols;
                              col = i % N_cols;
                              data[row][col][slice] = val;
                          }
                      }
                      else if(rep == EPR_Sint16){
                          Sint16 *pixelData = (Sint16*)(pix->getData());
                          for(int i=0; i < num_data; i++) {
                              //get pixel value
                              //val = b + (float) pixelData[i] * m;
                              val = (float)pixelData[i];

                              //get indices
                              row = i / N_cols;
                              col = i % N_cols;
                              data[row][col][slice] = val;
                          }

                      }
                      else if(rep == EPR_Uint32){
                          Uint32 *pixelData = (Uint32*)(pix->getData());
                          for(int i=0; i < num_data; i++) {
                              //get pixel value
                              val = b + (float) pixelData[i] * m;

                              //get indices
                              row = i / N_cols;
                              col = i % N_cols;
                              data[row][col][slice] = val;
                          }
                      }
                      else {
                          Sint32 *pixelData = (Sint32 *) (pix->getData());
                          for(int i=0; i < num_data; i++) {
                              //get pixel value
                              val = b + (float) pixelData[i] * m;

                              //get indices
                              row = i / N_cols;
                              col = i % N_cols;
                              data[row][col][slice] = val;
                          }
                      }
                  }
              }
          }
          else{
              std::cerr << "Error: cannot load DICOM image" << std::endl;
          }
          delete image;
      }
}

/**
 * Resize Method
 * @param fac
 */
void ct::resize(int fac) {
    // get anchor points
    double xmin = x[0];
    double xmax = x[N_cols-1];
    double ymin = y[0];
    double ymax = y[N_rows-1];

    // init new coordinates
    double *xnew = new double[N_cols/fac];
    double *ynew = new double[N_rows/fac];
    double num_x = (float)N_cols/fac;
    double num_y = (float)N_rows/fac;
    double dx = (xmax-xmin)/(num_x-1);
    double dy = (ymax-ymin)/(num_y-1);

    // set anchor points
    xnew[0] = xmin;
    ynew[0] = ymin;

    // fill out vectors
    for(int i=1; i < num_x; i++){
        xnew[i] = xmin + (float)i*dx;
    }
    for(int j=1; j<num_y;j++){
        ynew[j] = ymin + (float)j*dy;
    }

    // init new data holder
    int N_rows_rsz = N_rows/fac;
    int N_cols_rsz = N_cols/fac;

    double ***data_rsz = new double**[N_rows_rsz];
    for(int i=0; i< N_rows_rsz; i++){
        data_rsz[i] = new double*[N_cols_rsz];
        for(int j=0; j<N_cols_rsz; j++){
            data_rsz[i][j] = new double[N_slices];
            for(int k=0; k<N_slices; k++){
                double val = interp(xnew[j], ynew[i],z[k]);
                data_rsz[i][j][k] = val;
            }
        }
    }

    // point members to new values
    N_rows = N_rows_rsz;
    N_cols = N_cols_rsz;
    data = data_rsz;
    x = xnew;
    y = ynew;
}


/**
 * Interpolation Method
 * Interpolate on the 3D CT volume
 * @param double x-coordinate
 * @param double y-coordinate
 * @param double z-coordinate
 */
double ct::interp(double xo, double yo, double zo){
    // find neighbor points
    double dx = x[1] - x[0];
    double dy = y[1] - y[0];
    double dz = z[0] - z[1];

    int xin = floor((xo-x[0])/dx);
    int xin1 = xin + 1;
    int yin = floor((yo-y[0])/dy);
    int yin1 = yin + 1;
    int zin = floor((z[0]-zo)/dz);
    int zin1 = zin + 1;


    // calc interpolatns
    double fy2z1 = (data[yin1][xin1][zin] - data[yin1][xin][zin]) / (x[xin1] - x[xin]) * (xo - x[xin]) + data[yin1][xin][zin];
    double fy1z1 = (data[yin][xin1][zin] - data[yin][xin][zin]) / (x[xin1] - x[xin]) * (xo - x[xin]) + data[yin][xin][zin];

    double fy2z2 = (data[yin1][xin1][zin1] - data[yin1][xin][zin1]) / (x[xin1] - x[xin]) * (xo - x[xin]) + data[yin1][xin][zin1];
    double fy1z2 = (data[yin][xin1][zin1] - data[yin][xin][zin1]) / (x[xin1] - x[xin]) * (xo - x[xin]) + data[yin][xin][zin1];

    double fz1 = (fy2z1 - fy1z1) / (y[yin1] - y[yin]) * (yo - y[yin]) + fy1z1;
    double fz2 = (fy2z2 - fy1z2) / (y[yin1] - y[yin]) * (yo - y[yin]) + fy1z2;

    double val = (fz2 - fz1) / (z[zin1] - z[zin]) * (zo - z[zin]) + fz1;

    return (fz2 - fz1) / (z[zin1] - z[zin]) * (zo - z[zin]) + fz1;
}





