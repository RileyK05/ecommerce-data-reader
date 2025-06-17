#pragma once
#include <iostream>
#include <chrono>


struct PurchaseTime {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
};

struct CategoryCode {
	std::string_view code;
	std::string_view subcode;
	std::string_view secondarySubcode;
};

enum class EventType {
	VIEW,
	CART,
	REMOVE_FROM_CART,
	PURCHASE,
	UNKNOWN
};

struct ECommerceEvent {
	PurchaseTime purchaseTime;
	EventType eventType;
	uint64_t prodId;
	uint64_t categoryId;
	CategoryCode categoryCode;
	std::string_view brand;
	double price;
	uint64_t userId;
	std::string_view userSession;
};

