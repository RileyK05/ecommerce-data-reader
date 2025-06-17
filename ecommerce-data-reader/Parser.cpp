#include "Parser.h"
#include "DataStructure.h"
#include "mio.hpp"

#include <iostream>
#include <array>
#include <charconv>
#include <vector>

namespace {

    template<typename T>
    void parseNumeric(T& outValue, std::string_view fieldView) {
        if (fieldView.empty()) {
            outValue = 0;
            return;
        }

        auto result = std::from_chars(fieldView.data(), fieldView.data() + fieldView.size(), outValue);

        if (result.ec != std::errc()) {
            outValue = 0;
        }
    }

    EventType parseEventType(std::string_view eventStr) {
        if (eventStr == "view") return EventType::VIEW;
        if (eventStr == "cart") return EventType::CART;
        if (eventStr == "remove_from_cart") return EventType::REMOVE_FROM_CART;
        if (eventStr == "purchase") return EventType::PURCHASE;
        return EventType::UNKNOWN;
    }

    void parseTimestamp(PurchaseTime& outTime, std::string_view timeView) {
        if (timeView.length() < 19) {
            outTime = { 0,0,0,0,0,0 };
            return;
        }
        parseNumeric(outTime.year, timeView.substr(0, 4));
        parseNumeric(outTime.month, timeView.substr(5, 2));
        parseNumeric(outTime.day, timeView.substr(8, 2));
        parseNumeric(outTime.hour, timeView.substr(11, 2));
        parseNumeric(outTime.minute, timeView.substr(14, 2));
        parseNumeric(outTime.second, timeView.substr(17, 2));
    }

    void parseCategoryCode(CategoryCode& outCode, std::string_view catCodeStr) {
        outCode = { "", "", "" };

        std::vector<std::string_view> tempCodeStore;
        tempCodeStore.reserve(3);

        while (!catCodeStr.empty()) {
            size_t nextDot = catCodeStr.find('.');
            if (nextDot == std::string_view::npos) {
                tempCodeStore.emplace_back(catCodeStr);
                catCodeStr.remove_prefix(catCodeStr.size());
            }
            else {
                tempCodeStore.emplace_back(catCodeStr.substr(0, nextDot));
                catCodeStr.remove_prefix(nextDot + 1);
            }
        }

        if (tempCodeStore.size() > 0) {
            outCode.code = tempCodeStore[0];
        }
        if (tempCodeStore.size() > 1) {
            outCode.subcode = tempCodeStore[1];
        }
        if (tempCodeStore.size() > 2) {
            outCode.secondarySubcode = tempCodeStore[2];
        }
    }

    bool isEventValid(const ECommerceEvent& event) {
        return event.price >= 0.0 &&
            event.eventType != EventType::UNKNOWN &&
            event.prodId != 0 &&
            event.userId != 0;
    }

}
  
const size_t ALLOC_NEEDED = 70000000;

Parser::Parser() {
    try {
        eventVector.reserve(ALLOC_NEEDED);
    }
    catch (const std::bad_alloc& e) {
        std::cerr << "FATAL ERROR: Failed to allocate required memory ("
            << (ALLOC_NEEDED * sizeof(ECommerceEvent)) / (1024 * 1024) << " MB)."
            << std::endl;
        std::cerr << "  Exception details: " << e.what() << std::endl;
        std::cerr << "  This can happen if you do not have enough available RAM, "
            << "or if you are running a 32-bit version of this program "
            << "which cannot address this much memory." << std::endl;

        exit(EXIT_FAILURE);
    }
}

void Parser::parseFile(const std::string& fileName) {
    try {
        mio::mmap_source data(fileName);
        std::string_view dataView(data.data(), data.size());

        size_t firstNewline = dataView.find('\n');
        if (firstNewline != std::string_view::npos) {
            dataView.remove_prefix(firstNewline + 1);
        }

        while (!dataView.empty()) {
            size_t nextNewline = dataView.find('\n');
            std::string_view line;

            if (nextNewline == std::string_view::npos) {
                line = dataView;
                dataView.remove_prefix(dataView.size());
            }
            else {
                line = dataView.substr(0, nextNewline);
                dataView.remove_prefix(nextNewline + 1);
            }

            if (!line.empty() && line.back() == '\r') {
                line.remove_suffix(1);
            }

            if (line.empty()) continue;

            const size_t NUM_COLUMNS = 9;
            std::array<std::string_view, NUM_COLUMNS> fields;
            int fieldIndex = 0;
            std::string_view lineView = line;
            while (!lineView.empty() && fieldIndex < NUM_COLUMNS) {
                size_t nextComma = lineView.find(',');
                if (nextComma == std::string_view::npos) {
                    fields[fieldIndex] = lineView;
                    lineView.remove_prefix(lineView.size());
                }
                else {
                    fields[fieldIndex] = lineView.substr(0, nextComma);
                    lineView.remove_prefix(nextComma + 1);
                }
                fieldIndex++;
            }

            ECommerceEvent event;

            parseTimestamp(event.purchaseTime, fields[0]);
            event.eventType = parseEventType(fields[1]);
            parseNumeric(event.prodId, fields[2]);
            parseNumeric(event.categoryId, fields[3]);
            parseCategoryCode(event.categoryCode, fields[4]);
            event.brand = fields[5];
            parseNumeric(event.price, fields[6]);
            parseNumeric(event.userId, fields[7]);
            event.userSession = fields[8];

            if (isEventValid(event)) {
                eventVector.emplace_back(std::move(event));
            }
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Parser error: " << e.what() << std::endl;
    }
}
