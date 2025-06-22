#include "Analyzer.h"

AnalysisSummary Analyzer::getSummary(const std::vector<ECommerceEvent>& events) {
    AnalysisSummary summary;

    for (const auto& event : events) {
        switch (event.eventType) {
        case EventType::VIEW:
            summary.viewCount++;
            break;
        case EventType::CART:
            summary.cartCount++;
            break;
        case EventType::REMOVE_FROM_CART:
            summary.removeCount++;
            break;
        case EventType::PURCHASE:
            summary.purchaseCount++;
            summary.totalRevenue += event.price;
            break;
        case EventType::UNKNOWN:
            break;
        }
    }
    return summary;
}

ProductStatsMap Analyzer::getProductStats(const std::vector<ECommerceEvent>& events) {
    std::unordered_map <uint64_t, std::pair<size_t, size_t>> purchaseMap;

    for (const auto& event : events) {
        switch (event.eventType) {
        case EventType::VIEW:
            purchaseMap[event.prodId].first++;
            break;
        case EventType::PURCHASE:
            purchaseMap[event.prodId].second++;
            break;
        }
    }
    return purchaseMap;
}
