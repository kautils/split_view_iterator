

#ifndef KAUTIL_SPLIT_SPLIT_H
#define KAUTIL_SPLIT_SPLIT_H


#include <stdint.h>

namespace kautil{
    struct split_view;
    struct split_view_iterator{
        struct current_value{ void * data=0;uint64_t size =0; };
        using size_type = uint32_t;
        using reference = current_value&;
        
        split_view_iterator();
        virtual ~split_view_iterator();
        using self_type =  split_view_iterator;
        virtual void setup(void * bin,size_type bytes,void * delm, size_type delm_bytes);
        virtual self_type operator++();
        virtual self_type operator++(int junk);
        virtual bool operator!=(const self_type& r);
        virtual kautil::split_view_iterator begin();
        virtual kautil::split_view_iterator end();
        virtual void release_ref_immediately(); /// @note release ref immediately, it is not need nesseswarily
        virtual reference operator*(); 
    private:
        split_view * m =0;
    };


    
}//namespace kautil


#endif