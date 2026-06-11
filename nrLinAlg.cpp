#include <cmath>
#include <complex>
#include <cctype>
#include <string>
#include <vector>
#include <memory>
#include <stdexcept>
#include <iostream>
#include <unordered_map>

// TO LEARN:
// Pointers
// destructor
// *, ** and ? operators

// Changes:
// 


// ------------------- Vectors -------------------
template <class T>
class NRVec {
    private: 
        int n_vec;
        T *v; 
    public:
        NRVec();
        explicit NRVec(int n);
        NRVec(const T &a, int n);
        NRVec(const T *a, int n);
        NRVec(const NRVec& rhs);
        NRVec& operator=(const NRVec& rhs);
        NRVec& operator=(const T& a);
        inline T & operator[](const int i);
        inline const T & operator[](const int i) const;
        inline int size() const;
        ~NRVec();
};

template <class T>
NRVec<T>::NRVec() : n_vec(0), v(0) {}

template <class T>
NRVec<T>::NRVec(int n) : n_vec(n), v(new T[n]) {}

template <class T>
NRVec<T>::NRVec(const T& a, int n) : n_vec(n), v(new T[n]) {
    for(int i = 0; i<n; i++) 
        v[i] = *a++;
}

template <class T>
NRVec<T>::NRVec(const NRVec<T> &rhs) : n_vec(rhs.n_vec), v(new T[n_vec]){
    for(int i=0; i<n_vec; i++)      
        v(i) = rhs[i];
}

template <class T>
NRVec<T> & NRVec<T>::operator=(const NRVec<T> &rhs){
    if(this != &rhs){
        if (n_vec != rhs.n_vec){
            if (v != 0) 
                delete [] (v);
            n_vec=rhs.n_vec;
            v = new T[n_vec];
        }
        for (int i=0; i<n_vec; i++)
            v[i]=rhs[i];
    }
    return *this;
}

// assign a to all enteries
template <class T>
NRVec<T> & NRVec<T>::operator=(const T &a){
    for (int i=0; i<n_vec; i++)
        v[i] =a;
    return *this;
}

// subscripting
template <class T>
inline T & NRVec<T>::operator[](const int i){
    return v[i];
}

// subscripting
template <class T>
inline const T & NRVec<T>::operator[](const int i) const {
    return v[i];
}

// vector dim
template <class T>
inline int NRVec<T>::size() const {
    return n_vec;
}

template <class T>
NRVec<T>::~NRVec(){
    if (v != 0)
        delete[] (v);
}

template <class T>
class NRMat{
    private:
        int n_mat;
        int m_mat;
        T **v;
    public:
        NRMat();
        NRMat(int n, int m);
        NRMat(const T& a, int n, int m);
        NRMat(const T *a, int n, int m);
        NRMat(const NRMat& rhs);
        NRMat& operator=(const NRMat& rhs);
        NRMat& operator=(const T& a);
        inline T* operator[](const int i);
        inline const T* operator[](const int i) const;
        inline int nrows() const;
        inline int ncols() const;
        ~NRMat(); 
};

template <class T>
NRMat<T>::NRMat() : n_mat(0), m_mat(0), v(0) {}

template <class T>
NRMat<T>::NRMat(int n, int m) : n_mat(0), m_mat(0), v(new T*[n]){
    v[0] = new T[m*n];
    for (int i=1; i<n; i++)
        v[i] = v[i-1] + m;
}

template <class T>
NRMat<T>::NRMat(const T &a, int n, int m) : n_mat(n), m_mat(m), v(new T*[n]){
    int i,j;
    v[0] = new T[m*n];
    for (i=1; i<n; i++)
        v[i] = v[i-1] + m;
    for (i = 0; i<n; i++)
        for(j=0;j<m;j++)
            v[i][j] =a;
}

template <class T>
NRMat<T>::NRMat(const T *a, int n, int m) : n_mat(n), m_mat(m), v(new T*[n]){
    int i,j;
    v[0] = new T[m*n];
    for (i=1; i<n; i++)
        v[i] = v[i-1] + m;
    for (i = 0; i<n; i++)
        for(j=0;j<m;j++)
            v[i][j] = *a++;
}

template <class T>
NRMat<T>::NRMat(const NRMat &rhs) : n_mat(rhs.n_mat), m_mat(rhs.m_mat), v(new T*[n_mat]){
    int i,j;
    v[0] = new T*[n_mat*m_mat];
    for (i=1; i<n_mat; i++)
        v[i] = v[i-1] + m_mat;
    for (i=0; i<n_mat; i++)
        for(j=0; j<m_mat; j++)
            v[i][j] = rhs[i][j]; 
}

template <class T>
NRMat<T> & NRMat<T>::operator=(const NRMat<T> &rhs){
    if (this != &rhs){
        int i,j;
        if (n_mat != rhs.n_mat || m_mat != rhs.m_mat){
            if (v != 0) {
                delete[] (v[0]);
                delete[] (v);
            }
            n_mat = rhs.n_mat;
            m_mat = rhs.m_mat;
            v = new T*[n_mat];
            v[0] = new T*[n_mat*m_mat];
        }
        for (i =1; i<n_mat; i++)
            v[i] = v[i-1] + m_mat;
        for (i = 0; i < n_mat; i++)
            for(j = 0; j< m_mat; j++)
                v[i][j] = rhs[i][j];
    }
    return *this;
}

template <class T>
NRMat<T> & NRMat<T>::operator=(const T &a){
    for (int i=0; i<n_mat; i++)
        for(int j=0; j<m_mat; j++)
            v[i][j] = a;
    return *this; 
}

template <class T>
inline T* NRMat<T>::operator[](const int i){
    return v[i];
}

template <class T>
inline const T* NRMat<T>::operator[](const int i) const {
    return v[i];
}

template <class T>
inline int NRMat<T>::nrows() const {
    return n_mat;
}

template <class T>
inline int NRMat<T>::ncols() const {
    return m_mat;
}

template <class T>
NRMat<T>::~NRMat() {
    if (v != 0) {
        delete[] (v[0]);
        delete[] (v);
    }
}

int main() {

    NRVec<double> w(2);
    w[0] = 3;
    w[1] = 2;

    NRMat<double> A(2,2);
    A[0][0] = 1;
    A[1][0] = 0;
    A[0][1] = 1;
    A[1][1] = 0;
    std::cout << "w = \n    [ " << w[0] << " " << w[1] << " ]";
    return 0;
}