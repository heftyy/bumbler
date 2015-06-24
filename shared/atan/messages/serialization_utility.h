#pragma once

#include <memory>
#include <boost/serialization/split_free.hpp>

namespace boost {
    namespace serialization {

        /////////////////////////////////////////////////////////////
        // implement serialization for shared_ptr< T >
        // note: this must be added to the boost namespace in order to
        // be called by the library
        template<class Archive, class T>
        inline void save(Archive & ar, const std::shared_ptr< T > &t, const unsigned int file_version) {
            // only the raw pointer has to be saved
            // the ref count is rebuilt automatically on load
            const T * const tx = t.get();
            ar << tx;
        }

        template<class Archive, class T>
        inline void load(Archive & ar, std::shared_ptr< T > &t, const unsigned int file_version) {
            T *pTarget;
            ar >> pTarget;
            // note that the reset automagically maintains the reference count
            #if BOOST_WORKAROUND(BOOST_DINKUMWARE_STDLIB, == 1)
                    t.release();
                    t = std::shared_ptr< T >(pTarget);
                #else
                    t.reset(pTarget);
            #endif
        }

        // split non-intrusive serialization function member into separate
        // non intrusive save/load member functions
        template<class Archive, class T>
        inline void serialize(Archive & ar, std::shared_ptr< T > &t, const unsigned int file_version){
            boost::serialization::split_free(ar, t, file_version);
        }

        /////////////////////////////////////////////////////////////
        // implement serialization for unique_ptr< T >
        // note: this must be added to the boost namespace in order to
        // be called by the library
        template<class Archive, class T>
        inline void save(Archive & ar, const std::unique_ptr< T > &t, const unsigned int file_version) {
            // only the raw pointer has to be saved
            // the ref count is rebuilt automatically on load
            const T * const tx = t.get();
            ar << tx;
        }

        template<class Archive, class T>
        inline void load(Archive & ar, std::unique_ptr< T > &t, const unsigned int file_version) {
            T *pTarget;
            ar >> pTarget;
            t.reset(pTarget);
        }

        // split non-intrusive serialization function member into separate
        // non intrusive save/load member functions
        template<class Archive, class T>
        inline void serialize(Archive & ar, std::unique_ptr< T > &t, const unsigned int file_version){
            boost::serialization::split_free(ar, t, file_version);
        }

    } // namespace serialization
} // namespace boost