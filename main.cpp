#include <iostream>
//#define WEAK_PTR_IMPLEMENTED
//#pragma once
#include <vector>
#include <tuple>
#include <memory>

class BadWeakPtr: public std::runtime_error {
 public:
  BadWeakPtr() : std::runtime_error("BadWeakPtr") {
  }
};
struct Counter {
  size_t strong_count;
  size_t weak_count;
  bool deleted = true;
};
template <class T>
class WeakPtr;
template <class T>
class SharedPtr {
 public:
  T* ptr_;
  Counter* size_;

 public:
  friend WeakPtr<T>;
  SharedPtr() : ptr_(nullptr), size_(nullptr) {
  }
  SharedPtr(T* other) : ptr_(other), size_((other == nullptr) ? nullptr : new Counter{1, 0}) {  // NOLINT
  }
  SharedPtr(const SharedPtr& other) : ptr_(other.ptr_), size_(other.size_) {
    if (size_ != nullptr) {
      (*size_).strong_count++;
    }
  }
  SharedPtr(const WeakPtr<T>& other) : ptr_(other.ptr_), size_(other.size_) {  // NOLINT
    if (other.Expired()) {
      throw BadWeakPtr{};
    }
    if (size_ != nullptr) {
      (size_->strong_count)++;
    }
  }
  SharedPtr& operator=(const SharedPtr& other) {
    if (this != &other) {
      if (size_ != nullptr) {
        (size_->strong_count)--;
        if (size_->strong_count == 0) {
          if (size_->weak_count == 0) {
            if (size_->deleted) {
              size_->deleted = false;
              delete ptr_;
            }
            delete size_;
          } else {
            if (size_->deleted) {
              size_->deleted = false;
              delete ptr_;
            }
          }
        }
      }
      ptr_ = other.ptr_;
      size_ = other.size_;
      if (size_ != nullptr) {
        size_->strong_count++;
      }
    }
    return *this;
  }
  SharedPtr(SharedPtr&& other) noexcept : ptr_(other.ptr_), size_(other.size_) {
    other.ptr_ = nullptr;
    other.size_ = nullptr;
  }
  SharedPtr& operator=(SharedPtr&& other) noexcept {
    if (this != &other) {
      if (size_ != nullptr) {
        (size_->strong_count)--;
        if (size_->strong_count == 0) {
          if (size_->weak_count == 0) {
            if (size_->deleted) {
              size_->deleted = false;
              delete ptr_;
            }
            delete size_;
          } else {
            if (size_->deleted) {
              size_->deleted = false;
              delete ptr_;
            }
          }
        }
      }
      ptr_ = other.ptr_;
      size_ = other.size_;
      other.size_ = nullptr;
      other.ptr_ = nullptr;
    }
    return *this;
  }
  void Reset(T* ptr = nullptr) {
    T* tmp = ptr_;
    if (size_ != nullptr) {
      (size_->strong_count)--;
      if ((size_->strong_count) == 0) {
        if (size_->weak_count == 0) {
          if (size_->deleted) {
            size_->deleted = false;
            delete tmp;
          }
          delete size_;
        } else {
          if (size_->deleted) {
            size_->deleted = false;
            delete tmp;
          }
        }
      }
    }
    ptr_ = ptr;
    if (ptr_ == nullptr) {
      size_ = nullptr;
    } else {
      size_ = new Counter{1, 0};
    }
  }
  void Swap(SharedPtr& other) {
    T* tmp = ptr_;
    ptr_ = other.ptr_;
    other.ptr_ = tmp;
    Counter* tmp1 = size_;
    size_ = other.size_;
    other.size_ = tmp1;
  }
  T* Get() const {
    return ptr_;
  }
  size_t UseCount() const {
    if (size_ != nullptr) {
      return size_->strong_count;
    }
    return 0;
  }
  T& operator*() const {
    return *ptr_;
  }
  T* operator->() const {
    return ptr_;
  }
  explicit operator bool() const {
    return (ptr_ != nullptr);
  }
  ~SharedPtr() {
    if (ptr_ != nullptr) {
      if (size_ != nullptr) {
        (size_->strong_count)--;
        if (size_->strong_count == 0) {
          if (size_->weak_count == 0) {
            if (size_->deleted) {
              size_->deleted = false;
              delete ptr_;
            }
            delete size_;
          } else {
            if (size_->deleted) {
              size_->deleted = false;
              delete ptr_;
            }
          }
        }
      }
    } else {
      delete size_;
    }
  }
};
template <class T>
class WeakPtr {
 public:
  T* ptr_;
  Counter* size_;

 public:
  friend SharedPtr<T>;
  WeakPtr() : ptr_(nullptr), size_(nullptr) {
  }
  WeakPtr(T* other) : ptr_(other), size_((other == nullptr) ? nullptr : new Counter{0, 1}) {  // NOLINT
  }
  WeakPtr(const WeakPtr& other) : ptr_(other.ptr_), size_(other.size_) {
    if (size_ != nullptr) {
      (size_->weak_count)++;
    }
  }
  WeakPtr(const SharedPtr<T>& other) : ptr_(other.ptr_), size_(other.size_) {  // NOLINT
    if (size_ != nullptr) {
      (size_->weak_count)++;
    }
  }
  WeakPtr& operator=(const WeakPtr& other) {
    if (this != &other) {
      if (size_ != nullptr) {
        (size_->weak_count)--;
        if (size_->strong_count == 0) {
          if (size_->weak_count == 0) {
            if (size_->deleted) {
              size_->deleted = false;
              delete ptr_;
            }
            delete size_;
          } else {
            if (size_->deleted) {
              size_->deleted = false;
              delete ptr_;
            }
          }
        }
      }
      ptr_ = other.ptr_;
      size_ = other.size_;
      if (size_ != nullptr) {
        (size_->weak_count)++;
      }
    }
    return *this;
  }
  WeakPtr(WeakPtr&& other) noexcept : ptr_(other.ptr_), size_(other.size_) {
    if (other.ptr_ != nullptr) {
      other.ptr_ = nullptr;
    }
    if (other.size_ != nullptr) {
      other.size_ = nullptr;
    }
  }
  WeakPtr& operator=(WeakPtr&& other) noexcept {
    if (this != &other) {
      if (size_ != nullptr) {
        size_->weak_count--;
        if (size_->strong_count == 0) {
          if (size_->weak_count == 0) {
            if (size_->deleted) {
              size_->deleted = false;
              delete ptr_;
            }
            delete size_;
          } else {
            if (size_->deleted) {
              size_->deleted = false;
              delete ptr_;
            }
          }
        }
      }
      ptr_ = other.ptr_;
      size_ = other.size_;
      other.size_ = nullptr;
      other.ptr_ = nullptr;
    }
    return *this;
  }
  void Swap(WeakPtr& other) {
    T* tmp = ptr_;
    ptr_ = other.ptr_;
    other.ptr_ = tmp;
    Counter* tmp1 = size_;
    size_ = other.siz_;
    other.siz_ = tmp1;
  }
  void Reset() {
    if (size_ != nullptr) {
      (size_->weak_count)--;
      if ((size_->strong_count) == 0) {
        if (size_->weak_count == 0) {
          if (size_->deleted) {
            size_->deleted = false;
            delete ptr_;
          }
          delete size_;
        } else {
          if (size_->deleted) {
            size_->deleted = false;
            delete ptr_;
          }
        }
      }
    }
    ptr_ = nullptr;
    size_ = nullptr;
  }
  size_t UseCount() const {
    if (size_ != nullptr) {
      return size_->strong_count;
    }
    return 0;
  }
  bool Expired() const {
    if (size_ != nullptr) {
      return (size_->strong_count == 0);
    }
    return true;
  }
  SharedPtr<T> Lock() const {
    if (Expired()) {
      return nullptr;
    }
    SharedPtr<T> tmp;
    tmp.ptr_ = ptr_;
    tmp.size_ = size_;
    if (size_ != nullptr) {
      size_->strong_count++;
    }
    return tmp;
  }
  ~WeakPtr() {
    if (ptr_ != nullptr) {
      if (size_ != nullptr) {
        if ((size_->strong_count) == 0) {
          (size_->weak_count)--;
          if (size_->weak_count == 0) {
            if (size_->deleted) {
              size_->deleted = false;
              delete ptr_;
            }
            delete size_;
          } else {
            if (size_->deleted) {
              size_->deleted = false;
              delete ptr_;
            }
          }
        } else {
          (size_->weak_count)--;
        }
      }
    } else {
      delete size_;
    }
  }
};
template <class U, class ... Args>
SharedPtr<U> MakeShared(Args&& ... arg) {
  return SharedPtr<U>(new U(std::forward<Args>(arg) ...));
}

int main() {
  int x = 11;
  const auto ptr = MakeShared<std::pair<int&, std::unique_ptr<int>>>(x, std::make_unique<int>(11));
  std::cout <<(ptr->first == 11);
  std::cout <<(*(ptr->second) == 11);

  x = -11;
  *(ptr->second) = -11;
  std::cout <<(ptr->first == -11);
  std::cout <<(*(ptr->second) == -11);
  return 0;
}
