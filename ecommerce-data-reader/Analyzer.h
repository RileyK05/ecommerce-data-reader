#pragma once

#include "DataStructure.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <utility>

struct AnalysisSummary {
    double totalRevenue = 0.0;
    size_t viewCount = 0;
    size_t cartCount = 0;
    size_t removeCount = 0;
    size_t purchaseCount = 0;
};

using ProductStatsMap = std::unordered_map<uint64_t, std::pair<size_t, size_t>>;


class Analyzer {
public:
    AnalysisSummary getSummary(const std::vector<ECommerceEvent>& events);
    ProductStatsMap getProductStats(const std::vector<ECommerceEvent>& events);
};
