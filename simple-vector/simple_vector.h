
#pragma once
 
#include <algorithm>
#include "array_ptr.h"
#include <cassert>
#include <initializer_list>
#include <iterator>
#include <stdexcept>
 
class ReserveProxyObj {
public:
    ReserveProxyObj(size_t capacity_to_reserve)
        : new_capacity_(capacity_to_reserve)
    {
    }
    
    size_t GetNewCapacity() {
        return new_capacity_;
    }
    
private:
    size_t new_capacity_;
};
 
template <typename Type>
class SimpleVector {
 
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;
 
    SimpleVector() noexcept = default;
 
    // Создаёт вектор из size элементов, инициализированных значением по умолчанию
    explicit SimpleVector(size_t size)
        : simple_vector_(size), size_(size), capacity_(size)
    {
        for (auto it  = begin(); it != end(); ++it) {
                *it = Type{};
            }
    }
 
    // Создаёт вектор из size элементов, инициализированных значением value
    SimpleVector(size_t size, const Type& value) 
        : simple_vector_(size), size_(size), capacity_(size)    
    {
        for (auto it  = begin(); it != end(); ++it) {
                *it = value;
            }
    }
    
    SimpleVector(size_t size, Type&& value) 
        : simple_vector_(size), size_(size), capacity_(size)    
    {
        for (auto it = begin(); it != end(); ++it) {
            *it = std::move(value);
        }
    }
 
    // Создаёт вектор из std::initializer_list
    SimpleVector(std::initializer_list<Type> init) 
        : simple_vector_(init.size()), size_(init.size()), capacity_(init.size())
    {
        std::move(std::make_move_iterator(init.begin()), std::make_move_iterator(init.end()), begin());
    }    
    
    SimpleVector(const SimpleVector& other) {
        SimpleVector temp(other.GetSize());
        std::copy(other.begin(), other.end(), temp.begin());
        temp.size_ = other.size_;
        temp.capacity_ = other.capacity_;
        swap(temp);
    }
    
    SimpleVector(SimpleVector&& other) {
        SimpleVector temp(other.GetSize());
        std::copy(std::make_move_iterator(other.begin()), std::make_move_iterator(other.end()), temp.begin());
        temp.size_ = std::exchange(other.size_, 0);
        temp.capacity_ = std::exchange(other.capacity_, 0);
        swap(temp);
    }
    
    SimpleVector(ReserveProxyObj value) {
        Reserve(value.GetNewCapacity());
    }
    
    // Возвращает количество элементов в массиве
    size_t GetSize() const noexcept {
        return size_;
    }
 
    // Возвращает вместимость массива
    size_t GetCapacity() const noexcept {
        return capacity_;
    }
 
    // Сообщает, пустой ли массив
    bool IsEmpty() const noexcept {
        return (size_ == 0);
    }
 
    // Обнуляет размер массива, не изменяя его вместимость
    void Clear() noexcept {
        size_ = 0;
    }
 
    // Изменяет размер массива.
    // При увеличении размера новые элементы получают значение по умолчанию для типа Type
    void Resize(size_t new_size) {
        if (new_size <= size_) {
            size_ = new_size;
            return;
        }
        else if (new_size < capacity_) {
            for (auto it  = end(); it != begin() + new_size; ++it) {
                *it = Type{};
            }
            size_ = new_size;
            return;
        } else {
            ArrayPtr<Type> temp(new_size * 2);
            for (auto it  = temp.Get(); it != temp.Get() + new_size; ++it) {
                *it = Type{};
            }
            std::move(std::make_move_iterator(begin()), std::make_move_iterator(end()), temp.Get());
            simple_vector_.swap(temp);
            size_ = new_size;
            capacity_ = new_size * 2;
            return;
        }
    }
 
    // Возвращает ссылку на элемент с индексом index
    Type& operator[](size_t index) noexcept {
        assert(index < size_);
        return simple_vector_[index];
    }
 
    // Возвращает константную ссылку на элемент с индексом index
    const Type& operator[](size_t index) const noexcept {
        assert(index < size_);
        return simple_vector_[index];
    }
 
    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    Type& At(size_t index) {
        if (!(index < size_))
            throw std::out_of_range("The index value is out of range");
        return simple_vector_[index];
    }
 
    // Возвращает константную ссылку на элемент с индексом index
    // Выбрасывает исключение std::out_of_range, если index >= size
    const Type& At(size_t index) const {
        if (!(index < size_))
            throw std::out_of_range("The index value is out of range");
        return simple_vector_[index];
    }
 
    // Возвращает итератор на начало массива
    Iterator begin() noexcept {
        return simple_vector_.Get();
    }
 
    // Возвращает итератор на элемент, следующий за последним
    Iterator end() noexcept {
        return simple_vector_.Get() + size_;
    }
 
    // Возвращает константный итератор на начало массива
    ConstIterator begin() const noexcept {
        return simple_vector_.Get();
    }
 
    // Возвращает итератор на элемент, следующий за последним
    ConstIterator end() const noexcept {
        return simple_vector_.Get() + size_;
    }
 
    // Возвращает константный итератор на начало массива
    ConstIterator cbegin() const noexcept {
        return simple_vector_.Get();
    }
 
    // Возвращает итератор на элемент, следующий за последним
    ConstIterator cend() const noexcept {
        return simple_vector_.Get() + size_;
    }
 
    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs){
            SimpleVector temp(rhs);
            swap(temp);
        }
        return *this;
    }
    
    SimpleVector& operator=(SimpleVector&& rhs) {
        if (this != &rhs){
            SimpleVector temp(rhs);
            swap(temp);
        }
        return *this;
    }
 
    // Добавляет элемент в конец вектора
    // При нехватке места увеличивает вдвое вместимость вектора
    void PushBack(const Type& item) {
        const auto pos = size_;
        if (size_ == capacity_) {
            Resize(capacity_ + 1);
        } else {
            ++size_;
        }
        simple_vector_[pos] = item;
    }
    
    void PushBack(Type&& item) {
        const auto pos = size_;
        if (size_ == capacity_) {
            Resize(capacity_ + 1);
        } else {
            ++size_;
        }
        simple_vector_[pos] = std::move(item);
    }
 
    // Вставляет значение value в позицию pos.
    // Возвращает итератор на вставленное значение
    // Если перед вставкой значения вектор был заполнен полностью,
    // вместимость вектора должна увеличиться вдвое, а для вектора вместимостью 0 стать равной 1
    Iterator Insert(ConstIterator pos, const Type& value) {
        auto delta = pos - begin();
        if (size_ < capacity_) {
            std::copy_backward(begin() + delta, end(), end() + 1);
            simple_vector_[delta] = value;
            ++size_;
        } else {
            Resize(size_ + 1);
            std::copy_backward(begin() + delta, end(), end() + 1);
            simple_vector_[delta] = value;
        }
        return begin() + delta;
    }
    
    Iterator Insert(ConstIterator pos, Type&& value) {
        auto delta = pos - begin();
        if (size_ < capacity_) {
            std::move_backward(std::make_move_iterator(begin() + delta), std::make_move_iterator(end()), end() + 1);
            simple_vector_[delta] = std::move(value);            
            ++size_;
        } else {
            Resize(size_ + 1);
            std::move_backward(std::make_move_iterator(begin() + delta), std::make_move_iterator(end()), end() + 1);
            simple_vector_[delta] = std::move(value);
        }
        return begin() + delta;
    }
 
    // "Удаляет" последний элемент вектора. Вектор не должен быть пустым
    void PopBack() noexcept {
        assert(size_ != 0);
        --size_;
    }
 
    // Удаляет элемент вектора в указанной позиции
    Iterator Erase(ConstIterator pos) {
        assert(size_ != 0);
        std::move(const_cast<Iterator>(pos) + 1, end(), const_cast<Iterator>(pos));
        --size_;
        return const_cast<Iterator>(pos);
    }
 
    // Обменивает значение с другим вектором
    void swap(SimpleVector& other) noexcept {
        simple_vector_.swap(other.simple_vector_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
    
    void swap(SimpleVector&& other) noexcept {
        simple_vector_.swap(other.simple_vector_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }
    
    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_) {
            auto temp_size = size_;
            SimpleVector temp (new_capacity);
            std::copy(begin(), end(), temp.begin());
            swap(temp);
            size_ = temp_size;
        }
    }
    
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