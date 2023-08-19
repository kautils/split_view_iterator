### split_view
* split bianry data without copy
* c++11 foreach
* possible to export via dlopen or LoadLibrary

### example 
```c++
int tmain_kautil_split_static(){
    const char bin[1024] = {"aaa\r\nbbb\r\na"};
    const char dlm[3] = {"\r\n"};
    for(auto i = 0;i < 1000 ; ++i){
        auto split = kautil::split_view_iterator{};
        split.setup((void *)bin, 1024, (void *)dlm, 2);
        for(auto & [len,cur] :split ){
            printf("%.*s\n",len,reinterpret_cast<char*>(cur));
            fflush(stdout);
        }
        //split.release_ref_immediately();
        
    }
    return 0;
}
```

### note 
* when use kautil::split_view_iterator::begin() or kautil::split_view_iterator::end() in multithread program, need to use mutex like below.
* but no need for mutex when it is used independent objects.
```c++
#include <pthread.h>

static auto kMtx = PTHREAD_NORMAL_MUTEX_INITIALIZER;
auto split = kautil::split_view_iterator{};
pthread_mutex_lock(&kMtx);
split.begin();
pthread_mutex_unlock(&kMtx);


```