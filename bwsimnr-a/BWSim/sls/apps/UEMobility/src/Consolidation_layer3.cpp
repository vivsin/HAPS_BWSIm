#include "../include/class_layer3.h"
#include <iostream>
#include <vector>
#include <string>
#include <limits>
#include <sstream>
#include <algorithm> // for std::min, std::sort

// Function to reshape measurements for parallel filtering
std::vector<std::vector<double>> reshapeMeasurementsForParallel(const std::vector<std::vector<double>>& values) {
    size_t users = values.size();
    size_t entriesPerUser = values[0].size();
    size_t parallelCount = 32;
    size_t slots = entriesPerUser / parallelCount; // Number of slots (100 in this case)

    std::vector<std::vector<double>> reshapedMeasurements(parallelCount, std::vector<double>(users * slots, 0.0));

    for (size_t userIdx = 0; userIdx < users; ++userIdx) {
        for (size_t slotIdx = 0; slotIdx < slots; ++slotIdx) {
            for (size_t filterIdx = 0; filterIdx < parallelCount; ++filterIdx) {
                reshapedMeasurements[filterIdx][userIdx * slots + slotIdx] = values[userIdx][slotIdx * parallelCount + filterIdx];
            }
        }
    }

    return reshapedMeasurements;
}

std::vector<std::vector<double>> readCSV(const std::string& filename, bool& has_negative) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file" << std::endl;
        return {};
    }

    std::string line;
    std::vector<std::vector<double>> values;
    has_negative = false;

    // Read and parse the CSV file
    while (std::getline(file, line)) {
        std::istringstream ss(line);
        std::string value;
        std::vector<double> row;

        while (std::getline(ss, value, ',')) {
            double num = std::stod(value);
            if (num < 0) {
                has_negative = true;
            }
            row.push_back(num);
        }
        values.push_back(row);
    }
    return values;
}

std::vector<std::vector<double>> filterValues(const std::vector<std::vector<double>>& values, double absThreshSS_BlocksConsolidation_value) {
    std::vector<std::vector<double>> filtered_users;
    for (const auto& user_data : values) {
        std::vector<double> filtered_user_data;
        for (double value : user_data) {
            if (value >= absThreshSS_BlocksConsolidation_value) {
                filtered_user_data.push_back(value);
            }
        }
        filtered_users.push_back(filtered_user_data);
    }
    // Print the number of filtered users with data above the absThreshSS_BlocksConsolidation
    std::cout << "Number of users with data above the absThreshSS_BlocksConsolidation: " << filtered_users.size() << std::endl;
    return filtered_users;
}

std::vector<std::vector<double>> maxNrofRSIndexesToReport_func(const std::vector<std::vector<double>>& filtered_users, size_t maxNrofRSIndexesToReport) {
    std::vector<std::vector<double>> first_entries_from_each_user;
    for (const auto& user_data : filtered_users) {
        std::vector<double> sorted_user_data = user_data;
        std::sort(sorted_user_data.begin(), sorted_user_data.end(), std::greater<double>());
        std::vector<double> first_entries(sorted_user_data.begin(), sorted_user_data.begin() + std::min(maxNrofRSIndexesToReport, sorted_user_data.size()));
        first_entries_from_each_user.push_back(first_entries);
    }
    return first_entries_from_each_user;
}

