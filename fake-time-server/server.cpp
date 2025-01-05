#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <ctime>
#include <httplib.h> // Include cpp-httplib library

class MetricsServer {
public:
    MetricsServer() : svr() {
        svr.Get("/metrics", [this](const httplib::Request& req, httplib::Response& res) {
            std::string metrics = getMetrics();
            res.set_content(metrics, "text/plain");
        });
    }

    void start() {
        std::thread([this]() {
            svr.listen("0.0.0.0", 8080);
        }).detach();
    }

private:
    httplib::Server svr;

    std::string getMetrics() {
        std::string metrics = "# HELP fake_time The fake time metric.\n";
        metrics += "# TYPE fake_time gauge\n";
        metrics += "fake_time " + getFakeTime() + "\n";
        return metrics;
    }

    std::string getFakeTime() {
        std::time_t now = std::time(nullptr);
        std::tm* now_tm = std::localtime(&now);

        // Add 03:56:17 to the current time
        now_tm->tm_hour;// += 3;
        now_tm->tm_min;// += 56;
        now_tm->tm_sec;// += 17;
        std::mktime(now_tm); // Normalize the time structure

        char buffer[80];
        std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", now_tm);

        return std::string(buffer);
    }
};

int main() {
    MetricsServer metricsServer;
    metricsServer.start();

    std::cout << "Metrics server started at http://localhost:8080/metrics" << std::endl;

    // Keep the main thread alive to allow the server to run
    std::this_thread::sleep_for(std::chrono::hours(24));
    return 0;
}


