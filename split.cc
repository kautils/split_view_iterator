

#include "split.h"
#include <stdint.h>
#include <string.h>
#include <string>
#include <stdarg.h>


namespace kautil{

    struct split_view_exception : std::exception{
        std::string msg;
        split_view_exception(int size , const char * fmt, ...){
            va_list l;
            va_start(l,fmt);
            msg.resize(size);
            __mingw_vsprintf(msg.data(),fmt,l);
            va_end(l);
        }
        const char * what() const noexcept override{ return msg.data(); }
    };

    struct split_view {
        using size_type = uint32_t;
        
        uintptr_t cur = 0;
        uintptr_t cur_end = 0;
        uintptr_t bin =0;
        uintptr_t bin_end =0;
        uintptr_t delm=0;
        size_type delm_bytes=0;
        std::string * membuffer =0;
        split_view_iterator::current_value cur_expr;
        bool fst = true;
        bool root = true;
        
        
        split_view() {};
        ~split_view() { delete membuffer; }
    
        void setup(void *bin, size_type bytes, void *delm, size_type delm_bytes) {
            this->bin = reinterpret_cast<uintptr_t>(bin);
            this->bin_end = reinterpret_cast<uintptr_t>(bin) + bytes;
            this->delm = reinterpret_cast<uintptr_t>(delm);
            this->delm_bytes = delm_bytes;
            cur = this->bin;
            fst = true;
            root = true;
        }
    
        void reset() {
            cur = this->bin;
            fst = true;
        }
    
        bool next() {
            
            // intend to blanch less optimization
            cur = cur*fst + cur_end+delm_bytes*!fst;
            cur_end = cur;
            fst = false;

            for (; cur_end < bin_end - delm_bytes + 1; ++cur_end) {
                if (memcmp((void *) cur_end, (void *) delm, delm_bytes))continue;
                return true;
            }
            if (cur > bin_end)cur = bin_end;
            return (cur < bin_end) ? (cur_end = bin_end) : false;
        }
    
        size_type current(void **res) {
            if (res)*res = reinterpret_cast<void *>(cur);
            return cur_end - cur;
        }
        
        void increment_ref(uintptr_t addr){
            auto at = membuffer->size(); 
            membuffer->resize(membuffer->size()+sizeof(uintptr_t));
            memcpy(membuffer->data()+at,&addr,sizeof(uintptr_t));
        }
        
        void release_ref(){
            if(membuffer){
                auto release = reinterpret_cast<split_view**>(membuffer->data());
                auto release_len = membuffer->size()/sizeof(uintptr_t);
                for(auto i = 0; i < release_len;++i) delete release[i];
                membuffer->resize(0);
            }
        }
        

    };

}//namespace kautil


kautil::split_view_iterator::split_view_iterator() : m(new split_view){}
kautil::split_view_iterator::~split_view_iterator(){
    m->release_ref();
    if(m->root) delete m;
}
kautil::split_view_iterator::self_type kautil::split_view_iterator::operator++(){
    m->next(); self_type i = *this; return i;
}
kautil::split_view_iterator::self_type kautil::split_view_iterator::operator++(int junk){
    m->next(); return *this;
}
kautil::split_view_iterator::reference kautil::split_view_iterator::operator*(){
    m->cur_expr.size = m->cur_end -m->cur;
    if(m->cur > m->cur_end) throw split_view_exception{1024,"pos begin(%llx) > pos end(%llx) . [%s(%d)]",m->cur,m->cur_end,__FILE__,__LINE__};
    m->cur_expr.data = (void*)m->cur;
    return m->cur_expr;
} 
bool kautil::split_view_iterator::operator!=(const self_type& r){
    return m->cur != r.m->cur;
}
kautil::split_view_iterator kautil::split_view_iterator::begin(){
    kautil::split_view_iterator res;
    if(!m->membuffer) m->membuffer = new std::string; 
    *res.m = *m;
    res.m->root = false;
    res.m->membuffer= nullptr;
    res.m->next();
    m->increment_ref((uintptr_t)res.m);
    return res; 
}
kautil::split_view_iterator kautil::split_view_iterator::end(){
    kautil::split_view_iterator res;
    if(!m->membuffer) m->membuffer = new std::string; 
    *res.m = *m;
    res.m->fst =false;
    res.m->root = false;
    res.m->membuffer= nullptr;
    res.m->cur = res.m->bin_end;
    res.m->cur_end = res.m->bin_end;
    
    m->increment_ref((uintptr_t)res.m);
    return res;
}

void kautil::split_view_iterator::setup(void *bin, size_type bytes, void *delm, size_type delm_bytes) { m->setup(bin,bytes,delm,delm_bytes); }
void kautil::split_view_iterator::release_ref_immediately(){ m->release_ref(); } 

    

int tmain_kautil_split_static(){

    
    const char bin[1024] = {"aaa\r\nbbb\r\na"};
    const char dlm[3] = {"\r\n"};
//    while(true){
        for(auto i = 0;i < 1000 ; ++i){
            auto split = kautil::split_view_iterator{};
            split.setup((void *)bin, 1024, (void *)dlm, 2);
            for(auto & [cur,len] :split ){
                printf("+++%.*s\n",len,reinterpret_cast<char*>(cur));
                fflush(stdout);
            }
            split.release_ref_immediately();
            
        }
//    }
    
    
    return 0;
}

