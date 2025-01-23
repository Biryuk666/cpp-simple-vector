
#pragma once
 
#include <algorithm>
#include "array_ptr.h"
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
 
class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity_to_reserve);
    
    size_t GetNewCapacity();
    
private:
    size_t new_capacity_;
};
 
template <typename Type>
class SimpleVector {
 
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;
 
    SimpleVector() noexcept = default; 
    explicit SimpleVector(size_t size); 
    SimpleVector(size_t size, const Type& value); 
    SimpleVector(std::initializer_list<Type> init);    
    SimpleVector(const SimpleVector& other);    
    SimpleVector(SimpleVector&& other);    
    SimpleVector(ReserveProxyObj value);
    
    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept;

    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept;
 
    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept;
 
    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept;

    // Изменяет вместимость массива, при условии, что новая вместимость больше, чем текущая
    void Reserve(size_t new_capacity);
 
    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size);

    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item);

    void PushBack(Type&& item);

    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept;

    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value);

    Iterator Insert(ConstIterator pos, Type&& value);

    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos);

    // Обменивает значение с другим вектором
    void Swap(SimpleVector& other) noexcept;

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index);

    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const;

    // Возвращает итератор на начало массива
    Iterator begin() noexcept;

    // Возвращает итератор на элемент, следующий за последним
    Iterator end() noexcept;

    // Возвращает константный итератор на начало массива
    ConstIterator begin() const noexcept;

    // Возвращает итератор на элемент, следующий за последним
    ConstIterator end() const noexcept;

    // Возвращает константный итератор на начало массива
    ConstIterator cbegin() const noexcept;

    // Возвращает итератор на элемент, следующий за последним
    ConstIterator cend() const noexcept;
 
    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept;
 
    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept;

    SimpleVector& operator=(const SimpleVector& rhs);

    SimpleVector& operator=(SimpleVector&& rhs);
    
private:
    ArrayPtr<Type> simple_vector_;
    size_t size_ = 0;
    size_t capacity_ = 0;
};
 
ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}
 
template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return lhs.GetSize() == rhs.GetSize() && std::equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
}
 
template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs == rhs);
}
 
template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return std::lexicographical_compare(lhs.begin(), lhs.end(), rhs.begin(), rhs.end(),
                                        [] (const auto& lhs_value, const auto& rhs_value) {
        return lhs_value < rhs_value;
    });
}
 
template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(rhs < lhs);
}
 
template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return rhs < lhs;
}
 
template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return !(lhs < rhs);
}



// ------------------ReserveProxyObj-----------------

ReserveProxyObj::ReserveProxyObj(size_t capacity_to_reserve) : new_capacity_(capacity_to_reserve) {}

size_t ReserveProxyObj::GetNewCapacity() {
    return new_capacity_;
}

// -------------------SimpleVector-------------------

template <typename Type>
SimpleVector<Type>::SimpleVector(size_t size)
    : simple_vector_(size), size_(size), capacity_(size) {
    for (auto it = begin(); it != end(); ++it) {
        *it = Type{};
    }
}

template <typename Type>
SimpleVector<Type>::SimpleVector(size_t size, const Type& value)
    : simple_vector_(size), size_(size), capacity_(size) {
    for (auto it = begin(); it != end(); ++it) {
        *it = value;
    }
}

template <typename Type>
SimpleVector<Type>::SimpleVector(std::initializer_list<Type> init)
    : simple_vector_(init.size()), size_(init.size()), capacity_(init.size()) {
    std::copy(std::make_move_iterator(init.begin()), std::make_move_iterator(init.end()), begin());
}

template <typename Type>
SimpleVector<Type>::SimpleVector(const SimpleVector& other) {
    ArrayPtr<Type> temp(other.size_);
    std::copy(other.begin(), other.end(), temp.Get());
    simple_vector_.Swap(temp);
    size_ = other.size_;
    capacity_ = other.capacity_;
}

template <typename Type>
SimpleVector<Type>::SimpleVector(SimpleVector&& other) {
    simple_vector_ = (std::move(other.simple_vector_));
    size_ = std::exchange(other.size_, 0);
    capacity_ = std::exchange(other.capacity_, 0);
}

template <typename Type>
SimpleVector<Type>::SimpleVector(ReserveProxyObj value) {
    Reserve(value.GetNewCapacity());
}

template <typename Type>
size_t SimpleVector<Type>::GetSize() const noexcept {
    return size_;
}

template <typename Type>
size_t SimpleVector<Type>::GetCapacity() const noexcept {
    return capacity_;
}

template <typename Type>
bool SimpleVector<Type>::IsEmpty() const noexcept {
    return (size_ == 0);
}

template <typename Type>
void SimpleVector<Type>::Clear() noexcept {
    size_ = 0;
}

template <typename Type>
void SimpleVector<Type>::Reserve(size_t new_capacity) {
    if (new_capacity > capacity_) {
        auto temp_size = size_;
        SimpleVector temp(new_capacity);
        std::copy(std::make_move_iterator(begin()), std::make_move_iterator(end()), temp.begin());
        Swap(temp);
        size_ = temp_size;
    }
}

template <typename Type>
void SimpleVector<Type>::Resize(size_t new_size) {
    if (new_size <= size_) {
        size_ = new_size;
        return;
    }
    else if (new_size < capacity_) {
        for (auto it = end(); it != begin() + new_size; ++it) {
            *it = Type{};
        }
        size_ = new_size;
        return;
    }
    else {
        ArrayPtr<Type> temp(new_size);
        for (auto it = temp.Get(); it != temp.Get() + new_size; ++it) {
            *it = Type{};
        }
        std::copy(begin(), end(), temp.Get());
        simple_vector_.Swap(temp);
        size_ = new_size;
        capacity_ = new_size;
        return;
    }
}

template <typename Type>
void SimpleVector<Type>::PushBack(const Type& item) {
    if (capacity_ == 0) {
        Reserve(1);
    }
    else if (size_ == capacity_) {
        Reserve(size_ * 2);
    }
    simple_vector_[size_] = item;
    ++size_;
}

template <typename Type>
void SimpleVector<Type>::PushBack(Type&& item) {
    if (capacity_ == 0) {
        Reserve(1);
    }
    else if (size_ == capacity_) {
        Reserve(size_ * 2);
    }
    simple_vector_[size_] = std::move(item);
    ++size_;
}

template <typename Type>
void SimpleVector<Type>::PopBack() noexcept {
    assert(size_ != 0);
    --size_;
}

template <typename Type>
typename SimpleVector<Type>::Iterator SimpleVector<Type>::Insert(ConstIterator pos, const Type& value) {
    assert(begin() <= pos && pos <= end());
    auto delta = pos - begin();
    if (size_ < capacity_) {
        std::copy_backward(begin() + delta, end(), end() + 1);
        simple_vector_[delta] = value;
    }
    else {
        Reserve(std::max(capacity_ * 2, (size_t)1)); // если capacity_ == 0, то зарезирвировать 1; иначе capacity_ * 2
        std::copy_backward(begin() + delta, end(), end() + 1);
        simple_vector_[delta] = value;
    }
    ++size_;
    return begin() + delta;
}

template <typename Type>
typename SimpleVector<Type>::Iterator SimpleVector<Type>::Insert(ConstIterator pos, Type&& value) {
    assert(begin() <= pos && pos <= end());
    auto delta = pos - begin();
    if (size_ < capacity_) {
        std::move_backward(std::make_move_iterator(begin() + delta), std::make_move_iterator(end()), end() + 1);
        simple_vector_[(size_t)delta] = std::move(value);
    }
    else {
        Reserve(std::max(capacity_ * 2, (size_t)1));
        std::move_backward(std::make_move_iterator(begin() + delta), std::make_move_iterator(end()), end() + 1);
        simple_vector_[(size_t)delta] = std::move(value);
    }
    ++size_;
    return begin() + delta;
}

template <typename Type>
typename SimpleVector<Type>::Iterator SimpleVector<Type>::Erase(ConstIterator pos) {
    assert(size_ != 0);
    std::move(const_cast<Iterator>(pos) + 1, end(), const_cast<Iterator>(pos));
    --size_;
    return const_cast<Iterator>(pos);
}

template <typename Type>
void SimpleVector<Type>::Swap(SimpleVector& other) noexcept {
    simple_vector_.Swap(other.simple_vector_);
    std::swap(size_, other.size_);
    std::swap(capacity_, other.capacity_);
}

template <typename Type>
Type& SimpleVector<Type>::At(size_t index) {
    if (!(index < size_))
        throw std::out_of_range("The index value is out of range");
    return simple_vector_[index];
}

template <typename Type>
const Type& SimpleVector<Type>::At(size_t index) const {
    if (!(index < size_))
        throw std::out_of_range("The index value is out of range");
    return simple_vector_[index];
}

template <typename Type>
Type& SimpleVector<Type>::operator[](size_t index) noexcept {
    assert(index < size_);
    return simple_vector_[index];
}

template <typename Type>
const Type& SimpleVector<Type>::operator[](size_t index) const noexcept {
    assert(index < size_);
    return simple_vector_[index];
}

template <typename Type>
SimpleVector<Type>& SimpleVector<Type>::operator=(const SimpleVector& rhs) {
    if (this != &rhs) {
        SimpleVector temp(rhs);
        Swap(temp);
    }
    return *this;
}

template<typename Type>
SimpleVector<Type>& SimpleVector<Type>::operator=(SimpleVector&& rhs) {
    if (this != &rhs) {
        SimpleVector temp(rhs);
        Swap(temp);
    }
    return *this;
}

template <typename Type>
typename SimpleVector<Type>::Iterator SimpleVector<Type>::begin() noexcept {
    return simple_vector_.Get();
}

template <typename Type>
typename SimpleVector<Type>::Iterator SimpleVector<Type>::end() noexcept {
    return simple_vector_.Get() + size_;
}

template<typename Type>
typename SimpleVector<Type>::ConstIterator SimpleVector<Type>::begin() const noexcept {
    return simple_vector_.Get();
}

template <typename Type>
typename SimpleVector<Type>::ConstIterator SimpleVector<Type>::end() const noexcept {
    return simple_vector_.Get() + size_;
}

template <typename Type>
typename SimpleVector<Type>::ConstIterator SimpleVector<Type>::cbegin() const noexcept {
    return simple_vector_.Get();
}

template <typename Type>
typename SimpleVector<Type>::ConstIterator SimpleVector<Type>::cend() const noexcept {
    return simple_vector_.Get() + size_;
}