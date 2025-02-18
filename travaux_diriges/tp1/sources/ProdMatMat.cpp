#include <algorithm>
#include <cassert>
#include <iostream>
#include <thread>
#if defined(_OPENMP)
#include <omp.h>
#endif
#include "ProdMatMat.hpp"

namespace {
void prodSubBlocks(int iRowBlkA, int iColBlkB, int iColBlkA, int szBlock,
                   const Matrix& A, const Matrix& B, Matrix& C) {
  //#pragma omp parallel for
  for (int j = iColBlkB; j < std::min(B.nbCols, iColBlkB + szBlock); j++)
    for (int k = iColBlkA; k < std::min(A.nbCols, iColBlkA + szBlock); k++)
      for (int i = iRowBlkA; i < std::min(A.nbRows, iRowBlkA + szBlock); ++i)
        C(i, j) += A(i, k) * B(k, j);
}
// const int szBlock = 32;
}  // namespace

Matrix operator*(const Matrix& A, const Matrix& B) {
  const int szBlock = Matrix::szBlock;
  //std::cout << "Using szBlock = " << szBlock << "\n";
  Matrix C(A.nbRows, B.nbCols, 0.0);
  // prodSubBlocks(0, 0, 0, std::max({A.nbRows, B.nbCols, A.nbCols}), A, B, C);
  const int nRowBlockA = (A.nbRows+szBlock-1)/szBlock; //I
  const int nColBlockB = (B.nbCols+szBlock-1)/szBlock; //J
  const int nColBlockA = (A.nbCols+szBlock-1)/szBlock; //K
  #pragma omp parallel for collapse(2)
  for (int J = 0; J < nColBlockB; J++)
    for (int K = 0; K < nColBlockA; K++)
      for (int I = 0; I < nRowBlockA; I++)
        prodSubBlocks(I*szBlock, J*szBlock, K*szBlock, szBlock, A, B, C);
  return C;
}
