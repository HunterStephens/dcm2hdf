/**
 * ROI Set Class Implementation
 * @author Hunter Stephens
 * @version 0.0.1
*/

#include "roi_set.h"
#include <omp.h>

using namespace boost::filesystem;
using namespace std;


roi_set::roi_set(){
    num_rois = 0;
}

roi_set::roi_set(string pth) {

    /*
     * loop through directory looking for DICOM CT files
     */
    path p = pth;
    directory_iterator it{p};
    string plan_file;
    while (it != directory_iterator{}) {
        std::string str = (*it).path().string();
        std::istringstream ss(str);
        std::string file_name;
        while(std::getline(ss, file_name, '/')){}

        std::istringstream name(file_name);
        std::string type;
        std::getline(name, type, '.');
        if(type == "RS"){
            plan_file = str;
            break;
        }
        it++;
    }


    /*
     * Get ROIs
     */
    DcmFileFormat st_file;
    OFCondition status = st_file.loadFile(plan_file.c_str());
    DcmDataset *set = st_file.getDataset();
    DcmItem *roiitem;
    DcmItem *contouritem;


    unsigned long i = 0;
    unsigned long ii = 0;
    unsigned long j;
    unsigned long k;
    unsigned long num_rois;
    /*
     * Get ROI names and ID tag
     */
    while(set->findAndGetSequenceItem(DCM_StructureSetROISequence,roiitem,ii).good()){
        OFString name;
        roiitem->findAndGetOFString(DCM_ROIName, name);
        roi temp;
        temp.name = name.c_str();
        temp.ID = ii;
        rois.push_back(temp);
        ii++;
    }
    num_rois = ii;


    /*
     * Get contour data
     */
    while(set->findAndGetSequenceItem(DCM_ROIContourSequence,roiitem,i).good()){
        j = 0;
        while(roiitem->findAndGetSequenceItem(DCM_ContourSequence, contouritem,j).good()){
            OFString dec_string;
            contouritem->findAndGetOFStringArray(DCM_ContourData,dec_string).good();
            vector<string> tokens;
            boost::split(tokens, dec_string, boost::is_any_of("\\"));
            double* cont = new double[tokens.size()];
            #pragma omp parallel for
            for(int i=0; i<tokens.size(); i++){
                cont[i] = atof(tokens[i].c_str());
            }
            rois[i].contours.push_back(cont);
            rois[i].num_points.push_back(tokens.size());
            j++;
        }
        i++;
    }
}
