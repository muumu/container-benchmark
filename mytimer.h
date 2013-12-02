#ifndef MYTIMER_H
#define MYTIMER_H

#include <sys/time.h>
#include <vector>
#include <string>
#include <iostream>

class MyTimer {
    std::pair<std::string, double> current_data;
    std::vector<std::pair<std::string, double>> records;
    struct timeval start_timeval, end_timeval;
public:
    void start(const std::string& name) {
        current_data.first = name;
        current_data.second = 0.0;
        gettimeofday(&start_timeval, NULL);
    }
    void pause() {
        gettimeofday(&end_timeval, NULL);
        current_data.second += getTimeDiff();
    }
    void resume() {
        gettimeofday(&start_timeval, NULL);
    }
    void stop() {
        gettimeofday(&end_timeval, NULL);
        current_data.second += getTimeDiff();
        records.push_back(current_data);
    }
    void showResults() {
        double total = 0.0;
        for(auto& record : records) {
            std::cout << record.first << " : " << record.second << " [s]" << std::endl;
            total += record.second;
        }
        std::cout << "total : " << total << " [s]" << std::endl;
    }
private:
    double getTimeDiff() {
        return (end_timeval.tv_sec - start_timeval.tv_sec)
            + (end_timeval.tv_usec - start_timeval.tv_usec) / 1000000.0;
    }

};

#endif // MYTIMER_H
