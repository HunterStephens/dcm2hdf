#include "H5Cpp.h"
#include <string>
#include "patient.h"

using namespace H5;

int main(int argc, char** argv) {

    std::string path(argv[1]);
    int fac = atoi(argv[2]);
    patient pat(path, fac);

    double *ct_data = new double[pat.CT.N_rows*pat.CT.N_cols*pat.CT.N_slices];
    std::vector<double *> roi_data(pat.num_rois);
    std::vector<std::string> roi_names(pat.num_rois);
    for(int n=0; n<pat.num_rois; n++){
        roi_data[n] = new double[pat.CT.N_rows*pat.CT.N_cols*pat.CT.N_slices];
        for(int i=0; i < pat.CT.N_rows; i++){
            for(int j=0; j < pat.CT.N_cols; j++){
                for(int k=0; k < pat.CT.N_slices; k++)
                {
                    // \[offset=n_3+N_3*(n_2+N_2*n_1)\]
                    roi_data[n][k+pat.CT.N_slices*(j+pat.CT.N_cols*i)] = (double)pat.masks[n][i][j][k];
                }
            }
        }
        roi_names[n] = pat.ROIs.rois[n].name;
    }


    for(int i=0; i < pat.CT.N_rows; i++){
        for(int j=0; j < pat.CT.N_cols; j++){
            for(int k=0; k < pat.CT.N_slices; k++)
            {
                // \[offset=n_3+N_3*(n_2+N_2*n_1)\]
                ct_data[k+pat.CT.N_slices*(j+pat.CT.N_cols*i)] = pat.CT.data[i][j][k];
            }
        }
    }


    std::string hf_name = path + "/plan.hdf5";
    const H5std_string FILE_NAME(hf_name);
    const H5std_string DATASET_NAME("CT");
    const H5std_string DATASET_NAME_X("x");
    const H5std_string DATASET_NAME_Y("y");
    const H5std_string DATASET_NAME_Z("z");


    H5File file(FILE_NAME, H5F_ACC_TRUNC);

    // create data space
    hsize_t dimsf[3] = {pat.CT.N_rows, pat.CT.N_cols, pat.CT.N_slices};
    hsize_t dims_x(pat.CT.N_cols);
    hsize_t dims_y(pat.CT.N_rows);
    hsize_t dims_z(pat.CT.N_slices);
    DataSpace dataspace(3, dimsf);
    DataSpace ds_x(1, &dims_x);
    DataSpace ds_y(1, &dims_y);
    DataSpace ds_z(1, &dims_z);


    // Create CT dataset
    DataSet dataset = file.createDataSet(DATASET_NAME, PredType::NATIVE_DOUBLE,
                                         dataspace);
    DataSet datasetx = file.createDataSet(DATASET_NAME_X, PredType::NATIVE_DOUBLE,
                                         ds_x);
    DataSet datasety = file.createDataSet(DATASET_NAME_Y, PredType::NATIVE_DOUBLE,
                                          ds_y);
    DataSet datasetz = file.createDataSet(DATASET_NAME_Z, PredType::NATIVE_DOUBLE,
                                          ds_z);

    // Attempt to write CT data to HDF5 file
    dataset.write(ct_data, PredType::NATIVE_DOUBLE);
    datasetx.write(pat.CT.x, PredType::NATIVE_DOUBLE);
    datasety.write(pat.CT.y, PredType::NATIVE_DOUBLE);
    datasetz.write(pat.CT.z, PredType::NATIVE_DOUBLE);

    // create and write ROI data
    for(int n=0; n < pat.num_rois; n++){
        DataSpace dspace(3, dimsf);

        //create ROI dataset
        H5std_string name(pat.ROIs.rois[n].name);
        DataSet dset = file.createDataSet(name, PredType::NATIVE_DOUBLE, dspace);
        dset.write(roi_data[n], PredType::NATIVE_DOUBLE);

    }

    // write ROI names
    hsize_t numStrings(pat.num_rois);
    char **stringListCstr = new char *[numStrings];
    int i=0;
    for(std::vector<std::string>::iterator it = roi_names.begin(); it != roi_names.end(); it++)
    {
        stringListCstr[i] = new char[it->size()+1];
        strcpy(stringListCstr[i],it->c_str());
        i++;
    }

    DataSpace strSpace(1, &numStrings);
    StrType strType(PredType::C_S1, H5T_VARIABLE);
    DataSet strSet = file.createDataSet("ROIs",strType, strSpace);
    strSet.write(stringListCstr, strType);

    for(unsigned it = 0; i < numStrings; i++) {
        delete[] stringListCstr[i];
    }
    delete [] stringListCstr;
    
    return 0;
}