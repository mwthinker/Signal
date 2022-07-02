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

	mw::Signal<int> signal;
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
