#include <stdio.h>
#include <string>
#include <iostream>
#include "sqlite3.h"

#pragma comment(lib, "sqlite3.lib")

#define DB_NAME "breakblock.db"

// 데이터베이스 열기
sqlite3* open_db() {
    sqlite3* db;
    if (sqlite3_open(DB_NAME, &db)) {
        std::cerr << "데이터베이스 열기 실패: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return db;
}

// 점수 저장 함수
void save_score(const std::string& player_name, int score) {
    sqlite3* db = open_db();
    if (db == nullptr) {
        return;
    }

    // 데이터 삽입 쿼리
    std::string sql = "INSERT INTO scores (name, score) VALUES ('" + player_name + "', " + std::to_string(score) + ");";
    char* errMsg = nullptr;

    // 쿼리 실행
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "쿼리 실행 오류: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    else {
        std::cout << "점수 저장 성공: " << player_name << " - " << score << std::endl;
    }

    sqlite3_close(db);
}

// 데이터베이스 초기화 (테이블 생성)
void init_db() {
    sqlite3* db = open_db();
    if (db == nullptr) {
        return;
    }

    const char* create_table_sql = "CREATE TABLE IF NOT EXISTS scores ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "score INTEGER NOT NULL);";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, create_table_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "테이블 생성 오류: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    else {
        std::cout << "테이블 생성 또는 이미 존재함." << std::endl;
    }

    sqlite3_close(db);
}

// 게임 종료 후 이름과 점수 입력 받기
void game_over(int score) {
    std::cout << "게임 종료! 점수: " << score << std::endl;
    std::cout << "이름을 입력하세요: ";

    std::string player_name;
    std::cin >> player_name;

    // 점수와 이름을 데이터베이스에 저장
    save_score(player_name, score);
}

int main() {
    // 데이터베이스 초기화
    init_db();

    // 예시로 게임 종료 후 점수 저장
    int score = 100; // 게임에서 얻은 점수
    game_over(score);

    return 0;
}
