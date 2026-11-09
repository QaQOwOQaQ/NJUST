#include <iostream>
#include <cmath>

// 圆孔类
class RoundHole {
private:
    double radius;

public:
    explicit RoundHole(double r) : radius(r) {}

    double getRadius() const {
        return radius;
    }

    bool fits(const class RoundPeg& peg) const;
};

// 圆钉类
class RoundPeg {
protected:
    double radius;

public:
    explicit RoundPeg(double r) : radius(r) {}

    virtual double getRadius() const {
        return radius;
    }
};

// 让 RoundHole 可以判断 RoundPeg 是否适配
bool RoundHole::fits(const RoundPeg& peg) const {
    return this->getRadius() >= peg.getRadius();
}

// 方钉类
class SquarePeg {
private:
    double width;

public:
    explicit SquarePeg(double w) : width(w) {}

    double getWidth() const {
        return width;
    }
};


// 适配器：让 SquarePeg 兼容 RoundPeg
// 适配器假扮成一个圆钉，其半径等于方钉横截面对角线的一半，即能够容纳方钉的最小外接圆的半径
// 之所以继承自 RoundPeg 类是因为需要使适配器可以替代 RoundPeg 的实例从而实现多态，这样适配器便可以当作 RoundPeg 使用
// 即，在适配器的加持下，我们可以将方钉作为圆钉
// 而包含 SquarePeg 的原因就简单了，因为 getRadius 方法需要用到 SquarePeg 的成员变量 width
class SquarePegAdapter : public RoundPeg {
private:
    SquarePeg peg;

public:
    explicit SquarePegAdapter(SquarePeg p)
        : RoundPeg(p.getWidth() * std::sqrt(2) / 2), peg(p) {}

    double getRadius() const override {
        return peg.getWidth() * std::sqrt(2) / 2;
    }
};

// 测试代码
int main() {
    RoundHole hole(5);
    RoundPeg rpeg(5);
    std::cout << std::boolalpha << "RoundPeg(5) fits in RoundHole(5): " << hole.fits(rpeg) << "\n"; // true

    SquarePeg small_sqpeg(5);
    SquarePeg large_sqpeg(10);

    // 直接使用 SquarePeg 会报错（没有 getRadius() 方法）
    // hole.fits(small_sqpeg); // ❌ 无法编译！

    SquarePegAdapter small_sqpeg_adapter(small_sqpeg);
    SquarePegAdapter large_sqpeg_adapter(large_sqpeg);

    std::cout << "SquarePeg(5) fits in RoundHole(5): " << hole.fits(small_sqpeg_adapter) << "\n"; // true
    std::cout << "SquarePeg(10) fits in RoundHole(5): " << hole.fits(large_sqpeg_adapter) << "\n"; // false

    return 0;
}
