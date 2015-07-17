#pragma once

template<typename T>
class kill_actor {
public:
    T data;

    kill_actor() {}
    kill_actor(T data) : data(data) {}

private:
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive& ar, const unsigned int version) {
        ar & this->data;
    }
};