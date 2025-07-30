#include "../utils.h"
#include <iostream>
#include <string>

using namespace std;

Cookie::Cookie(const string &name, const string &value, int max_age, const string &path, const string &domain, bool secure, bool http_only)
    : name(name),
      value(value),
      path(path),
      domain(domain),
      creat_at(time(nullptr)),
      max_age(max_age),
      secure(secure),
      http_only(http_only) {}
      
bool Cookie::isExpired(time_t now) const {
    return (now - creat_at) > max_age;
}

string Cookie::toString() const {
    string cookie_str = name + "=" + value;
    if (!path.empty()) {
        cookie_str += "; Path=" + path;
    }
    if (!domain.empty()) {
        cookie_str += "; Domain=" + domain;
    }
    if (max_age > 0) {
        cookie_str += "; Max-Age=" + to_string(max_age);
    }
    if (secure) {
        cookie_str += "; Secure";
    }
    if (http_only) {
        cookie_str += "; HttpOnly";
    }
    return cookie_str;
}
