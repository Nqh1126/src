// highscore.cpp
#include <fstream>  // Để làm việc với file
#include <iostream> // Để in thông báo lỗi
#include "highscore.h"

// Hàm để đọc điểm cao từ file
int readHighScore() {
    std::ifstream file("highscore.txt");
    int highScore = 0;  // Giá trị mặc định nếu file không tồn tại
    if (file.is_open()) {
        file >> highScore;
        file.close();
    }
    return highScore;
}

// Hàm để ghi điểm cao vào file
void writeHighScore(int score) {
    std::ofstream file("highscore.txt");
    if (file.is_open()) {
        file << score;
        file.close();
    }
}

// Hàm để tạo file highscore.txt nếu không tồn tại
void createHighScoreFileIfNotExist() {
    std::ifstream file("highscore.txt");
    if (!file.is_open()) {
        std::ofstream newFile("highscore.txt");
        if (newFile.is_open()) {
            newFile << 0;  // Ghi điểm cao ban đầu (0) vào file
            newFile.close();
        }
        else {
            std::cerr << "Không thể tạo file highscore.txt!" << std::endl;
        }
    }
    else {
        file.close();
    }
}
