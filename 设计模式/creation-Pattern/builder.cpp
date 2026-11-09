#include <iostream>
#include <string>
#include <memory>

// 产品：Car（汽车）
class Car {
public:
    void setSeats(int _seats) { seats = _seats; }
    void setEngine(const std::string& _engine) { engine = _engine; }
    void setTripComputer(bool _tripComputer) { tripComputer = _tripComputer; }
    void setGPS(bool _gps) { gps = _gps; }

    void display() const {
        std::cout << "Car with " << seats << " seats, "
                  << engine << " engine, "
                  << (tripComputer ? "with trip computer, " : "without trip computer, ")
                  << (gps ? "with GPS." : "without GPS.") << std::endl;
    }

private:
    int seats;
    std::string engine;
    bool tripComputer;
    bool gps;
};

// 产品：Manual（手册）
class Manual {
public:
    void addSeatsInfo(int _seats) { seatsInfo = "Seats: " + std::to_string(_seats); }
    void addEngineInfo(const std::string& _engine) { engineInfo = "Engine: " + _engine; }
    void addTripComputerInfo(bool _tripComputer) { tripComputerInfo = _tripComputer ? "Has trip computer" : "No trip computer"; }
    void addGPSInfo(bool _gps) { gpsInfo = _gps ? "Has GPS" : "No GPS"; }

    void display() const {
        std::cout << seatsInfo << "\n" << engineInfo << "\n" 
                  << tripComputerInfo << "\n" << gpsInfo << std::endl;
    }

private:
    std::string seatsInfo;
    std::string engineInfo;
    std::string tripComputerInfo;
    std::string gpsInfo;
};

// 生成器接口
class Builder {
public:
    virtual void reset() = 0;
    virtual void setSeats(int _seats) = 0;
    virtual void setEngine(const std::string& _engine) = 0;
    virtual void setTripComputer(bool _tripComputer) = 0;
    virtual void setGPS(bool _gps) = 0;
    virtual ~Builder() = default;
};

// 具体生成器类：CarBuilder
class CarBuilder : public Builder {
public:
    CarBuilder() { reset(); }

    void reset() override { _car = std::make_unique<Car>(); }
    void setSeats(int _seats) override { _car->setSeats(_seats); }
    void setEngine(const std::string& _engine) override { _car->setEngine(_engine); }
    void setTripComputer(bool _tripComputer) override { _car->setTripComputer(_tripComputer); }
    void setGPS(bool _gps) override { _car->setGPS(_gps); }

    std::unique_ptr<Car> getProduct() {
        auto result = std::move(_car);
        reset();
        return result;
    }

private:
    std::unique_ptr<Car> _car;
};

// 具体生成器类：CarManualBuilder
class CarManualBuilder : public Builder {
public:
    CarManualBuilder() { reset(); }

    void reset() override { _manual = std::make_unique<Manual>(); }
    void setSeats(int _seats) override { _manual->addSeatsInfo(_seats); }
    void setEngine(const std::string& _engine) override { _manual->addEngineInfo(_engine); }
    void setTripComputer(bool _tripComputer) override { _manual->addTripComputerInfo(_tripComputer); }
    void setGPS(bool _gps) override { _manual->addGPSInfo(_gps); }

    std::unique_ptr<Manual> getProduct() {
        auto result = std::move(_manual);
        reset();
        return result;
    }

private:
    std::unique_ptr<Manual> _manual;
};

// 主管：Director
class Director {
public:
    void constructSportsCar(Builder& builder) {
        builder.reset();
        builder.setSeats(2);
        builder.setEngine("Sport engine");
        builder.setTripComputer(true);
        builder.setGPS(true);
    }

    void constructSUV(Builder& builder) {
        builder.reset();
        builder.setSeats(5);
        builder.setEngine("SUV engine");
        builder.setTripComputer(true);
        builder.setGPS(false);
    }
};

// 客户端：Application
class Application {
public:
    void makeCar() {
        Director director;

        CarBuilder carBuilder;
        director.constructSportsCar(carBuilder);
        std::unique_ptr<Car> _car = carBuilder.getProduct();
        _car->display();

        CarManualBuilder manualBuilder;
        director.constructSportsCar(manualBuilder);
        std::unique_ptr<Manual> _manual = manualBuilder.getProduct();
        _manual->display();
    }
};

// 主函数
int main() {
    Application app;
    app.makeCar();

    return 0;
}
