// unit_system.hpp
#pragma once
#include <iostream>
#include <type_traits>
#include <ratio>
#include <cmath>
#include <string>
#include <concepts>
#include <cstdint>

// ==================== 维度系统 ====================
namespace dimension {
    // 基本维度标签
    struct Length {};
    struct Time {};
    struct Mass {};
    struct Currency {};
    struct Count {};
    
    // 维度组合（幂次）
    template<typename Dim, int Power = 1>
    struct DimensionPower {
        using dimension = Dim;
        static constexpr int power = Power;
    };
    
    // 无维度
    struct Dimensionless {};
    
    // 检查是否为相同维度
    template<typename D1, typename D2>
    struct SameDimension : std::false_type {};
    
    template<typename D>
    struct SameDimension<D, D> : std::true_type {};
    
    template<typename D, int P1, int P2>
    struct SameDimension<DimensionPower<D, P1>, DimensionPower<D, P2>> : std::true_type {};
}

// ==================== 单位系统 ====================
namespace unit {
    using namespace dimension;
    
    // 单位基类
    template<typename Dim, typename Scale = std::ratio<1>, bool IsCoord = false>
    struct Unit {
        using dimension = Dim;
        using scale = Scale;
        static constexpr bool is_coordinate = IsCoord;
        static constexpr double scale_factor = 
            static_cast<double>(Scale::num) / Scale::den;
        
        static std::string name() {
            if constexpr (IsCoord) {
                return "coord_" + std::to_string(scale_factor);
            }
            return "unit_" + std::to_string(scale_factor);
        }
    };
    
    // 预定义单位
    namespace length {
        using Meter = Unit<Length>;
        using Kilometer = Unit<Length, std::kilo>;
        using Centimeter = Unit<Length, std::centi>;
        using Millimeter = Unit<Length, std::milli>;
    }
    
    namespace time {
        using Second = Unit<Time>;
        using Minute = Unit<Time, std::ratio<60>>;
        using Hour = Unit<Time, std::ratio<3600>>;
        using Day = Unit<Time, std::ratio<86400>>;
    }
    
    namespace mass {
        using Kilogram = Unit<Mass>;
        using Gram = Unit<Mass, std::milli>;
        using Ton = Unit<Mass, std::ratio<1000>>;
    }
    
    namespace currency {
        using Dollar = Unit<Currency>;
        using Cent = Unit<Currency, std::centi>;
    }
    
    // 坐标单位
    template<typename BaseUnit>
    using Coordinate = Unit<typename BaseUnit::dimension, typename BaseUnit::scale, true>;
}

// ==================== 量值类 ====================
template<typename U, typename T = double>
class Quantity {
    T value_;
    
public:
    using unit_t = U; // avoid shadowing namespace 'unit'
    using value_type = T;
    using dimension = typename U::dimension;
    
    // 构造函数
    constexpr Quantity() noexcept : value_(0) {}
    constexpr explicit Quantity(T val) noexcept : value_(val) {}
    
    // 禁止隐式转换
    template<typename U2, typename T2>
    explicit constexpr Quantity(const Quantity<U2, T2>& other) 
        : value_(other.template convert_to<U>().value()) {}
    
    // 获取值
    constexpr T value() const noexcept { return value_; }
    constexpr T raw_value() const noexcept { return value_; }
    
    // 单位转换
    template<typename TargetUnit>
    constexpr Quantity<TargetUnit, T> convert_to() const {
        using Conv = std::ratio_divide<typename U::scale, typename TargetUnit::scale>;
        static_assert(
            std::is_same_v<dimension, typename TargetUnit::dimension>,
            "Cannot convert between different dimensions"
        );
        
        T new_value = value_ * 
            static_cast<T>(Conv::num) / Conv::den;
        return Quantity<TargetUnit, T>(new_value);
    }
    
    // 类型安全的算术运算
    
    // 加法（自动向下转换）
    template<typename U2>
    constexpr auto operator+(const Quantity<U2, T>& other) const {
        static_assert(
            std::is_same_v<dimension, typename U2::dimension>,
            "Cannot add quantities of different dimensions"
        );
        static_assert(
            !U::is_coordinate && !U2::is_coordinate,
            "Cannot add coordinate quantities directly"
        );
        
        // 自动转换为较小单位
        if constexpr (U::scale_factor <= U2::scale_factor) {
            auto converted = other.template convert_to<U>();
            return Quantity<U, T>(value_ + converted.value());
        } else {
            auto converted = this->template convert_to<U2>();
            return Quantity<U2, T>(converted.value() + other.value());
        }
    }
    
    // 减法
    template<typename U2>
    constexpr auto operator-(const Quantity<U2, T>& other) const {
        static_assert(
            std::is_same_v<dimension, typename U2::dimension>,
            "Cannot subtract quantities of different dimensions"
        );
        static_assert(
            !U::is_coordinate && !U2::is_coordinate,
            "Cannot subtract coordinate quantities directly"
        );
        
        if constexpr (U::scale_factor <= U2::scale_factor) {
            auto converted = other.template convert_to<U>();
            return Quantity<U, T>(value_ - converted.value());
        } else {
            auto converted = this->template convert_to<U2>();
            return Quantity<U2, T>(converted.value() - other.value());
        }
    }
    
    // 标量乘法
    template<typename Scalar> requires std::is_arithmetic_v<Scalar>
    constexpr auto operator*(Scalar scalar) const {
        return Quantity<U, T>(value_ * static_cast<T>(scalar));
    }
    
    // 标量除法
    template<typename Scalar> requires std::is_arithmetic_v<Scalar>
    constexpr auto operator/(Scalar scalar) const {
        return Quantity<U, T>(value_ / static_cast<T>(scalar));
    }
    
    // 单位乘法（生成新单位）
    template<typename U2>
    constexpr auto operator*(const Quantity<U2, T>& other) const {
        // 返回一个组合量（简化版，返回double）
        return value_ * other.value();
    }
    
    // 单位除法
    template<typename U2>
    constexpr auto operator/(const Quantity<U2, T>& other) const {
        return value_ / other.value();
    }
    
    // 比较运算
    template<typename U2>
    constexpr bool operator==(const Quantity<U2, T>& other) const {
        if constexpr (U::scale_factor <= U2::scale_factor) {
            auto converted = other.template convert_to<U>();
            return std::abs(value_ - converted.value()) < 1e-10;
        } else {
            auto converted = this->template convert_to<U2>();
            return std::abs(converted.value() - other.value()) < 1e-10;
        }
    }
    
    template<typename U2>
    constexpr bool operator!=(const Quantity<U2, T>& other) const {
        return !(*this == other);
    }
    
    // 输出
    friend std::ostream& operator<<(std::ostream& os, const Quantity& q) {
        if constexpr (U::is_coordinate) {
            os << "Coord(" << q.value_ << ")";
        } else {
            os << q.value_;
        }

        // 显示单位信息
        using dim_t = typename U::dimension;
        if constexpr (std::is_same_v<dim_t, ::dimension::Length>) {
            if constexpr (std::is_same_v<U, unit::length::Meter>) os << " m";
            else if constexpr (std::is_same_v<U, unit::length::Kilometer>) os << " km";
            else if constexpr (std::is_same_v<U, unit::length::Centimeter>) os << " cm";
            else os << " [length]";
        }
        else if constexpr (std::is_same_v<dim_t, ::dimension::Time>) {
            if constexpr (std::is_same_v<U, unit::time::Second>) os << " s";
            else if constexpr (std::is_same_v<U, unit::time::Minute>) os << " min";
            else if constexpr (std::is_same_v<U, unit::time::Hour>) os << " h";
            else os << " [time]";
        }
        else if constexpr (std::is_same_v<dim_t, ::dimension::Mass>) {
            if constexpr (std::is_same_v<U, unit::mass::Kilogram>) os << " kg";
            else if constexpr (std::is_same_v<U, unit::mass::Gram>) os << " g";
            else os << " [mass]";
        }
        else {
            os << " [?]";
        }

        return os;
    }
};

// ==================== 坐标类 ====================
template<typename U, typename T = double>
class Coordinate {
    T value_;
    
public:
    using unit = U;
    using value_type = T;
    static_assert(U::is_coordinate, "Coordinate must use coordinate unit");
    
    constexpr Coordinate(T val = 0) noexcept : value_(val) {}
    
    // 获取值
    constexpr T value() const noexcept { return value_; }
    
    // 坐标 + 差值 = 新坐标
    template<typename DeltaUnit>
    constexpr Coordinate operator+(const Quantity<DeltaUnit, T>& delta) const {
        static_assert(
            std::is_same_v<typename U::dimension, typename DeltaUnit::dimension>,
            "Coordinate and delta must have same dimension"
        );
        static_assert(!DeltaUnit::is_coordinate, "Delta must not be coordinate");
        
        auto delta_converted = delta.template convert_to<U>();
        return Coordinate(value_ + delta_converted.value());
    }
    
    // 坐标 - 差值 = 新坐标
    template<typename DeltaUnit>
    constexpr Coordinate operator-(const Quantity<DeltaUnit, T>& delta) const {
        static_assert(
            std::is_same_v<typename U::dimension, typename DeltaUnit::dimension>,
            "Coordinate and delta must have same dimension"
        );
        static_assert(!DeltaUnit::is_coordinate, "Delta must not be coordinate");
        
        auto delta_converted = delta.template convert_to<U>();
        return Coordinate(value_ - delta_converted.value());
    }
    
    // 坐标 - 坐标 = 差值
    constexpr auto operator-(const Coordinate& other) const {
        using DeltaUnit = ::unit::Unit<typename U::dimension, typename U::scale, false>;
        return Quantity<DeltaUnit, T>(value_ - other.value_);
    }
    
    // 禁止坐标相加
    template<typename U2>
    constexpr auto operator+(const Coordinate<U2, T>&) const = delete;
    
    // 比较运算
    constexpr bool operator==(const Coordinate& other) const {
        return std::abs(value_ - other.value_) < 1e-10;
    }
    
    constexpr bool operator!=(const Coordinate& other) const {
        return !(*this == other);
    }
    
    constexpr bool operator<(const Coordinate& other) const {
        return value_ < other.value_;
    }
    
    // 输出
    friend std::ostream& operator<<(std::ostream& os, const Coordinate& c) {
        os << "Coord(" << c.value_ << ")";
        if constexpr (std::is_same_v<typename U::dimension, dimension::Length>) {
            os << " m";
        } else if constexpr (std::is_same_v<typename U::dimension, dimension::Time>) {
            os << " s";
        }
        return os;
    }
};

// ==================== 计数类型 ====================
template<typename ItemType>
class Count {
    int64_t value_;
    
public:
    using item_type = ItemType;
    
    constexpr Count(int64_t val = 0) noexcept : value_(val) {}
    
    // 获取值
    constexpr int64_t value() const noexcept { return value_; }
    
    // 算术运算
    constexpr Count operator+(Count other) const {
        return Count(value_ + other.value_);
    }
    
    constexpr Count operator-(Count other) const {
        return Count(value_ - other.value_);
    }
    
    constexpr Count operator*(int64_t scalar) const {
        return Count(value_ * scalar);
    }
    
    // 计数 * (量/物品) = 总量
    template<typename U, typename T>
    constexpr auto operator*(const Quantity<U, T>& per_item) const {
        // 这里简化处理，实际应该返回组合类型
        return Quantity<U, T>(static_cast<T>(value_) * per_item.value());
    }
    
    // 输出
    friend std::ostream& operator<<(std::ostream& os, const Count& c) {
        return os << c.value_ << " items";
    }
};

// ==================== 用户字面量 ====================
namespace literals {
    // 长度
    constexpr Quantity<unit::length::Meter> operator""_m(long double val) {
        return Quantity<unit::length::Meter>(static_cast<double>(val));
    }
    
    constexpr Quantity<unit::length::Kilometer> operator""_km(long double val) {
        return Quantity<unit::length::Kilometer>(static_cast<double>(val));
    }
    
    constexpr Quantity<unit::length::Centimeter> operator""_cm(long double val) {
        return Quantity<unit::length::Centimeter>(static_cast<double>(val));
    }
    
    // 时间
    constexpr Quantity<unit::time::Second> operator""_s(long double val) {
        return Quantity<unit::time::Second>(static_cast<double>(val));
    }
    
    constexpr Quantity<unit::time::Minute> operator""_min(long double val) {
        return Quantity<unit::time::Minute>(static_cast<double>(val));
    }
    
    constexpr Quantity<unit::time::Hour> operator""_h(long double val) {
        return Quantity<unit::time::Hour>(static_cast<double>(val));
    }
    
    // 质量
    constexpr Quantity<unit::mass::Kilogram> operator""_kg(long double val) {
        return Quantity<unit::mass::Kilogram>(static_cast<double>(val));
    }
    
    constexpr Quantity<unit::mass::Gram> operator""_g(long double val) {
        return Quantity<unit::mass::Gram>(static_cast<double>(val));
    }
}

// ==================== 类型别名 ====================
using Meter = Quantity<unit::length::Meter>;
using Kilometer = Quantity<unit::length::Kilometer>;
using Centimeter = Quantity<unit::length::Centimeter>;

using Second = Quantity<unit::time::Second>;
using Minute = Quantity<unit::time::Minute>;
using Hour = Quantity<unit::time::Hour>;

using Kilogram = Quantity<unit::mass::Kilogram>;
using Gram = Quantity<unit::mass::Gram>;

using TimeCoord = Coordinate<unit::Coordinate<unit::time::Second>>;
using LengthCoord = Coordinate<unit::Coordinate<unit::length::Meter>>;
