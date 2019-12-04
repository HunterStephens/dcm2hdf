/**
 * Patient Class Declaration
 * @author Hunter Stephens
 * @version 0.0.1
*/
#ifndef DCM2HDF_PATIENT_H
#define DCM2HDF_PATIENT_H

#include "ct.h"
#include "roi_set.h"



class patient {
public:
    /**
     * Data Members
     */
    ct CT;                             // patient CT stack
    roi_set ROIs;                      // patient ROI set
    std::string path;                  // path to DICOM directory
    std::vector<uint8_t ***> masks;    // set of ROI masks
    int num_rois;                      // number of rois

    /**
     * Constructor
     * @param none
     * @return none
     */
    patient();

    /**
     * Constructor
     * @param string path
     * @param int resize factor
     * @return none
     */
    patient(std::string, int);

};


#endif //DCM2HDF_PATIENT_H
