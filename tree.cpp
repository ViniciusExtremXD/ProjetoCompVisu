#include <iostream>
#include <filesystem>
#include <string>

namespace fs = std::filesystem;

void print_tree(const fs::path& path, int depth = 0) {
    if (!fs::exists(path)) return;

    std::string indent(depth * 2, ' '); // indentação
    std::cout << indent << path.filename().string();

    if (fs::is_directory(path)) {
        std::cout << "/\n";
        for (const auto& entry : fs::directory_iterator(path)) {
            print_tree(entry.path(), depth + 1);
        }
    } else {
        std::cout << "\n";
    }
}

int main() {
    fs::path root = fs::current_path();
    std::cout << "Estrutura do projeto em: " << root << "\n\n";
    print_tree(root);
    return 0;
}
