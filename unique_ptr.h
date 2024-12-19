#ifndef UNTITLED4_UNIQUE_ptr__H
#define UNTITLED4_UNIQUE_ptr__H
#include <iostream>
#include <algorithm>
#include <memory>
#pragma once
template <typename T>
class UniquePtr {
 private:
  T* ptr_;

 public:
  //по умолчанию
  UniquePtr() : ptr_(nullptr){};
  //от указателя
  explicit UniquePtr(T* pointer) : ptr_(pointer){};
  //деструктор
  ~UniquePtr() {
    delete ptr_;
  }
  //
  UniquePtr(const UniquePtr&) = delete;
  UniquePtr& operator=(const UniquePtr&) = delete;
  //Перемещающий конструктор и перемещающее присваивание
  UniquePtr(UniquePtr&& other) noexcept : ptr_(other.ptr_) {
    ptr_ = other.ptr_;
    other.ptr_ = nullptr;
  }
  UniquePtr& operator=(UniquePtr&& other) noexcept {
    if (this != &other) {
      delete ptr_;
      ptr_ = other.ptr_;
      other.ptr_ = nullptr;
    }
    return *this;
  }
  // release()
  T* Release() noexcept {
    T* c = ptr_;
    ptr_ = nullptr;
    return c;
  }
  // get
  T* Get() const {
    return ptr_;
  }
  // reset
  void Reset(T* ptr = nullptr) noexcept {
    if (ptr_ != ptr) {
      delete ptr_;
      ptr_ = ptr;
    }
  }
  // swap
  void Swap(UniquePtr<T>& other) noexcept {
    auto tmp = ptr_;
    ptr_ = other.ptr_;
    other.ptr_ = tmp;
  }
  //
  T& operator*() const noexcept {
    return *ptr_;
  }
  T* operator->() const noexcept {
    return ptr_;
  }
  //
  explicit operator bool() const noexcept {
    return ptr_ != nullptr;
  }
};
#endif  // UNTITLED4_UNIQUE_ptr__H
