#include <mw/signal.h>

#include <iostream>
#include <functional>
#include <cassert>
#include <vector>

void shouldNotBeCalled(int a) {
	assert(0);
}

bool mustBeCalled_ = false;

// Fails if a is not 5.
void testMustBe5(int a) {
	assert(a == 5);
	mustBeCalled_ = true;
}

void test(int a) {
}

int id = 0;

class Test {
public:
	Test() {
		id_ = ++id;
	}

	~Test() {
		signal_(this);
	}

	void test(int a) {
	}

	mw::Signal<Test*> signal_;
	int id_;
};

void testTest(Test* test) {
	assert(id == test->id_);
}

int main(int argc, char** argv) {
	mw::signals::Connection connection;
	{
		mw::Signal<int> signal;
		assert(signal.empty());
		mw::signals::Connection c1 = signal.connect(std::bind(testMustBe5, std::placeholders::_1));
		assert(signal.size() == 1);
		
		mw::signals::Connection c2 = signal.connect(std::bind(shouldNotBeCalled, 1));
		// Test size.
		assert(signal.size() == 2);
		assert(!signal.empty());

		// Should be connected.
		assert(c2.connected());
		c2.disconnect();

		// Test size.
		assert(signal.size() == 1);

		// Should be disconnected now.
		assert(!c2.connected());

		// Must be 5.
		signal(5);

		// Was test called?
		assert(mustBeCalled_);

		// Should be connected.
		assert(c1.connected());
		c1.disconnect();

		// Test size.
		assert(signal.size() == 0);
		assert(signal.empty());

		// Should be disconnected now.
		assert(!c1.connected());

		connection = signal.connect(std::bind(shouldNotBeCalled, 1));
		// Should be connected.
		assert(connection.connected());
	}
	// Should be disconnected now.
	assert(!connection.connected());

	{ // Test call signal and this from inside a destructor.
		for (int i = 0; i < 999; ++i) {
			Test* t = new Test();
			assert(t->signal_.size() == 0);
			t->signal_.connect(std::bind(testTest, std::placeholders::_1));
			assert(t->signal_.size() == 1);
			delete t;
		}
	}

	std::cout << "Test succeeded!\n";
	return 0;
}
