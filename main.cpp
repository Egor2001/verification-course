#include <bits/stdc++.h>
#include <gtest/gtest.h>

struct Book {
	std::string name;
	std::string author;
	std::string category;
	std::string label;
	int year;
	unsigned int price;
};

struct Cart {
public:
	bool AddBook(int book_id) {
		return false;
	}
};

enum class PaymentOption {
	kUponOrder, kOnReceipt,
};

struct DeliveryOrder {
};

enum class ReturnOption {
	kByCourier, kBySelf, kCancel,
};

struct ReturnOrder {
};

using OrderID = int;

struct Customer {
public:
	DeliveryOrder CreateDeliveryOrder(Cart cart, std::string address, std::chrono::year_month_day date, PaymentOption option) {
		return {};
	}

	ReturnOrder CreateReturnRequest(OrderID id, ReturnOption option) {
		return {};
	}

	unsigned GetBalance() {
		return 0;
	}
};

struct Store {
public:
	enum class DeliveryStatus {
		kCreated, kWaiting, kDelivered, kCancelled,
	};

public:
	static std::shared_ptr<Store> Create() {
		return std::make_shared<Store>();
	}

	std::shared_ptr<Customer> CreateCustomer(unsigned balance) {
		return std::make_shared<Customer>();
	}

	int AddBook(const Book& book) {
		return 0;
	}

	std::optional<DeliveryStatus> CheckStatus(OrderID id) {
		return std::nullopt;
	}

	std::optional<OrderID> RegisterDelivery(DeliveryOrder order) {
		return std::nullopt;
	}

	bool Deliver(OrderID id) {
		return false;
	}

	bool Return(ReturnOrder order) {
		return false;
	}

	unsigned GetProfit() {
		return 0;
	}
};

std::vector<Book> GetTestBooks() {
	return {
		{.name = "A", .author="xxx", .category="fantasy", .label="l", .year=1000, .price=3},
		{.name = "B", .author="yyy", .category="memoirs", .label="m", .year=2000, .price=4},
		{.name = "C", .author="zzz", .category="study",   .label="n", .year=3000, .price=5},
	};
}

TEST(Main, Init) {
	auto store = Store::Create();
	auto books = GetTestBooks();
	Book book_a = books[0], book_b = books[1], book_c = books[2];
	int id_a = 0, id_b = 0, id_c = 0;

	id_a = store->AddBook(book_a);
	ASSERT_EQ(store->AddBook(book_a), id_a);
	ASSERT_EQ(store->AddBook(book_a), id_a);

	id_b = store->AddBook(book_b);
	ASSERT_EQ(store->AddBook(book_b), id_b);

	id_c = store->AddBook(book_c);

	ASSERT_NE(id_a, id_b);
	ASSERT_NE(id_a, id_c);
	ASSERT_NE(id_b, id_c);

	ASSERT_EQ(store->GetProfit(), 0);
}

TEST(Main, FailedOrder) {
	auto store = Store::Create();
	auto books = GetTestBooks();
	int id_a = store->AddBook(books[0]);
	int id_b = store->AddBook(books[1]);
	int id_c = store->AddBook(books[2]);
	auto cust = store->CreateCustomer(10);
	auto cart = Cart{};
	cart.AddBook(id_a);
	cart.AddBook(id_b);
	cart.AddBook(id_c);
	auto time = std::chrono::year_month_day{std::chrono::year(2024), std::chrono::June, std::chrono::day{30}};
	auto order1 = cust->CreateDeliveryOrder(cart, "loc", time, PaymentOption::kUponOrder);
	auto order2 = cust->CreateDeliveryOrder(cart, "loc", time, PaymentOption::kOnReceipt);
	auto p_order1_id = store->RegisterDelivery(order1);
	auto p_order2_id = store->RegisterDelivery(order2);
	ASSERT_EQ(p_order1_id, std::nullopt);
	ASSERT_NE(p_order2_id, std::nullopt);
	ASSERT_FALSE(store->Deliver(*p_order2_id));
}

TEST(Main, SimpleOrder) {
	auto store = Store::Create();
	auto books = GetTestBooks();
	int id_a = store->AddBook(books[0]);
	int id_b = store->AddBook(books[1]);
	int id_c = store->AddBook(books[2]);
	auto cust = store->CreateCustomer(42);
	auto cart1 = Cart{};
	auto cart2 = Cart{};
	cart1.AddBook(id_a);
	cart1.AddBook(id_b);
	cart2.AddBook(id_c);
	auto time = std::chrono::year_month_day{std::chrono::year(2024), std::chrono::June, std::chrono::day{30}};
	auto order1 = cust->CreateDeliveryOrder(cart1, "loc", time, PaymentOption::kOnReceipt);
	auto order2 = cust->CreateDeliveryOrder(cart2, "loc", time, PaymentOption::kUponOrder);
	auto p_order1_id = store->RegisterDelivery(order1);
	auto p_order2_id = store->RegisterDelivery(order2);
	ASSERT_NE(p_order1_id, std::nullopt);
	ASSERT_NE(p_order2_id, std::nullopt);
	ASSERT_EQ(store->GetProfit(), 5);
	ASSERT_TRUE(store->Deliver(*p_order1_id));
	ASSERT_TRUE(store->Deliver(*p_order2_id));
	ASSERT_EQ(store->GetProfit(), 12);
}

TEST(Main, ComplexOrder) {
	auto store = Store::Create();
	auto books = GetTestBooks();
	int id_a = store->AddBook(books[0]);
	int id_b = store->AddBook(books[1]);
	int id_c = store->AddBook(books[2]);
	auto cust = store->CreateCustomer(42);
	Cart cart_a = {}, cart_b = {}, cart_c = {};
	cart_a.AddBook(id_a);
	cart_b.AddBook(id_b);
	cart_c.AddBook(id_c);

	auto time = std::chrono::year_month_day{std::chrono::year(2024), std::chrono::June, std::chrono::day{30}};
	auto order_a = cust->CreateDeliveryOrder(cart_a, "loc", time, PaymentOption::kUponOrder);
	auto order_b = cust->CreateDeliveryOrder(cart_b, "loc", time, PaymentOption::kOnReceipt);
	auto order_c = cust->CreateDeliveryOrder(cart_c, "loc", time, PaymentOption::kOnReceipt);
	auto p_order_a_id = store->RegisterDelivery(order_a);
	auto p_order_b_id = store->RegisterDelivery(order_b);
	auto p_order_c_id = store->RegisterDelivery(order_c);
	ASSERT_NE(p_order_a_id, std::nullopt);
	ASSERT_NE(p_order_b_id, std::nullopt);
	ASSERT_NE(p_order_c_id, std::nullopt);

	auto req_a = cust->CreateReturnRequest(*p_order_a_id, ReturnOption::kCancel);
	auto req_b = cust->CreateReturnRequest(*p_order_b_id, ReturnOption::kBySelf);
	auto req_c = cust->CreateReturnRequest(*p_order_c_id, ReturnOption::kByCourier);

	ASSERT_TRUE(store->Return(req_a));
	ASSERT_FALSE(store->Return(req_b));
	ASSERT_FALSE(store->Return(req_c));

	ASSERT_FALSE(store->Deliver(*p_order_a_id));
	ASSERT_TRUE(store->Deliver(*p_order_b_id));
	ASSERT_TRUE(store->Deliver(*p_order_c_id));

	ASSERT_TRUE(store->Return(req_b));
	ASSERT_TRUE(store->Return(req_c));
}

int main(int argc, char **argv) {
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
