/**
 * CT Class Declaration
 * @author Hunter Stephens
 * @version 0.0.1
*/

#ifndef DCM2HDF_CT_H
#define DCM2HDF_CT_H

#include <iostream>
#include "dcmtk/dcmdata/dctk.h"
#include "dcmtk/dcmimgle/dcmimage.h"
#include <boost/filesystem.hpp>
#include <string>
#include <vector>
#include <sstream>
#include <math.h>


class ct {
public:
    /**
     * Data Members
     */
    double*** data;                  // 3D tensor of CT data
    double* x;                       // x coordinate data
    double* y;                       // y coordinate data
    double* z;                       // z coordinate data
    unsigned int N_cols;             // number of columns in CT image
    unsigned int N_rows;             // number of rows in CT image
    unsigned int N_slices;           // number of CT slices in stack
    std::string pth_name;


    /**
     * Constructor
     * @param none
     * @return none
     */
     ct();

     /**
      * Parametrized Constructor
      * @param std::string path to DICOM directory
      */
     ct(std::string);

     /**
      * Resize Method
      * Re-samples the CT volume given a scalar value
      * @param int scaling factor
      */
      void resize(int);

      /**
       * Interpolation Method
       * Interpolate a point within the CT volume
       * @param double x-value
       * @param double y-value
       * @param double z-value
       * @return double interpoalted value
       */
       double interp(double,double,double);

};


#endif //DCM2HDF_CT_H
