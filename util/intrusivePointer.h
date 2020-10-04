struct RefCountable {
    std::size_t count = 0;

    void addRef() {
        count++;
    };

    void release() {
        count--;
    }

    bool shared() {
        return count == 1;
    }
};

template<typename T>
void intrusive_ptr_add_ref(T* iptr) {
    iptr->addRef();
}

template<typename T>
void intrusive_ptr_release(T* iptr) {
    iptr->release();
}

template<typename T>
class intrusive_ptr {
protected:
    template<typename U> friend class intrusive_ptr;

    T* ptr;

public:
    intrusive_ptr() : ptr(nullptr) {}

    intrusive_ptr(T* ptr, bool addRef = true) : ptr(ptr) {
        if (ptr && addRef)
            intrusive_ptr_add_ref(ptr);
    }

    intrusive_ptr(const intrusive_ptr& iptr) : ptr(iptr.ptr) {
        if (ptr)
            intrusive_ptr_add_ref(ptr);
    }

    ~intrusive_ptr() {
        if (ptr)
            intrusive_ptr_release(ptr);
    }

    intrusive_ptr& operator=(const intrusive_ptr& iptr) {
        return operator=(iptr.ptr);
    }

    intrusive_ptr& operator=(intrusive_ptr&& iptr) {
        swap(iptr);
        return *this;
    }

    template<typename U>
    intrusive_ptr& operator=(const intrusive_ptr<U>& iptr) {
        return operator=(iptr.ptr);
    }

    intrusive_ptr& operator=(T* ptr) {
        if (ptr != this->ptr) {
            const T* tmp = this->ptr;
            if (ptr)
                onIntrusiveAddRef(ptr);
            this->ptr = ptr;
            if (tmp)
                onIntrusiveRelease(tmp);
        }
    }

    T& operator*() const {
        return *ptr;
    }

    T* get() const {
        return ptr;
    }

    T* operator->() const {
        return ptr;
    }

    void reset() {
        T* const tmp = ptr;
        ptr = nullptr;
        if (tmp)
            intrusive_ptr_release(tmp);
    }

    void swap(intrusive_ptr& iptr) {
        T* const tmp = ptr;
        ptr = iptr.ptr;
        iptr.ptr = tmp;
    }

    void attach(T* ptr) {
        T* const tmp = this->ptr;
        this->ptr = ptr;
        if (tmp)
            intrusive_ptr_release(tmp);
    }

    T* detach() {
        T* const tmp = ptr;
        ptr = nullptr;
        return tmp;
    }

    operator bool() const {
        return ptr != nullptr;
    }

    bool operator!() const {
        return ptr == nullptr;
    }
};

template <typename T, typename U>
bool operator==(const intrusive_ptr<T>& iptr1, const intrusive_ptr<U>& iptr2) {
    return (iptr1.get() == iptr2.get());
}

template <typename T, typename U>
bool operator!=(const intrusive_ptr<T>& iptr1, const intrusive_ptr<U>& iptr2) {
    return (iptr1.get() != iptr2.get());
}

template <typename T>
bool operator==(const intrusive_ptr<T>& iptr, T* ptr) {
    return (iptr.get() == ptr);
}

template <typename T>
bool operator!=(const intrusive_ptr<T>& iptr, T* ptr) {
    return (iptr.get() != ptr);
}

template <typename T>
bool operator==(T* ptr, const intrusive_ptr<T>& iptr) {
    return (ptr == iptr.get());
}

template <typename T>
bool operator!=(T* ptr, const intrusive_ptr<T>& iptr) {
    return (ptr != iptr.get());
}

template<typename T>
intrusive_ptr<T> make_intrusive(T* ptr) {
    return intrusive_ptr<T>(ptr);
}

template <class T, class U>
intrusive_ptr<T> intrusive_cast(const intrusive_ptr<U>& iptr) {
    return make_intrusive(static_cast<T*>(iptr.get()));
}
