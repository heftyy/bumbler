#pragma once

#include <memory>
#include <atomic>
#include <boost/serialization/split_free.hpp>

namespace boost {
    namespace serialization {

        template<class Archive, class T>
        inline void save(Archive& ar, const std::atomic<T>& t, const unsigned int){
            // only the raw pointer has to be saved
            const T value = t.load();
            ar << value;
        }

        template<class Archive, class T>
        inline void load(Archive& ar, std::atomic<T>& t, const unsigned int){
            T value;
            ar >> value;
            t = value;
        }

        template<class Archive, class T>
        inline void serialize(Archive& ar, std::atomic<T>& t,
                              const unsigned int file_version){
            boost::serialization::split_free(ar, t, file_version);
        }

    } // namespace serialization
} // namespace boost