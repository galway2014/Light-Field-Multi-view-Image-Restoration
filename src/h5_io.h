#ifndef _HDF5
#define _HDF5

#include <hdf5.h>
#include <hdf5_hl.h>
#include <opencv2/opencv.hpp>


using namespace std;
using namespace cv;

//void  hdf5_writer_2d(const char* file_name, float* hdf5_array,size_t* dim);
//void  hdf5_writer_2d_vec(const char* file_name, vector<float> hdf5_array, size_t* dim);
//void  hdf5_writer_2d_pt (const char* file_name, vector<Point2d> hdf5_array);
//void  hdf5_read_2d_pt (const char* file_name, vector<Point2d>& hdf5_array);
//void  hdf5_read_2d  (const char* file_name, float* hdf5_array);
//void  hdf52mat      (const char* file_name, Mat& img);
//void  mat2hdf5      (const char* file_name, Mat& img);

#define DATASETNAME "POS"

void hdf5_writer_2d(const char* file_name, float* hdf5_array, size_t* dim){

    hid_t       file, dataset,dataset_config; /* file and dataset handles */
    hid_t       datatype, dataspace;   /* handles */
    hsize_t     dimsf[2];              /* dataset dimensions */
    //herr_t      status;

    /*
     * Create a new file using H5F_ACC_TRUNC access,
     * default file creation properties, and default file
     * access properties.
     */
    file = H5Fcreate(file_name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Describe the size of the array and create the data space for fixed
     * size dataset.
     */
    dimsf[0] = dim[0];
    dimsf[1] = dim[1];

    dataspace = H5Screate_simple(2, dimsf, NULL);

    /*
     * Define datatype for the data in the file.
     * We will store little endian INT numbers.
     */
    datatype = H5Tcopy(H5T_NATIVE_FLOAT);
    H5Tset_order(datatype, H5T_ORDER_LE);
    dataset_config = H5Pcreate(H5P_DATASET_CREATE);
    dataset = H5Dcreate2(file, DATASETNAME, datatype, dataspace,
              H5P_DEFAULT, dataset_config, H5P_DEFAULT);
    /*
   
     * Write the data to the dataset using default transfer properties.
     */
    H5Dwrite(dataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, hdf5_array);

    H5LTset_attribute_int( file, DATASETNAME, "Pos_y", (int*)&dim[0], 1);
    H5LTset_attribute_int( file, DATASETNAME, "Pos_x", (int*)&dim[1], 1);

    /*
     * Close/release resources.
     */
    H5Sclose(dataspace);
    H5Tclose(datatype);
    H5Dclose(dataset);
    H5Fclose(file);
}

void  hdf5_writer_2d_pt (const char* file_name, vector<Point2d> hdf5_array){

    hid_t       file, dataset,dataset_config; /* file and dataset handles */
    hid_t       datatype, dataspace;   /* handles */
    hsize_t     dimsf[2];              /* dataset dimensions */
    file = H5Fcreate(file_name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);
    dimsf[0] = hdf5_array.size();
    dimsf[1] = 2;
    dataspace = H5Screate_simple(2, dimsf, NULL);
    datatype = H5Tcopy(H5T_NATIVE_FLOAT);
    H5Tset_order(datatype, H5T_ORDER_LE);
    dataset_config = H5Pcreate(H5P_DATASET_CREATE);
    dataset = H5Dcreate2(file, DATASETNAME, datatype, dataspace,
              H5P_DEFAULT, dataset_config, H5P_DEFAULT);
    float* temp=new float[hdf5_array.size()*2];
    for (size_t i=0; i<hdf5_array.size(); i++){
        temp[2*i]  =hdf5_array[i].y;
        temp[2*i+1]=hdf5_array[i].x;
    }

    H5Dwrite(dataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, temp);

    H5LTset_attribute_int( file, DATASETNAME, "Pos_y", (int*)&dimsf[0], 1);
    H5LTset_attribute_int( file, DATASETNAME, "Pos_x", (int*)&dimsf[1], 1);

    /*
     * Close/release resources.
     */
    H5Sclose(dataspace);
    H5Tclose(datatype);
    H5Dclose(dataset);
    H5Fclose(file);
}

void hdf5_read_2d_pt(const char* file_name, vector<Point2d>& hdf5_array){

   hid_t       file_id, dataset_id;  /* identifiers */
   /* Open an existing file. */
   file_id = H5Fopen(file_name, H5F_ACC_RDWR, H5P_DEFAULT);

   /* Open an existing dataset. */
   dataset_id = H5Dopen(file_id, "/POS",H5P_DEFAULT);

   /* Read the dataset. */
   float* temp=new float[hdf5_array.size()*2];
   H5Dread(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                    temp);
    for (size_t i=0; i<hdf5_array.size(); i++){
		hdf5_array[i].y= temp[2*i];
		hdf5_array[i].x=temp[2*i+1];
    }
   /* Close the dataset. */
   H5Dclose(dataset_id);

   /* Close the file. */
   H5Fclose(file_id);
}



void hdf5_writer_2d_vec(const char* file_name, vector<float> hdf5_array, size_t* dim){

    hid_t       file, dataset,dataset_config; /* file and dataset handles */
    hid_t       datatype, dataspace;   /* handles */
    hsize_t     dimsf[2];              /* dataset dimensions */
    //herr_t      status;

    /*
     * Create a new file using H5F_ACC_TRUNC access,
     * default file creation properties, and default file
     * access properties.
     */
    file = H5Fcreate(file_name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Describe the size of the array and create the data space for fixed
     * size dataset.
     */
    dimsf[0] = dim[0];
    dimsf[1] = dim[1];

    dataspace = H5Screate_simple(2, dimsf, NULL);

    /*
     * Define datatype for the data in the file.
     * We will store little endian INT numbers.
     */
    datatype = H5Tcopy(H5T_NATIVE_FLOAT);
    H5Tset_order(datatype, H5T_ORDER_LE);
    dataset_config = H5Pcreate(H5P_DATASET_CREATE);
    dataset = H5Dcreate2(file, DATASETNAME, datatype, dataspace,
              H5P_DEFAULT, dataset_config, H5P_DEFAULT);
    /*
   
     * Write the data to the dataset using default transfer properties.
     */
    float* temp=new float[hdf5_array.size()];
    for (size_t i=0; i<hdf5_array.size(); i++){
        temp[i]=hdf5_array[i];
    }

    H5Dwrite(dataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, temp);

    H5LTset_attribute_int( file, DATASETNAME, "Pos_y", (int*)&dim[0], 1);
    H5LTset_attribute_int( file, DATASETNAME, "Pos_x", (int*)&dim[1], 1);

    /*
     * Close/release resources.
     */
    H5Sclose(dataspace);
    H5Tclose(datatype);
    H5Dclose(dataset);
    H5Fclose(file);
}



void hdf5_read_2d(const char* file_name, float* hdf5_array){

   hid_t       file_id, dataset_id;  /* identifiers */
   /* Open an existing file. */
   file_id = H5Fopen(file_name, H5F_ACC_RDWR, H5P_DEFAULT);

   /* Open an existing dataset. */
   dataset_id = H5Dopen(file_id, "/POS",H5P_DEFAULT);

   /* Read the dataset. */
   H5Dread(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                    hdf5_array);

   /* Close the dataset. */
   H5Dclose(dataset_id);

   /* Close the file. */
   H5Fclose(file_id);
}


void  hdf52mat      (const char* file_name, Mat& img){


   hid_t       file_id, dataset_id;  /* identifiers */
   /* Open an existing file. */
   file_id = H5Fopen(file_name, H5F_ACC_RDWR, H5P_DEFAULT);

   /* Open an existing dataset. */
   dataset_id = H5Dopen(file_id, "/POS",H5P_DEFAULT);

   /* Read the dataset. */
   float* data_vec = new float[img.cols*img.rows];
   H5Dread(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                    data_vec);
   //convert Mat
   for (int j = 0; j <img.rows; j++)
	    for (int i = 0; i < img.cols; i++)
            img.at<float>(j,i)=data_vec[j*img.cols+i];

   delete[] data_vec;
   /* Close the dataset. */
   H5Dclose(dataset_id);

   /* Close the file. */
   H5Fclose(file_id);
}

void  mat2hdf5      (const char* file_name, Mat& img){


    hid_t       file, dataset,dataset_config; /* file and dataset handles */
    hid_t       datatype, dataspace;   /* handles */
    hsize_t     dimsf[2];              /* dataset dimensions */
    /*
     * Create a new file using H5F_ACC_TRUNC access,
     * default file creation properties, and default file
     * access properties.
     */
    file = H5Fcreate(file_name, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

    /*
     * Describe the size of the array and create the data space for fixed
     * size dataset.
     */
    dimsf[0] = img.rows;//may invert
    dimsf[1] = img.cols;

    dataspace = H5Screate_simple(2, dimsf, NULL);

    /*
     * Define datatype for the data in the file.
     * We will store little endian INT numbers.
     */
    datatype = H5Tcopy(H5T_NATIVE_FLOAT);
    H5Tset_order(datatype, H5T_ORDER_LE);
    dataset_config = H5Pcreate(H5P_DATASET_CREATE);
    dataset = H5Dcreate2(file, DATASETNAME, datatype, dataspace,
              H5P_DEFAULT, dataset_config, H5P_DEFAULT);
    /*
   
     * Write the data to the dataset using default transfer properties.
     */
    float* temp=new float[img.rows*img.cols];
   for (int j = 0; j <img.rows; j++)
	    for (int i = 0; i < img.cols; i++){
            temp[j*img.cols+i]=img.at<float>(j,i);

        }

    H5Dwrite(dataset, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, temp);

    H5LTset_attribute_int( file, DATASETNAME, "Pos_y", (int*)&dimsf[0], 1);
    H5LTset_attribute_int( file, DATASETNAME, "Pos_x", (int*)&dimsf[1], 1);

    /*
     * Close/release resources.
     */
    H5Sclose(dataspace);
    H5Tclose(datatype);
    H5Dclose(dataset);
    H5Fclose(file);
}



#endif

