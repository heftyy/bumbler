#pragma once

#include <string>
#include <boost/functional/hash.hpp>

namespace bumbler {

class identifier {
public:
    identifier() : hash_(0) {}

    identifier(const char* string) : identifier(std::string(string)) {}

    identifier(const std::string& string) : string_(string) {
        static boost::hash<std::string> name_hash;
        hash_ = name_hash(string);
    }

    bool operator==(const identifier& rhs) const {
        return hash_ == rhs.hash_;
    }

    bool operator!=(const identifier& rhs) const {
        return hash_ != rhs.hash_;
    }

    bool operator<(const identifier& rhs) const {
        return hash_ < rhs.hash_;
    }

    bool operator>(const identifier& rhs) const {
        return hash_ > rhs.hash_;
    }

    bool valid() const {
        return hash_ != 0;
    }

    const std::string& to_string() const {
        return string_;
    }

    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & string_;
        ar & hash_;
    }

private:
    std::string string_;
    size_t hash_;
};

}
