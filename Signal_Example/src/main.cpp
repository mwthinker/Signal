#include <mw/signal.h>

#include <iostream>

enum class GameEvent {
	GameOver,
	Walk
};

class Zombie {
public:
	Zombie() = default;

	Zombie(Zombie&& zombie) noexcept
		: gameEventUpdated{std::move(zombie.gameEventUpdated)}
		, pointsUpdated{std::move(zombie.pointsUpdated)} {

	}

	Zombie& operator=(Zombie&& zombie) noexcept {
		gameEventUpdated = std::move(zombie.gameEventUpdated);
		pointsUpdated = std::move(zombie.pointsUpdated);
		return *this;
	}

	mw::PublicSignal<Zombie, GameEvent> gameEventUpdated;
	mw::PublicSignal<Zombie, int> pointsUpdated;

	void walk() {
		++x_;

		gameEventUpdated(GameEvent::Walk);
		if (x_ == 2) {
			++points_;
			pointsUpdated(points_);
		}

		if (x_ == 3) {
			++points_;
			pointsUpdated(points_);
		}

		if (x_ == 5) {
			gameEventUpdated(GameEvent::GameOver);
		}
	}

private:
	int x_{0};
	int points_{0};
};

template <class Unit>
void example() {
	bool gameOver = false;

	Unit unit;
	mw::signals::ScopedConnections connections;

	connections += {
		unit.gameEventUpdated.connect([&](GameEvent gameEvent) {
			switch (gameEvent) {
				case GameEvent::GameOver:
					gameOver = true;
					std::cout << "Game Over\n";
					break;
				case GameEvent::Walk:
					std::cout << "Walking\n";
					break;
			}
		}),
		unit.pointsUpdated.connect([&](int points) {
			std::cout << "Points updated: " << points << "\n";
		})
	};

	while (!gameOver) {
		unit.walk();
	}
}

int main(int argc, char** argv) {
	std::cout << "Example PublicSignal Zombie\n";
	example<Zombie>();

	Zombie zombie;
	Zombie zombie2 = std::move(zombie2);

	return 0;
}
