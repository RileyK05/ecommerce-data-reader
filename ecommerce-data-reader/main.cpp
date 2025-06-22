#include "Parser.h"
#include "Analyzer.h"
#include "DataStructure.h"

#include <iostream>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <iomanip>

void printSummary(const AnalysisSummary& summary) {
    std::cout << "--- Analysis Summary ---" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "  Total Revenue:          $" << summary.totalRevenue << std::endl;
    std::cout << "  Total View Events:        " << summary.viewCount << std::endl;
    std::cout << "  Total Cart Events:        " << summary.cartCount << std::endl;
    std::cout << "  Total Remove Cart Events: " << summary.removeCount << std::endl;
    std::cout << "  Total Purchase Events:    " << summary.purchaseCount << std::endl;
    std::cout << "--------------------------" << std::endl;
}

void printTopProducts(const ProductStatsMap& stats, size_t topN) {
    std::cout << "\n--- Top " << topN << " Products by Purchase-to-View Rate ---" << std::endl;

    std::vector<std::pair<double, uint64_t>> productsByRate;
    for (const auto& pair : stats) {
        uint64_t productId = pair.first;
        size_t views = pair.second.first;
        size_t purchases = pair.second.second;

        if (views > 100 && purchases > 10) { // Only consider products with significant activity
            double conversionRate = static_cast<double>(purchases) / views * 100.0;
            productsByRate.push_back({ conversionRate, productId });
        }
    }

    // Sort products by conversion rate in descending order
    std::sort(productsByRate.rbegin(), productsByRate.rend());

    std::cout << std::left << std::setw(15) << "Product ID"
        << std::setw(15) << "Views"
        << std::setw(15) << "Purchases"
        << std::setw(15) << "Conv. Rate (%)" << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;

    size_t count = 0;
    for (const auto& product : productsByRate) {
        if (count >= topN) break;
        uint64_t productId = product.second;
        const auto& productStats = stats.at(productId);
        std::cout << std::left << std::setw(15) << productId
            << std::setw(15) << productStats.first
            << std::setw(15) << productStats.second
            << std::fixed << std::setprecision(4) << product.first << "%" << std::endl;
        count++;
    }
    std::cout << "------------------------------------------------------------" << std::endl;
}


int main() {
    // --- 1. Parsing Stage ---
    Parser parser;
    parser.runUnitTests();

    std::cout << "--- Running Performance Test ---" << std::endl;
    const std::string filePath = "2019-Nov.csv";

    std::cout << "Processing file: " << filePath << std::endl;

    auto start = std::chrono::high_resolution_clock::now();
    parser.parseFile(filePath);
    auto end = std::chrono::high_resolution_clock::now();

    const auto& events = parser.getEventVector();
    std::chrono::duration<double> duration = end - start;

    std::cout << "\nPerformance Results:" << std::endl;
    std::cout << "  Parsed " << events.size() << " valid records in " << duration.count() << " seconds." << std::endl;
    if (duration.count() > 0.0) {
        std::cout << "  Processing speed: " << (static_cast<double>(events.size()) / 1'000'000.0) / duration.count()
            << " million records/sec." << std::endl;
    }
    std::cout << "------------------------------" << std::endl << std::endl;

    // --- 2. Analysis Stage ---
    Analyzer analyzer;

    auto analysisStart = std::chrono::high_resolution_clock::now();

    AnalysisSummary summary = analyzer.getSummary(events);
    ProductStatsMap productStats = analyzer.getProductStats(events);

    auto analysisEnd = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> analysisDuration = analysisEnd - analysisStart;

    std::cout << "Analysis phase took " << analysisDuration.count() << " seconds." << std::endl;

    // --- 3. Output Stage ---
    printSummary(summary);
    printTopProducts(productStats, 10);

    return EXIT_SUCCESS;
}
