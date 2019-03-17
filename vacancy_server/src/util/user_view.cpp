#include <util/user_view.h>

namespace util {

    std::string UserView::get_user_input(const std::string &str) {
        std::string input;
        std::cout << (str.empty() ? "> " : str);
        if (getline(std::cin, input)) {
            return input;
        }
        return "";
    }

    void UserView::println(const std::string &str) {
        std::cout << str << std::endl;
    }

} // namespace util