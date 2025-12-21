#include "unit_system.hpp"
#include <iostream>

using namespace literals;

// 枚举类型用于计数
enum class Product { Apple, Banana, Orange };
using ProductCount = Count<Product>;

int main() {
    std::cout << "=== 1. 基本单位运算 ===\n";
    auto distance1 = 5.0_m;
    auto distance2 = 300.0_cm;  // 3米
    auto total_distance = distance1 + distance2;
    
    std::cout << "距离1: " << distance1 << "\n";
    std::cout << "距离2: " << distance2 << "\n";
    std::cout << "总距离: " << total_distance << "\n";
    
    std::cout << "\n=== 2. 单位转换 ===\n";
    auto speed = 100.0_km / 1.0_h;
    std::cout << "速度: " << speed << " km/h\n";
    
    std::cout << "\n=== 3. 坐标类型 ===\n";
    LengthCoord start(10.0);
    auto displacement = 5.0_m;
    auto end = start + displacement;
    auto distance = end - start;
    
    std::cout << "起点: " << start << "\n";
    std::cout << "位移: " << displacement << "\n";
    std::cout << "终点: " << end << "\n";
    std::cout << "起点到终点距离: " << distance << "\n";
    
    // 编译错误示例（取消注释会报错）：
    // auto wrong = start + start;  // 错误：坐标不能相加
    
    std::cout << "\n=== 4. 计数类型 ===\n";
    ProductCount apple_count(5);
    ProductCount banana_count(3);
    auto total_count = apple_count + banana_count;
    
    auto weight_per_apple = 0.2_kg;
    auto total_weight = apple_count * weight_per_apple;
    
    std::cout << "苹果数量: " << apple_count << "\n";
    std::cout << "香蕉数量: " << banana_count << "\n";
    std::cout << "总数: " << total_count << "\n";
    std::cout << "苹果总重量: " << total_weight << "\n";
    
    std::cout << "\n=== 5. 类型安全验证 ===\n";
    // auto time = 10.0_s;  // removed: unused variable caused a compiler warning
    
    // 下面的代码会编译错误（取消注释）：
    // auto error1 = distance1 + time;      // 错误：不同维度
    // auto error2 = start + start;         // 错误：坐标相加
    // auto error3 = start + end;           // 错误：坐标相加
    
    std::cout << "所有类型检查通过！\n";
    
    return 0;
}