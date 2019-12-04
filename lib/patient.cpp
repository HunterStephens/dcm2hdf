/**
 * Patient Class Implementation
 * @author Hunter Stephens
 * @version 0.0.1
*/

#include <opencv2/core/types.hpp>
#include <opencv2/imgproc.hpp>
#include "patient.h"
#include <fstream>


patient::patient(){
    // nothing
}

patient::patient(std::string pth, int fac){
    path = pth;
    /*
     * Read in CT
     */
    ct CT_read(pth);
    CT_read.resize(fac);
    CT = CT_read;

    /*
     * Read in ROIs
     */
    roi_set rois(path);
    ROIs = rois;
    num_rois = ROIs.rois.size();

    /*
     * Create Masks
     */
    masks.resize(num_rois);
    #pragma omp parallel for
    for(int n=0; n<num_rois; n++){
        // --- Initalize mask tensor ---
        auto ***mask3d = new uint8_t **[CT.N_rows];
        for(int i=0; i < CT.N_rows; i++){
            mask3d[i] = new uint8_t *[CT.N_cols];
            for(int j=0; j < CT.N_cols; j++){
                mask3d[i][j] = new uint8_t [CT.N_slices];
                for(int k=0; k < CT.N_slices; k++){
                    mask3d[i][j][k] = 0;
                }
            }
        }
        // --- Calc Mask at slices ---
        roi ROI = ROIs.rois[n];
        for(int i=0; i < ROI.contours.size(); i++) {
            double *contour = ROI.contours[i];
            int num = ROI.num_points[i] / 3;

            // --- reshape contour sequence
            int it = 0;
            double **points = new double*[num];
            std::vector<cv::Point> pnts(num);
            for(int row=0; row<num; row++){
                points[row] = new double[3];
                for(int col=0; col < 3; col++){
                    points[row][col] = contour[it];
                    it++;
                }
                cv::Point pt(points[row][0],points[row][1]);
                pnts[row] = pt;
            }
            // --- find slice id ---
            double z_slice = points[0][2];
            double dz = abs(CT.z[1] - CT.z[0]);
            int slice = (int)(CT.z[0]-z_slice)/dz;
            if(slice < CT.N_slices){
                // --- create 2d slice mask
                for(int row = 0; row < CT.N_rows; row++){
                    for(int col = 0; col < CT.N_cols; col++) {
                        // get point
                        cv::Point temp;
                        temp.x = CT.x[col];
                        temp.y = CT.y[row];
                        double in_out = cv::pointPolygonTest(pnts, temp, false);
                        if (in_out > 0) {
                            mask3d[row][col][slice] = 1;
                        }
                    }
                }
            }

        }
        masks[n] = mask3d;
    }
}
