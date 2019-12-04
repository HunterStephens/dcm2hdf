/**
 * ROI Class Declaration
 * @author Hunter Stephens
 * @version 0.0.1
*/

#ifndef DCM2HDF_ROI_H
#define DCM2HDF_ROI_H

#include <string>
#include <vector>


class roi {
public:
    /**
     * Data Members
     */
     std::string name;               // ROI label
     int ID;                         // ROI tag in DICOM
     std::vector<double*> contours;  // Vector of contours
     std::vector<double> num_points; // Number of points in each contour
     double* x;                      // ROI voxel coordinates
     double* y;
     double* z;

     /**
      * Constructor
      * @param none
      * @return none
      */
      roi();

};


#endif //DCM2HDF_ROI_H
