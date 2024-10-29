#include <memory>


template<typename T>
class Vector {
private:
    T *data_;
    size_t size_;
    size_t cap_;

    void Destroy(T *arr, size_t r) {
        for (size_t i = 0; i < r; ++i) {
            (arr + i)->~T();
        }
    }

public:
    Vector() : data_(nullptr), size_(0), cap_(0) {

    }

    Vector(std::initializer_list<T> init) : data_(nullptr), size_(0), cap_(0) {
        Reserve(init.size());
        for (const auto &val: init) {
            PushBack(val);
        }
    }

    explicit Vector(size_t count, const T &value = T()) : data_(nullptr), size_(0), cap_(0) {
        Reserve(count);
        for (size_t i = 0; i < count; ++i) {
            PushBack(value);
        }
    }

    Vector(const Vector &oth) : data_(nullptr), size_(0), cap_(0) {
        Reserve(oth.Size());
        for (size_t i = 0; i < oth.Size(); ++i) {
            PushBack(oth[i]);
        }
    }

    Vector(Vector &&oth) : Vector() {
        Swap(oth);
    }


    ~Vector() {
        Clear();
        delete[] reinterpret_cast<char *>(data_);
    }

    Vector &operator=(const Vector &oth) {
        if (this != &oth) {
            Clear();
            Reserve(oth.Size());
            for (size_t i = 0; i < oth.Size(); ++i) {
                PushBack(oth[i]);
            }
        }
        return *this;
    }

    Vector &operator=(Vector &&oth) {
        if (this != &oth) {
            Clear();
            this->Swap(oth);
        }
        return *this;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= cap_) {
            return;
        }
        T *new_data = reinterpret_cast<T *>(new char[new_capacity * sizeof(T)]);
        size_t r_ind = 0;
        try {
            for (; r_ind < size_; ++r_ind) {
                new(new_data + r_ind) T(data_[r_ind]);
            }
        } catch (...) {
            Destroy(new_data, r_ind);
            delete[] reinterpret_cast<char *>(new_data);
            throw;
        }
        Destroy(data_, size_);
        delete[] reinterpret_cast<char *>(data_);
        data_ = new_data;
        cap_ = new_capacity;
    }

    void Resize(size_t count, const T &value = T()) {
        if (Capacity() < count) {
            Reserve(count);
        }
        while (Size() > count) {
            PopBack();
        }
        while (Size() < count) {
            PushBack(value);
        }
    }

    void PushBack(const T &value) {
        if (Size() < Capacity()) {
            new(data_ + size_) T(value);
            ++size_;
            return;
        }
        size_t new_capacity = (cap_ == 0 ? 1 : cap_ << 1);
        T *new_data = reinterpret_cast<T *>(new char[new_capacity * sizeof(T)]);
        size_t r_ind = 0;
        try {
            for (; r_ind < size_; ++r_ind) {
                new(new_data + r_ind) T(data_[r_ind]);
            }
            new(new_data + size_) T(value);
        } catch (...) {
            Destroy(new_data, r_ind);
            if (r_ind == size_) {
                (new_data + size_)->~T();
            }
            delete[] reinterpret_cast<char *>(new_data);
            throw;
        }
        Destroy(data_, size_);
        delete[] reinterpret_cast<char *>(data_);
        data_ = new_data;
        cap_ = new_capacity;
        ++size_;
    }

    void PushBack(T &&value) {
        if (Size() < Capacity()) {
            new(data_ + size_) T(std::forward<T>(value));
            ++size_;
            return;
        }
        size_t new_capacity = (cap_ == 0 ? 1 : cap_ << 1);
        T *new_data = reinterpret_cast<T *>(new char[new_capacity * sizeof(T)]);
        size_t r_ind = 0;
        try {
            for (; r_ind < size_; ++r_ind) {
                new(new_data + r_ind) T(data_[r_ind]);
            }
            new(new_data + size_) T(std::forward<T>(value));
        } catch (...) {
            Destroy(new_data, r_ind);
            if (r_ind == size_) {
                (new_data + size_)->~T();
            }
            delete[] reinterpret_cast<char *>(new_data);
            throw;
        }
        Destroy(data_, size_);
        delete[] reinterpret_cast<char *>(data_);
        data_ = new_data;
        cap_ = new_capacity;
        ++size_;
    }

    void PopBack() {
        --size_;
        (data_ + size_)->~T();
    }

    void Clear() {
        Destroy(data_, size_);
        size_ = 0;
    }

    T &At(size_t index) {
        if (index >= Size()) {
            throw std::out_of_range("vector::At: index out of range");
        }
        return data_[index];
    }

    T &At(size_t index) const {
        if (index >= Size()) {
            throw std::out_of_range("vector::At: index out of range");
        }
        return data_[index];
    }

    T &operator[](size_t index) {
        return data_[index];
    }

    T &operator[](size_t index) const {
        return data_[index];
    }

    T &Front() {
        return data_[0];
    }

    T &Front() const {
        return data_[0];
    }

    T &Back() {
        return data_[size_ - 1];
    }

    T &Back() const {
        return data_[size_ - 1];
    }

    T *Data() {
        return data_;
    }

    const T *Data() const {
        return data_;
    }

    size_t Size() const {
        return size_;
    }

    size_t Capacity() const {
        return cap_;
    }

    bool Empty() const {
        return Size() == 0;
    }

    void Swap(Vector &oth) {
        if (this != &oth) {
            std::swap(data_, oth.data_);
            std::swap(size_, oth.size_);
            std::swap(cap_, oth.cap_);
        }
    }

    void Swap(Vector &&oth) {
        if (this != &oth) {
            std::swap(data_, oth.data_);
            std::swap(size_, oth.size_);
            std::swap(cap_, oth.cap_);
        }
    }

    class Iterator : public std::iterator<std::random_access_iterator_tag, T> {
    private:
        T *ptr_;
    public:
        explicit Iterator(T *ptr) : ptr_(ptr) {}

        T &operator*() {
            return *ptr_;
        }

        Iterator &operator++() {
            ++ptr_;
            return *this;
        }

        Iterator operator++(int) {
            Iterator cur = *this;
            ++ptr_;
            return cur;
        }

        Iterator &operator--() {
            --ptr_;
            return *this;
        }

        Iterator operator--(int) {
            Iterator cur = *this;
            --ptr_;
            return cur;
        }

        Iterator operator+(size_t ind) const {
            return Iterator(ptr_ + ind);
        }

        Iterator operator-(size_t ind) const {
            return Iterator(ptr_ - ind);
        }

        bool operator!=(const Iterator &oth) const {
            return ptr_ != oth.ptr_;
        }

        bool operator==(const Iterator &oth) const {
            return ptr_ == oth.ptr_;
        }
    };

    Iterator begin() { // NOLINT
        return Iterator(data_);
    }

    Iterator end() { // NOLINT
        return Iterator(data_ + size_);
    }
};