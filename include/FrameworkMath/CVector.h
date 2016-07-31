#pragma once

#include "FrameworkMath/common_defines.h"

namespace FrameworkMath {
/
    /** \brief Algebric vector class template.
      *
      * This class template implements a vector object. It allows to compute the fundamental operations and 
      */
    //template<typename T>
    //class CMutableMatrix {
    //private:
    //    /** \brief Number of reserved cells.                                                                                                            */
    //    unsigned int m_ReservedCells;
    //    /** \brief Matrix number of rows.                                                                                                               */
    //    unsigned int m_Rows;
    //    /** \brief Matrix number of columns.                                                                                                            */
    //    unsigned int m_Cols;

    //    /** \brief Pointer to data content.                                                                                                             */
    //    T* m_Data;

    //public:
    //    /** \brief Default construtcor.
    //     *
    //     * Default constructor creates an empty matrix. 
    //     */
    //    CMatrix();

    //    /** \brief Constructor of a matrix with given size.                                                                                             */
    //    explicit CMatrix(unsigned int size_X, unsigned int size_Y);

    //    /** \brief Default destructor.                                                                                                                  */
    //    ~CMatrix();

    //    /** \brief Allocate the assigned space for the matrix. Elements are initialized to zero.                                                        */
    //    bool Reserve(unsigned int size_x, unsigned size_y);

    //    /** \brief Autosize management flag.
    //     *
    //     * This flag enables the automatic management of matrix dimension i.e. if an object is assigned but its position is out of range then the matrix
    //     * is expanded of the necessary number of row/columns in order to make this element included. All new elements created by the expansion are 
    //     * initialized to zero.
    //     */
    //    void AutoReserve(bool flag); 

    //    /** \brief Compute the determinant of the matrix.                                                                                               */
    //    double Determinant();

    //    /** \brief Compute the rank of the matrix.                                                                                                      */
    //    unsigned int Rank();
    //    };

    /** \brief NxN template matrix
     *
     * This class implements a NxN fixed-size template matrix.
     */
    template <typename T>
    class framework_math_class CSquareMatrix {
    private:
        /** \brief Data content.                                                                                                                        */
        T *data;
        /** \brief Size of the matrix.                                                                                                                  */
        unsigned int size;
    
    public:
        /** \brief Default constructor.
         *
         * This constructor creates a 4x4 identity matrix.
         */
        CSquareMatrix():  size(4)
                         ,data(NULL) {
            // allocate data
            data = new T[size][size];
            for (unsigned int i=0; i<size; i++) {
                for (unsigned int j=0; j<size; j++)
                    data[i][j] = ( i=j ? 1 : 0 );
                }
            }

        /** \brief Default constructor.
         *
         * Matrix is initialized to identity matrix of given size.
         */
        explicit CSquareMatrix(unsigned int size) {
            *this = CSquareMatrix<T>(size);
            }

        /** \brief Default destructor.                                                                                                                  */
        ~CSquareMatrix() {
                // deallcate data
            delete[] data;
            }

        /** \brief Get the actual size of the matrix.                                                                                                   */
        __inline unsigned int GetSize() {return size;}

        /** \brief Resize the matrix.
         * 
         * As resize will allocate new data, the data content will be reinitialized to identity matrix.
         */
        __inline void Resize(unsigned int new_size) {*this = CMatrix<T>(new_size);}

        /** \brief Compute the determinant of the matrix.                                                                                               */
        __inline double Determinant() {
            double res = 0.0;
            // standard method along the first row
            for (unsigned int i=0; i < size; i++)
                res += data[0][i] * Minor(0,i).Determinant();
            return res;
            }

        /** \brief Extract a minor from the matrix excluding the given row and column.                                                                  */
        __inline CSquareMatrix<T> Minor(unsigned int irow, unsigned int icol) {
            CSquareMatrix<T> res = CSquareMatrix(size-1);
            unsigned int count = -1;
            for (unsigned int i=0; i < size; i++)
                for (unsigned int j = 0; j < size; j++)
                    // assign element if it is not in the given row or column
                    if (i != irow && j != icol) {
                        count++;
                        res[count / (size-1)][count % (size-1)] = data[i][j];
                        }
            // return the matrix
            return res;
            }


        /** \brief Assignment operator.
         *
         * Matrix assignment makes the new matrix to be the same size of the rhs matrix and will contain the same data.
         */
        __inline CSquareMatrix& operator=(CSquareMatrix<T> &assign) {
            delete [] data;
            size = assign.GetSize();
            data = new T[size*size];
            for (unsigned int i=0; i<size; i++)
                for (unsigned int j=0; j<size; j++)
                    data[i][j] = assign[i][j];
            }

        };

    }