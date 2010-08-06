#ifndef __my_smart_ptr_t__
#define __my_smart_ptr_t__

#include <assert.h>

namespace smart_ptrs{

template<class T> class my_smart_ptr_t {
protected:
	T* pRep;
	unsigned int* pUseCount;
public:

	my_smart_ptr_t()
    : pRep(0), pUseCount(0)
    {}

    //What will happen if rep is NULL? -> bug
	explicit my_smart_ptr_t(T* rep)
    : pRep(rep), pUseCount( new unsigned int(1) )
	{}

    template<class Y>
	my_smart_ptr_t(const my_smart_ptr_t<Y>& r)
    : pRep(0), pUseCount(0)
    {
		pRep = r.get();
		pUseCount = r.useCountPointer();
		if(pUseCount){
			++(*pUseCount);
		}
    }

    template< class Y>
	my_smart_ptr_t& operator=(const my_smart_ptr_t<Y>& r){
		if( pRep == r.pRep ){
			return *this;
	    }

		release();

		pRep = r.get();
		pUseCount = r.useCountPointer();
		if(pUseCount){
			++(*pUseCount);
		}
		return *this;
	}

	virtual ~my_smart_ptr_t() {
        release();
	}

	inline T& operator*() const {
	    assert(pRep); return *pRep;
	}

	inline T* operator->() const {
	    assert(pRep); return pRep;
	}

	inline T* get() const {
	    return pRep;
	}

	inline unsigned int* useCountPointer() const {
	    return pUseCount;
	}

	inline T* getPointer() const {
	    return pRep;
	}

protected:

    inline void release(void){
		bool destroyThis = false;

		if( pUseCount ){
			if( --(*pUseCount) == 0){
				destroyThis = true;
	        }
		}
		if (destroyThis){
			destroy();
	    }
    }

    virtual void destroy(void){
        delete pRep;
        delete pUseCount;
    }
};

template<class T, class U> inline bool operator==(my_smart_ptr_t<T> const& a, my_smart_ptr_t<U> const& b)
{
	return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(my_smart_ptr_t<T> const& a, my_smart_ptr_t<U> const& b)
{
	return a.get() != b.get();
}

}

#endif //__my_smart_ptr_t__

