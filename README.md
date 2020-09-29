# DICOM to HDF5 Converter
#### created by: Hunter Stephens

Converts a DICOM directory into an HDF5 data file with
CT volume and ROI volume masks.

## Pre-installation
dcm2hdf requires a few prerequiste toolkits to be installed
and visible to cmake.

[DCMTK](https://dicom.offis.de/dcmtk.php.en) \
[BOOST](https://www.boost.org/doc/libs/1_66_0/index.html) \
[HDF5](https://support.hdfgroup.org/HDF5/doc/cpplus_RM/index.html) \
[OpenCV](https://opencv.org/) \
[CMake](https://cmake.org/)

* *note: sometimes dcmtk needs to be built from source for the cmake config file to be properly installed*

## Installation
dcm2hdf is built using cmake. 

* *Windows users please consult relevant documentation to install prequiste libraries so that 
cmake can find the libraries. Alternatively, add all libraries and source to VisualStudio and build*

**Linux/Mac users**
```
mkdir build
cmake ..
make
sudo ln -s /path/to/dcm2hdf /usr/local/bin
```

## Usage

```
dcm2hdf [/path/to/dicom] [scale factor]
```

**Example**
```
cd /path/to/dicom
dcm2hdf $PWD 2
```
Result is a *plan.hdf5* file within DICOM directory.

## HDF5 Structure
```
\CT  [CT stack volume]
\x   [x coordinates]
\y   [y coordinates]
\z   [z coordinates]
\ROI_name [3D structore mask]
    ...
```

