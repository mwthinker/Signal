#include <mw/signal.h>

#include <gtest/gtest.h>

class ScopedConnectionTest : public ::testing::Test {
protected:

	ScopedConnectionTest() {
	}

	~ScopedConnectionTest() override {
	}

	void SetUp() override {
	}

	void TearDown() override {
	}
};

TEST_F(ScopedConnectionTest, givenScopedConnections_whenGoingOutOfScope_thenSignalSizeDecreesed) {
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

TEST_F(ScopedConnectionTest, givenScopedConnections_whenInvoked_thenAllIsInvoked) {
	// Given.
	mw::Signal signal;
	int nbr = 0;
	auto func = [&nbr]() {
		++nbr;
	};

	// When.
	{
		mw::signals::ScopedConnection scopedConnection = signal.connect(func);
		mw::signals::ScopedConnections scopedConnections = {
			signal.connect(func),
			signal.connect(func)
		};
		scopedConnections += {
			signal.connect(func),
			signal.connect(func)
		};
		scopedConnections += signal.connect(func);
		EXPECT_EQ(5, scopedConnections.size());

		signal.invoke();
	}

	// Then.
	EXPECT_EQ(0, signal.size());
	EXPECT_EQ(6, nbr);
}
