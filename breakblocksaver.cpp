#include <stdio.h>
#include <string>
#include <iostream>
#include "sqlite3.h"

#pragma comment(lib, "sqlite3.lib")

#define DB_NAME "breakblock.db"

// �����ͺ��̽� ����
sqlite3* open_db() {
    sqlite3* db;
    if (sqlite3_open(DB_NAME, &db)) {
        std::cerr << "�����ͺ��̽� ���� ����: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }
    return db;
}

// ���� ���� �Լ�
void save_score(const std::string& player_name, int score) {
    sqlite3* db = open_db();
    if (db == nullptr) {
        return;
    }

    // ������ ���� ����
    std::string sql = "INSERT INTO scores (name, score) VALUES ('" + player_name + "', " + std::to_string(score) + ");";
    char* errMsg = nullptr;

    // ���� ����
    if (sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "���� ���� ����: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    else {
        std::cout << "���� ���� ����: " << player_name << " - " << score << std::endl;
    }

    sqlite3_close(db);
}

// �����ͺ��̽� �ʱ�ȭ (���̺� ����)
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
        std::cerr << "���̺� ���� ����: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }
    else {
        std::cout << "���̺� ���� �Ǵ� �̹� ������." << std::endl;
    }

    sqlite3_close(db);
}

// ���� ���� �� �̸��� ���� �Է� �ޱ�
void game_over(int score) {
    std::cout << "���� ����! ����: " << score << std::endl;
    std::cout << "�̸��� �Է��ϼ���: ";

    std::string player_name;
    std::cin >> player_name;

    // ������ �̸��� �����ͺ��̽��� ����
    save_score(player_name, score);
}

int main() {
    // �����ͺ��̽� �ʱ�ȭ
    init_db();

    // ���÷� ���� ���� �� ���� ����
    int score = 100; // ���ӿ��� ���� ����
    game_over(score);

    return 0;
}
