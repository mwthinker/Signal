#include <mw/signal.h>

#include <gtest/gtest.h>

class SignalTest : public ::testing::Test {
protected:

	SignalTest() {
	}

	~SignalTest() override {
	}

	void SetUp() override {
	}

	void TearDown() override {
	}

	const std::function<void()> emptyCallback = []() {};
};

TEST_F(SignalTest, newSignal_thenSignalIsEmpty) {
	// Given.
	mw::Signal signal;

	// Then.
	EXPECT_TRUE(signal.empty());
	EXPECT_EQ(0, signal.size());
}

TEST_F(SignalTest, connectionAdded_thenSignalNotEmpty) {
	// Given.
	mw::Signal signal;

	// When.
	[[maybe_unused]] auto connection = signal.connect(emptyCallback);

	// Then.
	EXPECT_FALSE(signal.empty());
	EXPECT_EQ(1, signal.size());
}

TEST_F(SignalTest, connectionAssignedCopiedAndDisconnected_thenCopiedConnectionIsDisconnected) {
	// Given.
	mw::Signal signal;

	// When.
	auto connection = signal.connect(emptyCallback);
	EXPECT_TRUE(connection.connected());
	mw::signals::Connection copiedConnection;
	EXPECT_FALSE(copiedConnection.connected());
	copiedConnection = connection;
	EXPECT_TRUE(copiedConnection.connected());
	copiedConnection.disconnect();

	// Then.
	EXPECT_FALSE(connection.connected());
	EXPECT_FALSE(copiedConnection.connected());
}

TEST_F(SignalTest, connectionConstuctorCopiedAndDisconnected_thenCopiedConnectionIsDisconnected) {
	// Given.
	mw::Signal signal;

	// When.
	auto connection = signal.connect(emptyCallback);
	EXPECT_TRUE(connection.connected());
	mw::signals::Connection copiedConnection = connection;
	EXPECT_TRUE(copiedConnection.connected());
	copiedConnection.disconnect();

	// Then.
	EXPECT_FALSE(connection.connected());
	EXPECT_FALSE(copiedConnection.connected());
}

TEST_F(SignalTest, connectionsAdded_thenSignalHasCorrectSize) {
	// Given.
	mw::Signal signal;

	// When.
	for (int i = 0; i < 17; ++i) {
		[[maybe_unused]] auto connection = signal.connect(emptyCallback);
	}

	// Then.
	EXPECT_FALSE(signal.empty());
	EXPECT_EQ(17, signal.size());
}

TEST_F(SignalTest, invokeMultipleTimes_thenInvokedMultipleTimes) {
	// Given.
	int nbr = 0;
	mw::Signal signal;
	[[maybe_unused]] auto tmp = signal.connect([&nbr]() {
		++nbr;
	});

	// When.
	const int InvokedNbr = 17;
	for (int i = 0; i < InvokedNbr; ++i) {
		signal.invoke();
	}

	// Then.
	EXPECT_EQ(InvokedNbr, nbr);
}

TEST_F(SignalTest, clearConnections_thenConnectionAreRemoved) {
	// Given.
	int nbr = 0;
	mw::Signal signal;
	auto connection = signal.connect([&nbr]() {
		++nbr;
	});
	EXPECT_TRUE(connection.connected());

	// When.
	signal.clear();

	// Then.
	EXPECT_FALSE(connection.connected());
}

TEST_F(SignalTest, invokedWithSpecificArgument_thenCallbacksInvokedWithSpecificArgument) {
	// Given.
	int specificVar = 0;
	mw::Signal<int> signal;
	auto connection = signal.connect([&specificVar](int value) {
		specificVar = value;
	});
	EXPECT_TRUE(connection.connected());

	// When.
	signal.invoke(2);

	// Then.
	EXPECT_EQ(2, specificVar);
}

TEST_F(SignalTest, removeConnection_whenInvoked_thenCallbackIsNotInvoked) {
	// Given.
	bool invoked = false;
	mw::Signal signal;
	auto connection = signal.connect([&invoked]() {
		invoked = true;
	});
	EXPECT_TRUE(connection.connected());
	EXPECT_FALSE(invoked);

	// When.
	connection.disconnect();
	signal.invoke();

	// Then.
	EXPECT_FALSE(invoked);
}

TEST_F(SignalTest, moveSignalToNewSignalObject_thenNewSignalShallContainAllConnections) {
	// Given.
	mw::Signal signal;
	[[maybe_unused]] auto c1 = signal.connect(emptyCallback);
	[[maybe_unused]] auto c2 = signal.connect(emptyCallback);

	EXPECT_EQ(2, signal.size());
	EXPECT_FALSE(signal.empty());

	// When.
	mw::Signal newSignal = std::move(signal);

	// Then.
	EXPECT_EQ(0, signal.size());
	EXPECT_TRUE(signal.empty());

	EXPECT_EQ(2, newSignal.size());
	EXPECT_FALSE(newSignal.empty());
}

TEST_F(SignalTest, signalAssignedToNewSignalObject_thenNewSignalShallContainAllConnections) {
	// Given.
	mw::Signal signal;
	[[maybe_unused]] auto connection = signal.connect(emptyCallback);
	[[maybe_unused]] auto connection2 = signal.connect(emptyCallback);

	EXPECT_EQ(2, signal.size());

	// When.
	mw::Signal newSignal;
	newSignal = std::move(signal);

	// Then.
	EXPECT_EQ(0, signal.size());
	EXPECT_EQ(2, newSignal.size());
}

TEST_F(SignalTest, addingNewCallbackInCallback_thenNewCallbackShouldNotBeInvoked) {
	// Given.
	mw::Signal signal;
	[[maybe_unused]] auto c1 = signal.connect([&]() {
		[[maybe_unused]] auto c2 = signal.connect([]() {
			FAIL();
			});
		});

	EXPECT_EQ(1, signal.size());

	// When/Then.
	signal.invoke();
}

TEST_F(SignalTest, removedConnectionAndInvoke_thenOnlyActiveConnectionsAreInvoked) {
	// Given.
	mw::Signal signal;
	bool called1 = false;
	auto connection1 = signal.connect([&called1]() {
		called1 = true;
		});
	bool called2 = false;
	auto connection2 = signal.connect([&called2]() {
		called2 = true;
		});

	// When.
	connection1.disconnect();
	signal.invoke();

	// Then.
	EXPECT_FALSE(called1);
	EXPECT_FALSE(connection1.connected());
	EXPECT_TRUE(called2);
	EXPECT_TRUE(connection2.connected());
}

TEST_F(SignalTest, connectingDuringInvoke_thenNewCallbackIsNotCalled) {
	mw::Signal signal;
	bool invoked = false;
	[[maybe_unused]] auto tmp = signal.connect([&]() {
		invoked = true;
		[[maybe_unused]] auto tmp = signal.connect([]() {
			FAIL();
			});
		});
	EXPECT_FALSE(invoked);

	// When.
	signal.invoke();

	// Then.
	EXPECT_TRUE(invoked);
	EXPECT_EQ(2, signal.size());
}

TEST_F(SignalTest, disconnectiongDuringInvoke_thenDisconnectedCallbackIsNotInvoked) {
	// Given.
	mw::Signal signal;
	mw::signals::Connection c2;
	[[maybe_unused]] auto c1 = signal.connect([&]() {
		c2.disconnect();
	});
	c2 = signal.connect([]() {
		FAIL();
	});
	EXPECT_EQ(2, signal.size());

	// When.
	signal.invoke();

	// Then.
	EXPECT_EQ(1, signal.size());
}

TEST_F(SignalTest, disconnectingAndConnecting_thenCallbackOrderIsPreserved) {
	// Given.
	mw::Signal signal;
	mw::signals::Connection c4;
	int order = 0;

	[[maybe_unused]] auto c1 = signal.connect([&order]() {
		EXPECT_EQ(++order, 1);
	});
	[[maybe_unused]] auto c2 = signal.connect([&]() {
		EXPECT_EQ(++order, 2);
		[[maybe_unused]] auto tmp = signal.connect([&order]() {
			EXPECT_EQ(++order, 4);
		});
	});
	[[maybe_unused]] auto c3 = signal.connect([&]() {
		EXPECT_EQ(++order, 3);
		c1.disconnect();
	});
	EXPECT_EQ(3, signal.size());

	// When.
	signal.invoke();

	// Then.
	EXPECT_EQ(3, signal.size());
}

TEST_F(SignalTest, connecting_thenCallbackOrderIsPreserved) {
	// Given.
	mw::Signal signal;
	int order = 0;

	[[maybe_unused]] auto c1 = signal.connect([&order]() {
		EXPECT_EQ(++order, 1);
	});
	[[maybe_unused]] auto c2 = signal.connect([&order]() {
		EXPECT_EQ(++order, 2);
	});
	[[maybe_unused]] auto c3 = signal.connect([&order]() {
		EXPECT_EQ(++order, 3);
	});
	[[maybe_unused]] auto c4 = signal.connect([&order]() {
		EXPECT_EQ(++order, 4);
	});

	// When.
	signal.invoke();

	// Then.
	EXPECT_EQ(4, signal.size());
}

TEST_F(SignalTest, disconnecting_thenCallbackOrderIsPreserved) {
	// Given.
	mw::Signal signal;
	int order = 0;

	auto c1 = signal.connect([]() {});
	[[maybe_unused]] auto c2 = signal.connect([&order]() {
		EXPECT_EQ(++order, 1);
	});
	[[maybe_unused]] auto c3 = signal.connect([&order]() {
		EXPECT_EQ(++order, 2);
	});
	auto c4 = signal.connect([]() {});
	[[maybe_unused]] auto c5 = signal.connect([&order]() {
		EXPECT_EQ(++order, 3);
	});

	// When.
	c1.disconnect();
	c4.disconnect();
	signal.invoke();

	// Then.
	EXPECT_EQ(3, signal.size());
}

TEST_F(SignalTest, recursiveInvoke_thenRecursiveInvokationIsMade) {
	// Given.
	int invokations = 0;

	mw::Signal signal;
	[[maybe_unused]] auto tmp = signal.connect([&]() {
		if (++invokations < 5) {
			signal.invoke();
		}
	});

	// When.
	signal.invoke();

	// Then.
	EXPECT_EQ(5, invokations);
}

TEST_F(SignalTest, registerSameFunctionTwiceAndInvoke_thenFunctionIsCalledTwice) {
	// Given.
	int invokations = 0;

	mw::Signal signal;
	auto callback = [&]() {
		++invokations;
	};
	[[maybe_unused]] auto tmp = signal.connect(callback);
	[[maybe_unused]] auto tmp2 = signal.connect(callback);

	// When.
	signal.invoke();

	// Then.
	EXPECT_EQ(2, invokations);
}

TEST_F(SignalTest, connectionDisconnected_thenSignalSizeDecreased) {
	// Given.
	mw::Signal signal;
	auto c1 = signal.connect(emptyCallback);
	[[maybe_unused]] auto c2 = signal.connect(emptyCallback);

	// When.
	EXPECT_EQ(2, signal.size());
	c1.disconnect();

	// Then.
	EXPECT_EQ(1, signal.size());
}

TEST_F(SignalTest, multipleCallbacks_whenInvokeWithParameters_thenArgumentsAreCopiedNotForwarded) {
	// Given.
	using IntPtr = std::shared_ptr<int>;
	mw::Signal<IntPtr> signal;
	int product = 1;

	[[maybe_unused]] auto c1 = signal.connect([&](IntPtr intPtr) {
		EXPECT_TRUE(intPtr);
		EXPECT_EQ(1, *intPtr);
		product *= 2;
	});
	[[maybe_unused]] auto c2 = signal.connect([&](IntPtr intPtr) {
		EXPECT_TRUE(intPtr);
		EXPECT_EQ(1, *intPtr);
		product *= 3;
	});

	// When.
	auto intPtr = std::make_shared<int>(1);
	signal.invoke(intPtr);

	// Then.
	EXPECT_EQ(2*3, product);
}

struct Object {
	Object() = default;

	Object(const Object& ob) {
		++copies;
	}

	static int copies;
};

int Object::copies = 0;

TEST_F(SignalTest, invokeWithArguments_thenCopiesAreCopiedMaxTwice) {
	// TODO! Fix Signal to copy max once (if possible)! Now one extra unneeded copy is made.
	// Given.
	int invokations = 0;
	
	Object::copies = 0;
	mw::Signal<Object> signal;
	[[maybe_unused]] auto tmp = signal.connect([&](Object ob) {
		++invokations;
	});

	// When.
	Object value;
	EXPECT_EQ(0, Object::copies);
	signal(value);

	// Then.
	EXPECT_EQ(1, invokations);
	EXPECT_EQ(2, Object::copies);
}

struct A {
	int nbr = 7;
};

TEST_F(SignalTest, testCompilable) {
	{
		mw::Signal signal;
		mw::signals::ScopedConnections s{
			signal.connect([]() {}),
			signal.connect([]() {})
		};
		s += signal.connect([]() {});
		s += {
			signal.connect([]() {}),
			signal.connect([]() {})
		};
	}
	{
		mw::Signal<A> signal;
		auto c = signal.connect([](A) {});

		signal(A{});
		A tmp;
		signal(tmp);
		const A constTmp;
		signal(constTmp);
	}
	{
		mw::Signal<A&> signal;
		auto c = signal.connect([](A) {});

		signal(A{});
		A tmp;
		signal(tmp);
	}
	{
		mw::Signal<const A&> signal;
		auto c = signal.connect([](A) {});

		signal(A{});
		A tmp;
		signal(tmp);
		const A constTmp;
		signal(constTmp);
	}
}
