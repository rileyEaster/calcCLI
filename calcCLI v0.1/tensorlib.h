#ifndef TENSORLIB_H
#define TENSORLIB_H

#include <cmath>
#include <complex>
#include <cctype>
// #include <string>
// #include <vector>
#include <memory>
#include <stdexcept>
// #include <iostream>
// #include <unordered_map>


// TO LEARN:
// Pointers
// destructor
// *, ** and ? operators

// Changes:
// label attatched functions
// add restructions on T



// ------------------- Vectors -------------------
template <class T>
class ccVector {
    private: 
        int nn;
        T *v; 
    public:
        ccVector();
        explicit ccVector(int n);
        ccVector(const T &a, int n);
        ccVector(const T *a, int n);
        ccVector(const ccVector& rhs);
        ccVector& operator=(const ccVector& rhs);
        ccVector& operator=(const T& a);
        inline T & operator[](const int i);
        inline const T & operator[](const int i) const;
        inline int size() const;
        ~ccVector();
};

template <class T>
ccVector<T>::ccVector() : nn(0), v(0) {}

template <class T>
ccVector<T>::ccVector(int n) : nn(n), v(new T[n]) {}

template <class T>
ccVector<T>::ccVector(const T& a, int n) : nn(n), v(new T[n]) {
    for(int i = 0; i<n; i++) 
        v[i] = *a++;
}

template <class T>
ccVector<T>::ccVector(const ccVector<T> &rhs) : nn(rhs.nn), v(new T[nn]){
    for(int i=0; i<nn; i++)      
        v(i) = rhs[i];
}

template <class T>
ccVector<T> & ccVector<T>::operator=(const ccVector<T> &rhs){
    if(this != &rhs){
        if (nn != rhs.nn){
            if (v != 0) 
                delete [] (v);
            nn=rhs.nn;
            v = new T[nn];
        }
        for (int i=0; i<nn; i++)
            v[i]=rhs[i];
    }
    return *this;
}

// assign a to all enteries
template <class T>
ccVector<T> & ccVector<T>::operator=(const T &a){
    for (int i=0; i<nn; i++)
        v[i] =a;
    return *this;
}

// subscripting
template <class T>
inline T & ccVector<T>::operator[](const int i){
    return v[i];
}

// subscripting
template <class T>
inline const T & ccVector<T>::operator[](const int i) const {
    return v[i];
}

// vector dim
template <class T>
inline int ccVector<T>::size() const {
    return nn;
}

template <class T>
ccVector<T>::~ccVector(){
    if (v != 0)
        delete[] (v);
}

template <class T>
class ccMatrix{
    private:
        int nn;
        int mm;
        T **v;
    public:
        ccMatrix();
        ccMatrix(int n, int m);
        ccMatrix(const T& a, int n, int m);
        ccMatrix(const T *a, int n, int m);
        ccMatrix(const ccMatrix& rhs);
        ccMatrix& operator=(const ccMatrix& rhs);
        ccMatrix& operator=(const T& a);
        inline T* operator[](const int i);
        inline const T* operator[](const int i) const;
        inline int nrows() const;
        inline int ncols() const;
        ~ccMatrix(); 
};

template <class T>
ccMatrix<T>::ccMatrix() : nn(0), mm(0), v(0) {}

template <class T>
ccMatrix<T>::ccMatrix(int n, int m) : nn(0), mm(0), v(new T*[n]){
    v[0] = new T[m*n];
    for (int i=1; i<n; i++)
        v[i] = v[i-1] + m;
}

template <class T>
ccMatrix<T>::ccMatrix(const T &a, int n, int m) : nn(n), mm(m), v(new T*[n]){
    int i,j;
    v[0] = new T[m*n];
    for (i=1; i<n; i++)
        v[i] = v[i-1] + m;
    for (i = 0; i<n; i++)
        for(j=0;j<m;j++)
            v[i][j] =a;
}

template <class T>
ccMatrix<T>::ccMatrix(const T *a, int n, int m) : nn(n), mm(m), v(new T*[n]){
    int i,j;
    v[0] = new T[m*n];
    for (i=1; i<n; i++)
        v[i] = v[i-1] + m;
    for (i = 0; i<n; i++)
        for(j=0;j<m;j++)
            v[i][j] = *a++;
}

template <class T>
ccMatrix<T>::ccMatrix(const ccMatrix &rhs) : nn(rhs.nn), mm(rhs.mm), v(new T*[nn]){
    int i,j;
    v[0] = new T*[nn*mm];
    for (i=1; i<nn; i++)
        v[i] = v[i-1] + mm;
    for (i=0; i<nn; i++)
        for(j=0; j<mm; j++)
            v[i][j] = rhs[i][j]; 
}

template <class T>
ccMatrix<T> & ccMatrix<T>::operator=(const ccMatrix<T> &rhs){
    if (this != &rhs){
        int i,j;
        if (nn != rhs.nn || mm != rhs.mm){
            if (v != 0) {
                delete[] (v[0]);
                delete[] (v);
            }
            nn = rhs.nn;
            mm = rhs.mm;
            v = new T*[nn];
            v[0] = new T*[nn*mm];
        }
        for (i =1; i<nn; i++)
            v[i] = v[i-1] + mm;
        for (i = 0; i < nn; i++)
            for(j = 0; j< mm; j++)
                v[i][j] = rhs[i][j];
    }
    return *this;
}

template <class T>
ccMatrix<T> & ccMatrix<T>::operator=(const T &a){
    for (int i=0; i<nn; i++)
        for(int j=0; j<mm; j++)
            v[i][j] = a;
    return *this; 
}

template <class T>
inline T* ccMatrix<T>::operator[](const int i){
    return v[i];
}

template <class T>
inline const T* ccMatrix<T>::operator[](const int i) const {
    return v[i];
}

template <class T>
inline int ccMatrix<T>::nrows() const {
    return nn;
}

template <class T>
inline int ccMatrix<T>::ncols() const {
    return mm;
}

template <class T>
ccMatrix<T>::~ccMatrix() {
    if (v != 0) {
        delete[] (v[0]);
        delete[] (v);
    }
}

#endif

// int main() {

//     ccVector<double> w(2);
//     w[0] = 3;
//     w[1] = 2;

//     ccMatrix<double> A(2,2);
//     A[0][0] = 1;
//     A[1][0] = 0;
//     A[0][1] = 1;
//     A[1][1] = 0;
//     std::cout << "w = \n    [ " << w[0] << " " << w[1] << " ]";
//     return 0;
// }