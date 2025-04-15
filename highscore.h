#pragma once
// highscore.h

#ifndef HIGHSCORE_H
#define HIGHSCORE_H

// Hàm để đọc điểm cao từ file
int readHighScore();

// Hàm để ghi điểm cao vào file
void writeHighScore(int score);

// Hàm để tạo file highscore.txt nếu không tồn tại
void createHighScoreFileIfNotExist();

#endif  // HIGHSCORE_H
