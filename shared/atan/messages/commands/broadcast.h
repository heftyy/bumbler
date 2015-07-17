#pragma once

template<typename T>
class broadcast {
public:
    T data;

    broadcast() { }
    broadcast(T data) : data(data) {}

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & this->data;
    }
};