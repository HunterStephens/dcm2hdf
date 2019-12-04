/**
 * ROI Set Class Declaration
 * @author Hunter Stephens
 * @version 0.0.1
*/

#ifndef DCM2HDF_ROI_SET_H
#define DCM2HDF_ROI_SET_H

#include <iostream>
#include "dcmtk/dcmrt/drmstrct.h"
#include "roi.h"
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>
#include <bits/stdc++.h>
#include <string>
#include <vector>


class roi_set {
public:

    /**
     * Data Members
     */
     int num_rois;           // Number of ROIs in set
     std::vector<roi> rois;  // List of ROIs

     /**
      * Constructors
      */
      roi_set();
      roi_set(std::string);

};


#endif //DCM2HDF_ROI_SET_H
