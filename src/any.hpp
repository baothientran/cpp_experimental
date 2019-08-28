#ifndef ANY_HPP
#define ANY_HPP

#include <type_traits>
#include <typeinfo>
#include <utility>

class bad_any_cast : std::bad_cast {
public:
    const char *what() const throw() override {
        return "bad any_cast";
    }
};

class any {
private:
    union storage_union {
        using stack_storage_t = typename std::aligned_storage<2*sizeof(void*), alignof(void*)>::type;
        void *heap;
        stack_storage_t stack;    
    };

    struct vtable_storage {
        const std::type_info &(*type)() noexcept;
        void (*move)(storage_union &src, storage_union &dest) noexcept;
        void (*copy)(const storage_union &src, storage_union &dest);
        void (*destroy)(storage_union &storage) noexcept;
    };

    template<class ValueType>
    struct vtable_heap {
        static const std::type_info &type() noexcept {
            return typeid(ValueType);
        }

        static void move(storage_union &src, storage_union &dest) noexcept {
            dest.heap = src.heap;
            src.heap = nullptr;
        }

        static void copy(const storage_union &src, storage_union &dest) {
            dest.heap = new ValueType(*reinterpret_cast<ValueType *>(src.heap));
        }

        static void destroy(storage_union &storage) noexcept {
            delete reinterpret_cast<ValueType *>(storage.heap);
            storage.heap = nullptr;
        }
    };

    template<class ValueType>
    struct vtable_stack {
        static const std::type_info &type() noexcept {
            return typeid(ValueType);
        }

        static void move(storage_union &src, storage_union &dest) noexcept {
            new (&dest.stack) ValueType(std::move(*reinterpret_cast<ValueType*>(&src.stack)));
        }

        static void copy(const storage_union &src, storage_union &dest) {
            new (&dest.stack) ValueType(*reinterpret_cast<const ValueType*>(&src.stack));
        }

        static void destroy(storage_union &storage) noexcept {
            reinterpret_cast<ValueType *>(&storage.stack)->~ValueType();
        }
    };

    template<class ValueType>
    struct require_allocation : std::integral_constant<bool, 
                                    !(std::is_nothrow_move_constructible<ValueType>::value &&
                                      sizeof(ValueType) <= sizeof(storage_union::stack_storage_t) && 
                                      std::alignment_of<ValueType>::value <= std::alignment_of<storage_union::stack_storage_t>::value)>
    {};

private:
    vtable_storage *vtable;
    storage_union storage;

    template <class ValueType>
    static vtable_storage *construct_vtable() {
        using vtable_t = typename std::conditional<require_allocation<ValueType>::value, vtable_heap<ValueType>, vtable_stack<ValueType>>::type;

        static vtable_storage storage {
            vtable_t::type,
            vtable_t::move,
            vtable_t::copy,
            vtable_t::destroy
        };

        return &storage;
    }

    template<class ValueType, class T>
    std::enable_if_t<require_allocation<ValueType>::value>
    construct_storage(ValueType &&val) {
        storage.heap = new T(std::forward<ValueType>(val));            
    }

    template<class ValueType, class T>
    std::enable_if_t<!require_allocation<ValueType>::value>
    construct_storage(ValueType &&val) {
        new (&storage.stack) T(std::forward<ValueType>(val));
    }

    template<class ValueType>
    ValueType *cast() noexcept {
        return require_allocation<std::decay_t<ValueType>>::value ?
                    reinterpret_cast<ValueType *>(storage.heap) :
                    reinterpret_cast<ValueType *>(&storage.stack);
    }

    template<class ValueType>
    const ValueType *cast() const noexcept {
        return require_allocation<std::decay_t<ValueType>>::value ?
                    reinterpret_cast<const ValueType *>(storage.heap) :
                    reinterpret_cast<const ValueType *>(&storage.stack);
    }

public:
    template<class T>
    friend T any_cast(const any &operand);

    template<class T>
    friend T any_cast(any &operand);

    template<class T>
    friend T any_cast(any &&operand);

    template<class T>
    friend const T *any_cast(const any *operand) noexcept;

    template<class T>
    friend T *any_cast(any *operand) noexcept;

public:
    any() noexcept : vtable{nullptr} {}

    any(const any &other) {
        if (other.has_value())
            other.vtable->copy(other.storage, storage);
        vtable = other.vtable;
    }

    any(any &&other) {
        if (other.has_value())
            other.vtable->move(other.storage, storage);
        vtable = other.vtable;
        other.vtable = nullptr;
    }

    template<class ValueType, 
             typename std::enable_if_t<!std::is_same<std::decay_t<ValueType>, any>::value, int> = 0>
    any(ValueType &&value) {
        static_assert(std::is_copy_constructible_v<std::decay_t<ValueType>>, "ValueType shall satisfy the CopyConstructible requirements.");
        vtable = construct_vtable<std::decay_t<ValueType>>();
        construct_storage<ValueType, std::decay_t<ValueType>>(std::forward<ValueType>(value));
    }

    any &operator=(const any &rhs) {
        if (&rhs == this)
            return *this;
        
        any(rhs).swap(*this);
        return *this;
    }

    any &operator=(any &&rhs) {
        if (&rhs == this)
            return *this;

        any(std::move(rhs)).swap(*this);
        return *this;
    }

    template<class ValueType,
             typename std::enable_if_t<!std::is_same_v<std::decay_t<ValueType>, any>, int> = 0>
    any &operator=(ValueType &&rhs) {
        any(std::forward<ValueType>(rhs)).swap(*this);
        return *this;
    }

    ~any() {
        reset();
    }

    const std::type_info &type() const noexcept {
        if (has_value())
            return vtable->type();
        return typeid(void);
    }

    bool has_value() const noexcept {
        return vtable != nullptr;
    }

    void reset() noexcept {
        if (!has_value())
            return;
        
        vtable->destroy(storage);
        vtable = nullptr;
    }

    void swap(any &other) noexcept {
        if (&other == this)
            return;
        
        std::swap(vtable, other.vtable);
        std::swap(storage, other.storage);
    }
};

template<class T>
T any_cast(const any &operand) {
    using U = std::remove_cv_t<std::remove_reference_t<T>>;
    static_assert(std::is_constructible_v<T, const U&>, 
                  "Let U = std::remove_cv_t<std::remove_reference_t<T>>. The program is ill-formed if std::is_constructible_v<T, const U&> is false");

    if (operand.type() != typeid(T))
        throw bad_any_cast();
    
    return static_cast<T>(*any_cast<U>(&operand));
}

template<class T>
T any_cast(any &operand) {
    using U = std::remove_cv_t<std::remove_reference_t<T>>;
    static_assert(std::is_constructible_v<T, U&>, 
                  "Let U = std::remove_cv_t<std::remove_reference_t<T>>. The program is ill-formed if std::is_constructible_v<T, U&> is false");

    if (operand.type() != typeid(T))
        throw bad_any_cast();

    return static_cast<T>(*any_cast<U>(&operand));
}

template<class T>
T any_cast(any &&operand) {
    using U = std::remove_cv_t<std::remove_reference_t<T>>;
    static_assert(std::is_constructible_v<T, U>, 
                  "Let U = std::remove_cv_t<std::remove_reference_t<T>>. The program is ill-formed if std::is_constructible_v<T, U> is false");

    if (operand.type() != typeid(T))
        throw bad_any_cast();

    return static_cast<T>(std::move(*any_cast<U>(&operand)));
}

template<class T>
const T *any_cast(const any *operand) noexcept {
    if (operand == nullptr || operand->type() != typeid(T))
        return nullptr;

    return operand->cast<T>();
}

template<class T>
T *any_cast(any *operand) noexcept {
    if (operand == nullptr || operand->type() != typeid(T))
        return nullptr;
    
    return operand->cast<T>();
}

namespace std {
    template<>
    void swap(any &lhs, any &rhs) {
        lhs.swap(rhs);
    }
}

#endif