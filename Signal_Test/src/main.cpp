#include <mw/signal.h>

#include <gtest/gtest.h>

// The fixture for testing class Foo.
class SignalTest : public ::testing::Test {
protected:
	// You can remove any or all of the following functions if their bodies would
	// be empty.

	SignalTest() {
		// You can do set-up work for each test here.
	}

	~SignalTest() override {
		// You can do clean-up work that doesn't throw exceptions here.
	}

	// If the constructor and destructor are not enough for setting up
	// and cleaning up each test, you can define the following methods:

	void SetUp() override {
		// Code here will be called immediately after the constructor (right
		// before each test).
	}

	void TearDown() override {
		// Code here will be called immediately after each test (right
		// before the destructor).
	}

	// Class members declared here can be used by all tests in the test suite
	// for Foo.
};

TEST_F(SignalTest, givenNewSignal_thenSignalIsEmpty) {
	// Given.
	mw::Signal<int> signal;

	// Then.
	EXPECT_TRUE(signal.empty());
	EXPECT_EQ(0, signal.size());
}

TEST_F(SignalTest, givenSignal_whenConnectionAdded_thenSignalNotEmpty) {
	// Given.
	mw::Signal<int> signal;

	// When.
	[[maybe_unused]] auto connection = signal.connect([](int) {});

	// Then.
	EXPECT_FALSE(signal.empty());
	EXPECT_EQ(1, signal.size());
}

TEST_F(SignalTest, givenSignal_whenNbrOfConnectionsAdded_thenSignalHasSizeNbr) {
	// Given.
	const int Nbr = 17;
	mw::Signal<int> signal;

	// When.
	for (int i = 0; i < Nbr; ++i) {
		[[maybe_unused]] auto connection = signal.connect([](int) {});
	}

	// Then.
	EXPECT_FALSE(signal.empty());
	EXPECT_EQ(Nbr, signal.size());
}

TEST_F(SignalTest, givenSignalAndActiveConnection_whenInvokeMultipleTimes_thenItIsinvokedMultipleTimes) {
	// Given.
	int nbr = 0;
	mw::Signal<int> signal;
	[[maybe_unused]] auto tmp = signal.connect([&nbr](int) {
		++nbr;
	});

	// When.
	const int InvokedNbr = 17;
	for (int i = 0; i < InvokedNbr; ++i) {
		signal.invoke(1);
	}

	// Then.
	EXPECT_EQ(InvokedNbr, nbr);
}

TEST_F(SignalTest, givenSignalAndActiveConnection_whenClear_thenConnectionIsRemoved) {
	// Given.
	int nbr = 0;
	mw::Signal<int> signal;
	auto connection = signal.connect([&nbr](int) {
		++nbr;
	});
	EXPECT_TRUE(connection.connected());

	// When.
	signal.clear();

	// Then.
	EXPECT_FALSE(connection.connected());
}

TEST_F(SignalTest, givenSignal_whenInvokedWithSpecificArgument_thenFunctionInvokedWithSpecificArgument) {
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

TEST_F(SignalTest, givenSignalWithRemovedConnection_whenInvoked_thenFunctionIsNotInvoked) {
	// Given.
	bool invoked = false;
	mw::Signal<int> signal;
	auto connection = signal.connect([&invoked](int value) {
		invoked = true;
	});
	EXPECT_TRUE(connection.connected());
	EXPECT_FALSE(invoked);

	// When.
	signal.invoke(2);

	// Then.
	EXPECT_FALSE(invoked);
}

TEST_F(SignalTest, givenSignal_whenMovedToNewSignalObject_thenNewSignalShallContainAllConnections) {
	// Given.
	mw::Signal<int> signal;
	auto connection = signal.connect([](int value) {});
	auto connection2 = signal.connect([](int value) {});
	
	EXPECT_EQ(2, signal.size());
	EXPECT_FALSE(signal.empty());

	// When.
	mw::Signal<int> newSignal = std::move(signal);

	// Then.
	EXPECT_EQ(0, signal.size());
	EXPECT_TRUE(signal.empty());

	EXPECT_EQ(2, newSignal.size());
	EXPECT_FALSE(newSignal.empty());
}

TEST_F(SignalTest, givenSignal_whenAssignedToNewSignalObject_thenNewSignalShallContainAllConnections) {
	// Given.
	mw::Signal<int> signal;
	auto connection = signal.connect([](int value) {});
	auto connection2 = signal.connect([](int value) {});

	EXPECT_EQ(2, signal.size());
	EXPECT_FALSE(signal.empty());

	// When.
	mw::Signal<int> newSignal;
	newSignal = std::move(signal);

	// Then.
	EXPECT_EQ(0, signal.size());
	EXPECT_TRUE(signal.empty());

	EXPECT_EQ(2, newSignal.size());
	EXPECT_FALSE(newSignal.empty());
}

TEST_F(SignalTest, givenSignal_whenAddingNewCallbackInCallback_thenNewCallbackShouldNotBeInvoked) {
	// Given.
	mw::Signal<int> signal;
	auto connection = signal.connect([&](int value) {
		auto connection2 = signal.connect([](int value) {
			FAIL();
		});
	});

	EXPECT_EQ(2, signal.size());
	EXPECT_FALSE(signal.empty());

	// When.
	signal.invoke(2);
	

	// Then not throw.
}


TEST_F(SignalTest, givenSignalWithConnections_whenRemovedConnectionAndInvoke_thenOnlyActiveConnectionIsInvoked) {
	// Given.
	mw::Signal<int> signal;
	bool called1 = false;
	auto connection1 = signal.connect([&called1](int) {
		called1 = true;
	});
	bool called2 = false;
	auto connection2 = signal.connect([&called2](int) {
		called2 = true;
	});

	// When.
	connection1.disconnect();
	signal.invoke(1);

	// Then.
	EXPECT_FALSE(called1);
	EXPECT_FALSE(connection1.connected());
	EXPECT_TRUE(called2);
	EXPECT_FALSE(connection2.connected());
}

TEST_F(SignalTest, givenSignalWithConnections_whenDisconnectiongDuringInvoke_then) {
	FAIL();
}


TEST_F(SignalTest, givenSignalWithConnections_whenConnectionDisconnected_thenSignalSizeDecreesed) {
	// Given.
	mw::Signal<int> signal;
	auto c1 = signal.connect([](int) {});
	auto c2 = signal.connect([](int) {});

	// When.
	EXPECT_EQ(2, signal.size());
	c1.disconnect();

	// Then.
	EXPECT_EQ(1, signal.size());
}

struct A {
	int nbr{7};
};

TEST_F(SignalTest, testCompilable) {
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

TEST_F(SignalTest, givenScopedConnections_whenGoingOutOfScope_thenSignalSizeDecreesed) {
	// Given.
	mw::Signal<int> signal;
	auto connection = signal.connect([](int) {});

	// When.
	{
		mw::signals::ScopedConnection scopedConnection = signal.connect([](int) {});
		EXPECT_EQ(2, signal.size());
		EXPECT_TRUE(scopedConnection.connected());
		EXPECT_TRUE(connection.connected());
	}	

	// Then.
	EXPECT_EQ(1, signal.size());
	EXPECT_TRUE(connection.connected());
}
