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

bool operator==(const Book& lhs, const Book& rhs) {
	return
		std::tuple{lhs.name, lhs.author, lhs.category, lhs.label, lhs.year} ==
		std::tuple{rhs.name, rhs.author, rhs.category, rhs.label, rhs.year};
}

bool operator!=(const Book& lhs, const Book& rhs) {
	return !(lhs == rhs);
}

struct Cart {
public:
	bool AddBook(int book_id) {
		if (std::find(book_ids.begin(), book_ids.end(), book_id) != book_ids.end()) {
			return false;
		}
		book_ids.push_back(book_id);
		return true;
	}

	const std::vector<int>& GetAll() const {
		return book_ids;
	}

private:
	std::vector<int> book_ids;
};

using OrderID = int;

enum class PaymentOption {
	kUponOrder, kOnReceipt,
};

struct DeliveryOrder {
	int cust_id;
	std::vector<int> book_ids;
	std::string address;
	std::chrono::year_month_day date;
	PaymentOption option;
};

enum class ReturnOption {
	kByCourier, kBySelf, kCancel,
};

struct ReturnRequest {
	int cust_id;
	OrderID order_id;
	ReturnOption option;
};

struct Customer {
public:
	DeliveryOrder CreateDeliveryOrder(const Cart& cart, const std::string address, std::chrono::year_month_day date, PaymentOption option) {
		return DeliveryOrder{
			.cust_id = id_, .book_ids = cart.GetAll(), .address = std::move(address), .date = date, .option = option,
		};
	}

	ReturnRequest CreateReturnRequest(OrderID order_id, ReturnOption option) {
		return ReturnRequest{
			.cust_id = id_, .order_id = order_id, .option = option,
		};
	}

	unsigned GetBalance() {
		return balance_;
	}

private:
	friend class Store;

	Customer(int id, unsigned balance): id_{id}, balance_{balance} {
	}

	static std::shared_ptr<Customer> Create(int id, unsigned balance) {
		auto ptr = new Customer(id, balance);
		return std::shared_ptr<Customer>(ptr);
	}

	bool TryDecBalance(unsigned value) {
		if (balance_ < value) {
			return false;
		}
		balance_ -= value;
		return true;
	}

	void IncBalance(unsigned value) {
		balance_ += value;
	}

private:
	int id_;
	unsigned balance_;
};

struct Store {
public:
	enum class DeliveryStatus {
		kWaiting, kDelivered, kCancelled,
	};

public:
	static std::shared_ptr<Store> Create() {
		return std::make_shared<Store>();
	}

	std::shared_ptr<Customer> CreateCustomer(unsigned balance) {
		auto customer = Customer::Create(next_customer_id_++, balance);
		customers_.push_back(customer);
		return customer;
	}

	int AddBook(const Book& book) {
		auto it = std::find(books_.begin(), books_.end(), book);
		if (it != books_.end()) {
			*it = book;
			return static_cast<int>(std::distance(books_.begin(), it));
		}
		auto id = next_book_id_++;
		books_.push_back(book);
		return id;
	}

	std::optional<DeliveryStatus> CheckStatus(OrderID id) {
		if (id >= next_order_id_) {
			return std::nullopt;
		}
		return statuses_[id];
	}

	std::optional<OrderID> RegisterDelivery(DeliveryOrder order) {
		unsigned price = 0;
		auto customer = GetCustomerAndPrice(order, &price);
		if (!customer) {
			return std::nullopt;
		}
		if (order.option == PaymentOption::kUponOrder) {
			if (!customer->TryDecBalance(price)) {
				return std::nullopt;
			}
			profit_ += price;
		}
		auto id = next_order_id_++;
		orders_.push_back(order);
		statuses_.push_back(DeliveryStatus::kWaiting);
		return id;
	}

	bool Deliver(OrderID id) {
		if (id >= next_order_id_) {
			return false;
		}
		auto order = orders_[id];
		if (statuses_[id] != DeliveryStatus::kWaiting) {
			return false;
		}
		unsigned price = 0;
		auto customer = GetCustomerAndPrice(order, &price);
		if (!customer || (order.option == PaymentOption::kOnReceipt && !customer->TryDecBalance(price))) {
			statuses_[id] = DeliveryStatus::kCancelled;
			return false;
		}
		if (order.option == PaymentOption::kOnReceipt) {
			profit_ += price;
		}
		statuses_[id] = DeliveryStatus::kDelivered;
		return true;
	}

	bool Return(ReturnRequest request) {
		if (request.order_id >= next_order_id_) {
			return false;
		}
		auto order = orders_[request.order_id];
		auto status = statuses_[request.order_id];
		if (status == DeliveryStatus::kCancelled ||
			(request.option == ReturnOption::kCancel) != (status == DeliveryStatus::kWaiting)) {
			return false;
		}

		unsigned price = 0;
		auto customer = GetCustomerAndPrice(order, &price);
		if (request.option != ReturnOption::kCancel ||
			(request.option == ReturnOption::kCancel && order.option == PaymentOption::kUponOrder)) {
			if (customer) {
				customer->IncBalance(price);
				profit_ -= price;
			}
		}
		statuses_[request.order_id] = DeliveryStatus::kCancelled;
		return true;
	}

	unsigned GetProfit() {
		return profit_;
	}

private:
	std::shared_ptr<Customer> GetCustomerAndPrice(const DeliveryOrder& order, unsigned *price) {
		auto cust_id = order.cust_id;
		if (cust_id >= next_customer_id_) {
			return nullptr;
		}
		auto customer = customers_[cust_id];
		*price = 0;
		for (auto book_id : order.book_ids) {
			if (book_id >= next_book_id_) {
				return nullptr;
			}
			*price += books_[book_id].price;
		}
		return customer;
	}

private:
	int next_customer_id_ = 0;
	int next_book_id_ = 0;
	OrderID next_order_id_ = 0;
	unsigned profit_ = 0;
	std::vector<std::shared_ptr<Customer>> customers_;
	std::vector<Book> books_;
	std::vector<DeliveryOrder> orders_;
	std::vector<DeliveryStatus> statuses_;
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
