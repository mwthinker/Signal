#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include <catch2/catch.hpp>

#include <mw/signal.h>

SCENARIO("using signal", "[signal]") {
	GIVEN("a Signal with no connections") {
		mw::Signal<int> signal;
		REQUIRE(signal.empty() == true);
		REQUIRE(signal.size() == 0);
		
		WHEN("connection is added") {
			mw::signals::Connection c1 = signal.connect([](int) {});

			THEN("connection size is increased") {
				REQUIRE(signal.size() == 1);
			}
			THEN("signal is not empty") {
				REQUIRE(signal.empty() == false);
			}
		}

		WHEN("two connections are added") {
			mw::signals::Connection c1 = signal.connect([](int) {});
			mw::signals::Connection c2 = signal.connect([](int) {});

			THEN("connection size is increased") {
				REQUIRE(signal.size() == 2);
			}
			THEN("signal is not empty") {
				REQUIRE(signal.empty() == false);
			}
		}
		
	}

	GIVEN("a Signal with connections") {
		mw::Signal<int> signal;
		mw::signals::Connection c1 = signal.connect([](int) {});
		mw::signals::Connection c2 = signal.connect([](int) {});

		WHEN("disconnecting one connection") {
			c1.disconnect();

			THEN("connection size is decreased") {
				REQUIRE(signal.size() == 1);
			}
			THEN("signal is not empty") {
				REQUIRE(signal.empty() == false);
			}
			THEN("one connection should be connected") {
				REQUIRE(!c1.connected());
			}
			THEN("one connection should be disconnected") {
				REQUIRE(c2.connected());
			}
		}

		WHEN("calling connections") {			
			THEN("one connection should be disconnected") {
				REQUIRE(c2.connected());
			}
		}
	}

	GIVEN("a Signal with scoped connections") {
		mw::Signal<int> signal;

		bool invokedC1 = false;
		mw::signals::ScopedConnection c1 = signal.connect([&](int) { invokedC1 = true; });
		
		auto c2 = signal.connect([&](int) {});

		WHEN("disconnecting one connection") {
			c1.disconnect();

			THEN("connection size is decreased") {
				REQUIRE(signal.size() == 1);
			}
			THEN("signal is not empty") {
				REQUIRE(signal.empty() == false);
			}
			THEN("one connection should be connected") {
				REQUIRE(!c1.connected());
			}
			THEN("one connection should be disconnected") {
				REQUIRE(c2.connected());
			}
		}

		WHEN("release a scoped connection, disconnects and invoke callbacks") {
			c1.release();
			c1.disconnect();

			signal(1);

			THEN("callback should been invoked") {
				REQUIRE(invokedC1);
			}
		}

		WHEN("scoped connection getting out of scope, disconnects and invoke callbacks") {
			c1 = {};
			c1.disconnect();

			signal(1);

			THEN("callback should been invoked") {
				REQUIRE(invokedC1);
			}
		}
	}

	GIVEN("a Signal with added functions") {
		mw::Signal signal;
		int nbr = 0;
		auto function = [&nbr]() {
			nbr++;
		};
		auto c1 = signal.connect(function);
		auto c2 = signal.connect(function);
		auto c3 = signal.connect(function);

		WHEN("calling added function multiple times") {
			for (int i = 0; i < 4; ++i) {
				signal();
			}

			THEN("function should be called multiple times") {
				REQUIRE(nbr == 4 * 3);
			}
		}

		WHEN("calling clear") {
			signal.clear();
			THEN("all added functions should been removed") {
				REQUIRE(signal.size() == 0);
			}
		}
	}

	GIVEN("a Signal added with function and parameters") {
		mw::Signal<int> signal;
		int nbr = 0;
		auto function = [&nbr](int newNbr) {
			nbr = newNbr;
		};
		auto c1 = signal.connect(std::bind(function, std::placeholders::_1));

		WHEN("calling function with parameter") {
			signal(8);

			THEN("function should be called with the correct parameter") {
				REQUIRE(nbr == 8);
			}
		}
	}

	GIVEN("a Signal added with function") {
		mw::Signal<int> signal;
		int nbr = 0;
		auto function = [&nbr](int newNbr) {
			nbr = newNbr;
		};
		auto c1 = signal.connect(std::bind(function, std::placeholders::_1));

		WHEN("calling function with parameter") {
			signal(8);

			THEN("function should be called with the correct parameter") {
				REQUIRE(nbr == 8);
			}
		}

		WHEN("calling removed function") {
			c1.disconnect();
			signal(8);

			THEN("function should not be called") {
				REQUIRE(nbr != 8);
			}
		}

		WHEN("move constructor to a new Signal obejct") {
			mw::Signal<int> signal2(std::move(signal));
			

			THEN("new signal should contain the function") {
				REQUIRE(signal2.size() == 1);
			}

			THEN("old signal should be empty") {
				REQUIRE(signal.empty());
			}
		}

		WHEN("move assignment to a new Signal obejct") {
			mw::Signal<int> signal2;
			signal2 = std::move(signal);

			THEN("new signal should contain the function") {
				REQUIRE(signal2.size() == 1);
			}

			THEN("old signal should be empty") {
				REQUIRE(signal.empty());
			}
		}
	}

}
