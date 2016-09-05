#ifndef AMQP_AMQP_LOGIN_H_
#define AMQP_AMQP_LOGIN_H_

#include <string>
#include "base/string_piece.h"

namespace amqp {

class Login {
 public:
  Login(): user_("guest"), password_("guest") {}

  Login(const base::StringPiece& user,
        const base::StringPiece& password)
      : user_(user.as_string()),
        password_(password.as_string()) {}

  virtual ~Login() {}

  const std::string& user() const { return user_; }
  const std::string& password() const { return password_; }

  std::string SaslPlain() const {
    std::string result("\0", 1);
    return result.append(user_).append("\0", 1).append(password_);
  }

 private:
  std::string user_;
  std::string password_;
};

} // namespace amqp
#endif // AMQP_AMQP_LOGIN_H_
