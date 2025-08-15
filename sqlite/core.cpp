#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <sstream>

// Helper to convert vector<float> to a space-separated string
std::string vectorToString(const std::vector<float>& vec) {
    std::ostringstream oss;
    for (size_t i = 0; i < vec.size(); ++i) {
        oss << vec[i];
        if (i + 1 < vec.size()) oss << " ";
    }
    return oss.str();
}

int main() {
    sqlite3* db;
    char* errMsg = nullptr;

    // Open (or create) database
    if (sqlite3_open("embeddings.db", &db) != SQLITE_OK) {
        std::cerr << "Error opening DB: " << sqlite3_errmsg(db) << "\n";
        return 1;
    }

    // Create table if not exists
    const char* create_table_sql =
        "CREATE TABLE IF NOT EXISTS embeddings ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT,"
        "label TEXT,"
        "vector TEXT"
        ");";
    if (sqlite3_exec(db, create_table_sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Error creating table: " << errMsg << "\n";
        sqlite3_free(errMsg);
    }

    // 1️⃣ Read existing embeddings first
    const char* select_sql = "SELECT id, label, vector FROM embeddings;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, select_sql, -1, &stmt, nullptr) == SQLITE_OK) {
        std::cout << "=== Existing records ===\n";
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            int id = sqlite3_column_int(stmt, 0);
            const unsigned char* lbl = sqlite3_column_text(stmt, 1);
            const unsigned char* vec_txt = sqlite3_column_text(stmt, 2);
            std::cout << "ID: " << id
                      << " | Label: " << lbl
                      << " | Vector: " << vec_txt << "\n";
        }
        sqlite3_finalize(stmt);
    } else {
        std::cerr << "Error selecting data: " << sqlite3_errmsg(db) << "\n";
    }

    // 2️⃣ Now insert a new embedding
    std::vector<float> embedding = {0.1f, 0.25f, -0.5f, 1.2f};
    std::string vec_str = vectorToString(embedding);
    std::string label = "person_001";

    std::string insert_sql =
        "INSERT INTO embeddings (label, vector) VALUES ('" +
        label + "', '" + vec_str + "');";

    if (sqlite3_exec(db, insert_sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Error inserting data: " << errMsg << "\n";
        sqlite3_free(errMsg);
    } else {
        std::cout << "Inserted new embedding for: " << label << "\n";
    }

    // Close DB
    sqlite3_close(db);
    return 0;
}

