//#pragma once
//
//#include "FrameworkMath/common_defines.h"
//#include "FrameworkMath/MathDefines.h"
//#include <math.h>
//
//namespace FrameworkMath {
//
//    /** \brief Matrix class template.
//      *
//      * This class template implements an algebric matrix. The matrix can be resized at any time (loosing the data content), if square it can be computed
//      * the determinant, transpost matrix and some other useful data.
//      */
//    
//#define INDX(row,col) ((row-1)*m_Columns + col - 1)
//
//    /** \brief NxN template matrix
//     *
//     * This class implements a NxN fixed-size template matrix.
//     */
//    template <typename T>
//    class CMatrix {
//    private:
//        /** \brief Data content.                                                                                                                        */
//        T *data;
//        /** \brief Number of matrix rows.                                                                                                               */
//        unsigned int m_Rows;
//        /** \brief Number of matrix columns.                                                                                                            */
//        unsigned int m_Columns;
//    
//    public:
//        /*
//                                                        _               _
//                                         __ ___ _ _  __| |_ _ _ _  _ __| |_ ___ _ _ ___
//                                        / _/ _ \ ' \(_-<  _| '_| || / _|  _/ _ \ '_(_-<
//                                        \__\___/_||_/__/\__|_|  \_,_\__|\__\___/_| /__/
//        */
//        /** \brief Default constructor.
//         *
//         * This constructor creates a 4x4 identity matrix.
//         */
//        CMatrix() : m_Rows(0), 
//                    m_Columns(0), 
//                    data(NULL) {
//            // allocate data
//            CMatrix(1);
//            }
//
//        /** \brief Default constructor.
//         *
//         * Matrix is initialized to identity matrix of given size.
//         */
//        explicit CMatrix(unsigned int square_size) : m_Rows(0), 
//                                                     m_Columns(0), 
//                                                     data(NULL) {
//            *this = CMatrix(square_size, square_size);
//            }
//
//        /** \brief Default constructor.
//         *
//         * Matrix is initialized to identity matrix of given size.
//         */
//        explicit CMatrix(unsigned int rows, unsigned int columns) : m_Rows(0), 
//                                                                    m_Columns(0), 
//                                                                    data(NULL) {
//            // store the matrix dimension
//            m_Rows    = rows;
//            m_Columns = columns;
//            data = new T[rows*columns];
//            for (unsigned int i=1; i<=rows; i++) {
//                for (unsigned int j=1; j<=columns; j++)
//                    At(i,j) = ( i==j ? (T)1.0 : (T)0.0 );
//                }
//            }
//
//        /** \brief Default destructor.                                                                                                                  */
//        ~CMatrix() {
//            // deallcate data
//            if (data != NULL) delete[] data;
//            }
//
//        /*
//                                    _                                                      _
//                                 __(_)______   _ __  __ _ _ _  __ _ __ _ ___ _ __  ___ _ _| |_
//                                (_-< |_ / -_) | '  \/ _` | ' \/ _` / _` / -_) '  \/ -_) ' \  _|
//                                /__/_/__\___|_|_|_|_\__,_|_||_\__,_\__, \___|_|_|_\___|_||_\__|
//                                           |___|                   |___/
//        */
//        /** \brief Get the actual size of the matrix.                                                                                                   */
//        __inline unsigned int GetNRows() {return m_Rows;}
//
//        /** \brief Get the actual size of the matrix.                                                                                                   */
//        __inline unsigned int GetNColumns() {return m_Columns;}
//
//        /** \brief Resize the matrix.
//         * 
//         * As resize will allocate new data, the data content will be reinitialized to identity matrix.
//         */
//        __inline void Resize(unsigned int rows, unsigned int columns) {*this = CMatrix(rows, columns);}
//
//        /** \brief Resize the matrix.
//         * 
//         * As resize will allocate new data, the data content will be reinitialized to identity matrix.
//         */
//        __inline void Resize(unsigned int square_size) {*this = CMatrix(square_size, square_size);}
//
//        /** \brief Get an element at position [row, col]                                                                                                */
//        __inline T& At(unsigned int row, unsigned int col) { return data[INDX(row,col)]; }
//
//
//        /*
//                                    _       _                       _           _      _   _
//                         _ __  __ _| |_ _ _(_)_ __  _ __  __ _ _ _ (_)_ __ _  _| |__ _| |_(_)___ _ _
//                        | '  \/ _` |  _| '_| \ \ / | '  \/ _` | ' \| | '_ \ || | / _` |  _| / _ \ ' \
//                        |_|_|_\__,_|\__|_| |_/_\_\_|_|_|_\__,_|_||_|_| .__/\_,_|_\__,_|\__|_\___/_||_|
//                                                |___|                |_|
//
//        */
//        /** \brief Extract a row.                                                                                                                       */
//        __inline CMatrix<T> GetRow(unsigned int irow) {
//            CMatrix<T> res(1,m_Columns);
//            for (unsigned int i=1; i<=m_Columns; i++)
//                res.At(1, i) = At(irow, i);
//            return res;
//            }
//
//        /** \brief Extract a column.                                                                                                                    */
//        __inline CMatrix<T> GetColumn(unsigned int icolumn) {
//            CMatrix<T> res(m_Rows,1);
//            for (unsigned int i=1; i<=m_Rows; i++)
//                res.At(i, 1) = At(i, icolumn);
//            return res;
//            }
//
//        /** \brief Swap two columns.                                                                                                                    */
//        __inline void SwapColumns(unsigned int first, unsigned int second) {
//            T aux;
//            for (unsigned int i=1; i<=m_Rows; i++) {
//                aux = At(i,first);
//                At(i, first) = At(i,second);
//                At(i,second) = aux;
//                }
//            }
//
//        /** \brief Swap two columns.                                                                                                                    */
//        __inline void SwapRows(unsigned int first, unsigned int second) {
//            T aux;
//            for (unsigned int i=1; i<=m_Columns; i++) {
//                aux = At(first, i);
//                At(first, i) = At(second, i);
//                At(second, i) = aux;
//                }
//            }
//
//        /** \brief Extract a submatrix from the actual matrix.                                                                                          */
//        __inline CMatrix<T> SubMatrix(unsigned int row_from, unsigned int col_from, unsigned int rows, unsigned int cols) {
//            // size check
//            row_from = min(row_from, m_Rows);
//            col_from = min(col_from, m_Columns);
//            rows = min(rows, m_Rows    - row_from + 1);
//            cols = min(cols, m_Columns - col_from + 1);
//            // create the matrix and fill data
//            CMatrix<T> res(rows, cols);
//            for (unsigned int i=0; i<rows; i++)
//                for (unsigned int j=0; j<cols; i++)
//                    res.At(i+1,j+1) = At(row_from + i, col_from + j);
//            return res;
//            }
//        /*
//                                                         _         _
//                                                 __ __ _| |__ _  _| |_  _ ___
//                                                / _/ _` | / _| || | | || (_-<
//                                                \__\__,_|_\__|\_,_|_|\_,_/__/
//        */
//        /** \brief Compute the determinant of the matrix.
//         *
//         * Since determinant of a matrix can be retrived only for square matrix a control on the size is done and if the matrix is not
//         * square then a NAN value is returned.
//         */
//        T Determinant() {
//            // Gauss method
//            T res = 1.0;
//            if (m_Rows != m_Columns)
//                return NAN;
//            for (unsigned int ipivot = 1; ipivot <= m_Rows; ipivot++) {
//                // select non-zero pivot row
//                bool            swap = false;
//                unsigned int    iswap;
//                for (unsigned int isel = ipivot; isel <= m_Rows; isel++)
//                    if (! ISZERO(At(isel ,ipivot)) ) {
//                        // found non-null element in the isel row, ipivot column
//                        iswap = isel;
//                        swap = true;
//                        break;
//                        }
//                // check if non zero pivot are founded
//                if (!swap) {
//                    // no valid pivot elements, procedure failed
//                    return NAN;
//                    }
//
//                // switch the rows
//                SwapRows(ipivot, iswap);
//
//                // nullify the elements under the pivot
//                for (unsigned int irow = ipivot+1; irow <= m_Rows; irow++) {
//                    T factor = -At(irow, ipivot)/At(ipivot, ipivot);
//                    for (unsigned int j = 1; j < m_Rows; j++)
//                        At(irow, j) += At(ipivot, j)*factor;
//                    }
//                }
//
//            // At the end a diagonal matrix has to be created, determinant is the product of the diagonal elements  
//            for (unsigned int idiag = 1; idiag <= m_Rows; idiag++)
//                res = res * At(idiag,idiag);
//
//            return res;
//            }
//
//        /** \brief Extract a minor from the matrix excluding the given row and column.                                                                  */
//        __inline void Minor(unsigned int irow, unsigned int icol, CMatrix<T> & res) {
//            res = CMatrix(m_Rows-1, m_Columns-1);
//            unsigned int count = -1;
//            for (unsigned int i=1; i <= m_Rows; i++) {
//                for (unsigned int j = 1; j <= m_Columns; j++)
//                    // assign element if it is not in the given row or column
//                    if (i != irow && j != icol) {
//                        count++;
//                        res.At(1 + (count / (m_Columns-1)), 1 + (count % (m_Rows-1))) = At(i,j);
//                        }
//                }
//            return;
//            }
//
//        /** \brief LU decomposition.                                                                                                                    */
//        bool LU(CMatrix<T> &L, CMatrix<T> &U) {
//            // matrix must be square
//            if (m_Rows != m_Columns)
//                return false;
//            // determinant too close to zero
//            if (fabs(Determinant()) < 1e-15)
//                return false;
//            // can proceed
//            CMatrix<T> A(m_Rows);
//            A = *this;
//            L.Resize(m_Rows);
//            U.Resize(m_Rows);
//            for (unsigned int ipivot = 1; ipivot <= m_Rows; ipivot++) {
//                // update the A matrix
//                A = A*L;
//                // begin of the step: element on the diagonal must be different from zero, otherwise swap with the first valid row with non zero element
//                if ( ISZERO(A.At(ipivot,ipivot)) ) {
//                    // ok, element on the diagonal is zero, look the for first non zero element under it
//                    unsigned int iswap = ipivot;
//                    bool swap = false;
//                    for (;iswap <= m_Rows; iswap++) {
//                        if ( !ISZERO(A.At(iswap, ipivot)) ) {
//                            swap = true;
//                            break;
//                            }
//                        }
//                    if (swap) {
//                        // non zero element found, swap rows
//                        SwapRows(ipivot, iswap);
//                        }
//                    else {
//                        // method can't go on because all elements under the actual element on diagonal are zero
//                        return false;
//                        }
//                    }
//                // get the element on the diagonal
//                T diag_elem = At(ipivot,ipivot);
//                //cycle through the elemts under the diagonal element at istep position
//                for (unsigned int j = ipivot+1; j<=m_Rows; j++) {
//                    // Add the pivot row to the actual using the right coefficient
//                    T coeff = -At(j, ipivot) / diag_elem;
//                    for (unsigned int i=1; i<=m_Columns; i++)
//                        At(j, i) += At(ipivot, i)*coeff;
//                    // fill L matrix
//                    L.At(j, ipivot) = coeff;
//                    }
//                }
//            // At the end
//            return true;
//            }
//        /** \brief Returns a new matrix as the inverse.                                                                                                 */
//        __inline CMatrix<T> Inverse() {
//            CMatrix<T> res;
//            }
//
//        /*
//                                                  _ _             _   _
//                                         __ _ _ _(_) |_ _ __  ___| |_(_)__
//                                        / _` | '_| |  _| '  \/ -_)  _| / _|
//                                        \__,_|_| |_|\__|_|_|_\___|\__|_\__|
//        */
//        /** \brief Assignment operator.
//         *
//         * Matrix assignment makes the new matrix to be the same size of the rhs matrix and will contain the same data.
//         */
//        void operator=(CMatrix<T> &assign) {
//            // allocate new space if matrix shape is different
//            if ( m_Rows != assign.GetNRows() || m_Columns != assign.GetNColumns() ) {
//                if (data != NULL)
//                    delete [] data;
//                m_Rows    = assign.GetNRows();
//                m_Columns = assign.GetNColumns();
//                data = new T[m_Rows*m_Columns];
//                }
//            for (unsigned int i=1; i<=m_Rows; i++) {
//                for (unsigned int j=1; j<=m_Columns; j++)
//                    At(i,j) = (T)assign.At(i,j);
//                }
//            return;
//            }
//
//        CMatrix<T> operator*(CMatrix<T> &mult) {
//            CMatrix<T> res;
//            // check dimension
//            if (m_Columns != mult.GetNRows()) {
//                res.Resize(1);
//                return res;
//                }
//            // execute the matrix operation
//            res.Resize(m_Rows, mult.GetNColumns());
//            for (unsigned int i=1; i<=m_Rows; i++) {
//                for (unsigned int j=1; j<=mult.GetNColumns(); j++) {
//                    for (unsigned int k=1; k<=m_Columns; k++)
//                        res.At(i,j) = At(i,k) * mult.At(k,j);
//                    }
//                }
//            // done
//            return res;
//            }
//
//        };
//
//    }