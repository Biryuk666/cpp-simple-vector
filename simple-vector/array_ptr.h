#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>
#include <utility>

template <typename Type>
class ArrayPtr {
public:
    // Инициализирует ArrayPtr нулевым указателем
    ArrayPtr() = default;

    // Создаёт в куче массив из size элементов типа Type.
    // Если size == 0, поле raw_ptr_ должно быть равно nullptr
    explicit ArrayPtr(size_t size);

    // Конструктор из сырого указателя, хранящего адрес массива в куче либо nullptr
    explicit ArrayPtr(Type* raw_ptr) noexcept;

    // Запрещаем копирование
    ArrayPtr(const ArrayPtr&) = delete;  

    ArrayPtr(ArrayPtr&& other);

    ~ArrayPtr();

    // Запрещаем присваивание
    ArrayPtr& operator=(const ArrayPtr&) = delete;

    ArrayPtr& operator=(ArrayPtr&& rhs);

    // Прекращает владением массивом в памяти, возвращает значение адреса массива
    // После вызова метода указатель на массив должен обнулиться
    [[nodiscard]] Type* Release() noexcept;

    // Возвращает ссылку на элемент массива с индексом index
    Type& operator[](size_t index) noexcept;

    // Возвращает константную ссылку на элемент массива с индексом index
    const Type& operator[](size_t index) const noexcept;

    // Возвращает true, если указатель ненулевой, и false в противном случае
    explicit operator bool() const;

    // Возвращает значение сырого указателя, хранящего адрес начала массива
    Type* Get() const noexcept;

    // Обменивается значениям указателя на массив с объектом other
    void Swap(ArrayPtr& other) noexcept;
    
private:
    Type* raw_ptr_ = nullptr;
};

template <typename Type>
ArrayPtr<Type>::ArrayPtr(size_t size) {
    if (size > 0) {
        raw_ptr_ = new Type[size];
    }
}

template <typename Type>
ArrayPtr<Type>::ArrayPtr(Type* raw_ptr) noexcept {
    raw_ptr_ = raw_ptr;
}

template <typename Type>
ArrayPtr<Type>::ArrayPtr(ArrayPtr&& other) {
    ArrayPtr temp(std::exchange(other.raw_ptr_, nullptr));
    Swap(temp);
}

template <typename Type>
ArrayPtr<Type>::~ArrayPtr() {
    delete[] raw_ptr_;
}

template <typename Type>
ArrayPtr<Type>& ArrayPtr<Type>::operator=(ArrayPtr&& rhs) {
    if (this->raw_ptr_ != rhs.raw_ptr_) {
        ArrayPtr temp(std::move(rhs));
        Swap(temp);
    }
    return *this;
}

template <typename Type>
Type* ArrayPtr<Type>::Release() noexcept {
    auto temp = raw_ptr_;
    raw_ptr_ = nullptr;
    return temp;
}

template <typename Type>
Type& ArrayPtr<Type>::operator[](size_t index) noexcept {
    return raw_ptr_[index];
}

template <typename Type>
const Type& ArrayPtr<Type>::operator[](size_t index) const noexcept {
    return raw_ptr_[index];
}

template <typename Type>
ArrayPtr<Type>::operator bool() const {
    return raw_ptr_ != nullptr;
}

template <typename Type>
Type* ArrayPtr<Type>::Get() const noexcept {
    return raw_ptr_;
}

template <typename Type>
void ArrayPtr<Type>::Swap(ArrayPtr& other) noexcept {
    std::swap(raw_ptr_, other.raw_ptr_);
}
