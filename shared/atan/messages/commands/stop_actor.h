#pragma once

template<typename T>
class stop_actor {
public:
    T data;

    stop_actor() {}
    stop_actor(const char* data) : data(std::string(data)) {}
    stop_actor(T data) : data(data) {}

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & this->data;
    }
};