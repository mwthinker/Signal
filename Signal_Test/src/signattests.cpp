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
	mw::Signal signal;

	// When.
	[[maybe_unused]] auto connection = signal.connect([]() {});

	// Then.
	EXPECT_FALSE(signal.empty());
	EXPECT_EQ(1, signal.size());
}

TEST_F(SignalTest, givenSignal_whenNbrOfConnectionsAdded_thenSignalHasSizeNbr) {
	// Given.
	const int Nbr = 17;
	mw::Signal signal;

	// When.
	for (int i = 0; i < Nbr; ++i) {
		[[maybe_unused]] auto connection = signal.connect([]() {});
	}

	// Then.
	EXPECT_FALSE(signal.empty());
	EXPECT_EQ(Nbr, signal.size());
}

TEST_F(SignalTest, givenSignalAndActiveConnection_whenInvokeMultipleTimes_thenItIsinvokedMultipleTimes) {
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

TEST_F(SignalTest, givenSignalAndActiveConnection_whenClear_thenConnectionIsRemoved) {
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

TEST_F(SignalTest, givenSignal_whenMovedToNewSignalObject_thenNewSignalShallContainAllConnections) {
	// Given.
	mw::Signal signal;
	auto connection = signal.connect([]() {});
	auto connection2 = signal.connect([]() {});
	
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

TEST_F(SignalTest, givenSignal_whenAssignedToNewSignalObject_thenNewSignalShallContainAllConnections) {
	// Given.
	mw::Signal signal;
	auto connection = signal.connect([]() {});
	auto connection2 = signal.connect([]() {});

	EXPECT_EQ(2, signal.size());

	// When.
	mw::Signal newSignal;
	newSignal = std::move(signal);

	// Then.
	EXPECT_EQ(0, signal.size());
	EXPECT_EQ(2, newSignal.size());
}

TEST_F(SignalTest, givenSignal_whenAddingNewCallbackInCallback_thenNewCallbackShouldNotBeInvoked) {
	// Given.
	mw::Signal signal;
	auto connection = signal.connect([&]() {
		auto connection2 = signal.connect([]() {
			FAIL();
		});
	});

	EXPECT_EQ(1, signal.size());

	// When/Then.
	signal.invoke();
}

TEST_F(SignalTest, givenSignal_whenRemovedConnectionAndInvoke_thenOnlyActiveConnectionsAreInvoked) {
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

TEST_F(SignalTest, givenSignal_whenDisconnectiongDuringInvoke_then) {
	FAIL(); // TODO?
}

TEST_F(SignalTest, givenSignalWithConnections_whenConnectionDisconnected_thenSignalSizeDecreesed) {
	// Given.
	mw::Signal signal;
	auto c1 = signal.connect([]() {});
	auto c2 = signal.connect([]() {});

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
	mw::Signal signal;
	auto connection = signal.connect([]() {});

	// When.
	{
		mw::signals::ScopedConnection scopedConnection = signal.connect([]() {});
		EXPECT_EQ(2, signal.size());
		EXPECT_TRUE(scopedConnection.connected());
		EXPECT_TRUE(connection.connected());
	}	

	// Then.
	EXPECT_EQ(1, signal.size());
	EXPECT_TRUE(connection.connected());
}
