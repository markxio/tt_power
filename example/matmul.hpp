#include <vector>
#include <iostream>

void matMul(std::vector<std::vector<int>>& m1, std::vector<std::vector<int>>& m2, std::vector<std::vector<int>>& res) {
    int r1 = m1.size();
    int c1 = m1[0].size();
    int r2 = m2.size();
    int c2 = m2[0].size();

    if (c1 != r2) {
        std::cout << "Invalid Input" << std::endl;
        exit(EXIT_FAILURE);
    }

    // Resize result matrix to fit the result dimensions
    res.resize(r1, std::vector<int>(c2, 0)); 
  
    for (int i = 0; i < r1; i++) {
        for (int j = 0; j < c2; j++) {
            for (int k = 0; k < c1; k++) {
                res[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
}
