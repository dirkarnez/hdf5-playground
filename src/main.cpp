#include <iostream>

#include "hdf5.h"

#define H5FILE_NAME "SDS.h5"
#define DATASETNAME "IntArray"
#define NX 5 /* dataset dimensions */
#define NY 6
#define RANK 2

int main(void)
{
    {
        hid_t file, dataset;       /* file and dataset handles */
        hid_t datatype, dataspace; /* handles */
        hsize_t dimsf[2];          /* dataset dimensions */
        herr_t status;
        int data[NX][NY]; /* data to write */
        int i, j;

        /*
         * Data and output buffer initialization.
         */
        for (j = 0; j < NX; j++)
            for (i = 0; i < NY; i++)
                data[j][i] = i + j;
        /*
         * 0 1 2 3 4 5
         * 1 2 3 4 5 6
         * 2 3 4 5 6 7
         * 3 4 5 6 7 8
         * 4 5 6 7 8 9
         */

        /*
         * Create a new file using H5F_ACC_TRUNC access,
         * default file creation properties, and default file
         * access properties.
         */
        file = H5Fcreate(H5FILE_NAME, H5F_ACC_TRUNC, H5P_DEFAULT, H5P_DEFAULT);

        /*
         * Describe the size of the array and create the data space for fixed
         * size dataset.
         */
        dimsf[0] = NX;
        dimsf[1] = NY;
        dataspace = H5Screate_simple(RANK, dimsf, NULL);

        /*
         * Define datatype for the data in the file.
         * We will store little endian INT numbers.
         */
        datatype = H5Tcopy(H5T_NATIVE_INT);
        status = H5Tset_order(datatype, H5T_ORDER_LE);

        /*
         * Create a new dataset within the file using defined dataspace and
         * datatype and default dataset creation properties.
         */
        dataset = H5Dcreate2(file, DATASETNAME, datatype, dataspace, H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

        /*
         * Write the data to the dataset using default transfer properties.
         */
        status = H5Dwrite(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data);

        /*
         * Close/release resources.
         */
        H5Sclose(dataspace);
        H5Tclose(datatype);
        H5Dclose(dataset);
        H5Fclose(file);
    }

    // read
    {
        hid_t file, dataset; /* handles */
        hid_t datatype, dataspace;
        H5T_class_t t_class; /* data type class */
        H5T_order_t order;   /* data order */
        size_t size;         /*
                              * size of the data element
                              * stored in file
                              */
        hsize_t dims_out[2];    /* dataset dimensions */
        herr_t status;
        int data_out[NX][NY]; /* data to write */
        int i, j, status_n, rank;

        /*
         * Open the file and the dataset.
         */
        file = H5Fopen(H5FILE_NAME, H5F_ACC_RDONLY, H5P_DEFAULT);
        dataset = H5Dopen2(file, DATASETNAME, H5P_DEFAULT);

        /*
         * Get datatype and dataspace handles and then query
         * dataset class, order, size, rank and dimensions.
         */
        datatype = H5Dget_type(dataset); /* datatype handle */
        t_class = H5Tget_class(datatype);
        if (t_class == H5T_INTEGER)
        {
            printf("Data set has INTEGER type \n");
        }

        order = H5Tget_order(datatype);
        if (order == H5T_ORDER_LE)
        {
            printf("Little endian order \n");
        }

        size = H5Tget_size(datatype);
        printf(" Data size is %d \n", (int)size);

        dataspace = H5Dget_space(dataset); /* dataspace handle */
        rank = H5Sget_simple_extent_ndims(dataspace);
        status_n = H5Sget_simple_extent_dims(dataspace, dims_out, NULL);
        printf("rank %d, dimensions %lu x %lu \n", rank, (unsigned long)(dims_out[0]),
               (unsigned long)(dims_out[1]));

        /*
        * Read data from hyperslab in the file into the hyperslab in
        * memory and display.
        */
        status = H5Dread(dataset, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, data_out);
        for (j = 0; j < NX; j++) {
            for (i = 0; i < NY; i++)
                printf("%d ", data_out[j][i]);
            printf("\n");
        }

        H5Tclose(datatype);
        H5Dclose(dataset);
        H5Sclose(dataspace);
        H5Fclose(file);
    }

    std::cin.get();

    return 0;
}