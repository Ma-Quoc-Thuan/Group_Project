#ifndef USERFACTORY_H
#define USERFACTORY_H

#include "user.h"

// Áp dụng Factory Method Pattern để khởi tạo đối tượng User linh hoạt
class UserFactory {
public:
    static User* createUser(const QString& username, const QString& password, const QString& role, const QString& mssv = "") {
        if (role.toUpper() == "ADMIN") {
            return new Admin(username, password);
        } else if (role.toUpper() == "STUDENT") {
            return new StudentUser(username, password, mssv);
        }
        return nullptr;
    }
};

#endif // USERFACTORY_H