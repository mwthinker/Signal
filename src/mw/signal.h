#ifndef SIGNAL_MW_SIGNAL_H
#define SIGNAL_MW_SIGNAL_H

#include <vector>
#include <memory>
#include <functional>

namespace mw {

	template <typename...>
	class Signal;

	namespace signals {

		// Used to disconnect a slot from a signal.
		class Connection {
		public:
			template <typename...> friend class ::mw::Signal;

			Connection() noexcept = default;
			Connection(const Connection&) = default;
			Connection(Connection&&) noexcept = default;

			Connection& operator=(const Connection&) = default;
			Connection& operator=(Connection&&) noexcept = default;

			void disconnect();

			bool connected() const;

		private:
			class SignalInterface {
			protected:
				friend class Connection;

				SignalInterface() = default;
				~SignalInterface() = default; // Not virtual. Should not be deleted.

			private:
				SignalInterface(const SignalInterface&) = delete;
				SignalInterface& operator=(const SignalInterface&) = delete;

				virtual void disconnect(size_t id) = 0;
			};

			struct Info {
				SignalInterface* signal;
			};
			using InfoPtr = std::shared_ptr<Info>;

			static inline size_t calculateHash(const InfoPtr& info) {
				return std::hash<InfoPtr>{}(info);
			}

			// Is called from mw::Signal to bind a connection.
			explicit Connection(InfoPtr c)
				: info_{std::move(c)} {
			}

			InfoPtr info_;
		};

		// Automatically disconnects when all copies goes out of scope.
		class ScopedConnection {
		public:
			ScopedConnection() = default;
			ScopedConnection(const Connection& connection)
				: connection_{connection} {
			}
			~ScopedConnection() {
				connection_.disconnect();
			}

			ScopedConnection(const ScopedConnection&) = default;
			ScopedConnection(ScopedConnection&&) noexcept = default;

			ScopedConnection& operator=(const ScopedConnection&) = default;
			ScopedConnection& operator=(ScopedConnection&&) noexcept = default;

			void disconnect() {
				connection_.disconnect();
			}

			bool connected() const {
				return connection_.connected();
			}

			void release() {
				connection_ = {};
			}

		private:
			Connection connection_;
		};

		class ScopedConnections {
		public:
			void operator+=(const Connection& scopedConnection) {
				connections_.push_back(scopedConnection);
			}

			void operator+=(std::initializer_list<Connection> connections) {
				connections_.insert(connections_.end(), connections.begin(), connections.end());
			}

			// Removes all connections.
			void clear() {
				connections_.clear();
			}

			// Removes all unconnected connections, i.e. all connections with no callback assigned.
			void cleanUp() {
				connections_.erase(std::remove_if(connections_.begin(), connections_.end(), [](const ScopedConnection& connection) {
					return !connection.connected();
				}), connections_.end());
			}

		private:
			std::vector<ScopedConnection> connections_;
		};

	}

	// A function container, in which the functions stored can be called. A slot/callbacks class.
	template <typename... A>
	class Signal : public signals::Connection::SignalInterface {
	public:
		using Callback = std::function<void(A...)>;

		Signal() = default;
		~Signal();

		Signal(const Signal&) = delete;
		Signal& operator=(const Signal&) = delete;

		Signal(Signal&&) noexcept;
		Signal& operator=(Signal&&) noexcept;

		[[nodiscard]]
		signals::Connection connect(const Callback& callback);

		template <typename... Params>
		void operator()(Params&&... params);

		template <typename... Params>
		void invoke(Params&&... params);

		template <typename T, typename... TArgs>
		[[nodiscard]] signals::Connection connect(T* object, void(T::* ptr)(TArgs... args)) {
			return connect([object, ptr](A... args) {
				(object->*ptr)(args...);
			});
		}

		void clear();

		int size() const noexcept;

		bool empty() const noexcept;

	private:
		using InfoPtr = signals::Connection::InfoPtr;

		void disconnect(size_t id) override;

		struct Pair {
			InfoPtr info;
			Callback callback;
		};

		std::vector<Pair> functions_; // All mapped callbacks.
	};

	template <typename Friend, typename... Args>
	class PublicSignal {
	public:
		using Callback = typename Signal<Args...>::Callback;

		friend Friend;

		[[nodiscard]]
		signals::Connection connect(const Callback& callback) {
			return signal_.connect(callback);
		}

		template <typename T, typename... TArgs>
		[[nodiscard]] signals::Connection connect(T* object, void(T::* ptr)(TArgs... args)) {
			return signal_.connect(object, ptr);
		}

	private:
		PublicSignal() = default;
		PublicSignal(const PublicSignal&) = delete;
		PublicSignal& operator=(const PublicSignal&) = delete;
		PublicSignal(PublicSignal&&) noexcept = default;
		PublicSignal& operator=(PublicSignal&&) noexcept = default;

		template <typename... Params>
		void operator()(Params&&... params) {
			signal_.invoke(std::forward<Params>(params)...);
		}

		template <typename... Params>
		void invoke(Params&&... params) {
			signal_.invoke(std::forward<Params>(params)...);
		}

		void clear() {
			signal_.clear();
		}

		int size() const noexcept {
			return signal_.size();
		}

		bool empty() const noexcept {
			return signal_.empty();
		}

		Signal<Args...> signal_;
	};

	// ------------ Definitions ------------

	inline void signals::Connection::disconnect() {
		if (info_ && info_->signal != nullptr) {
			info_->signal->disconnect(Connection::calculateHash(info_));
		}
	}

	inline bool signals::Connection::connected() const {
		return info_ && info_->signal != nullptr;
	}

	template <typename... A>
	Signal<A...>::Signal::~Signal() {
		clear();
	}

	template <typename... A>
	Signal<A...>::Signal(Signal<A...>&& signal) noexcept
		: functions_{std::move(signal.functions_)} {
	}

	template <typename... A>
	Signal<A...>& Signal<A...>::operator=(Signal<A...>&& signal) noexcept {
		functions_ = std::move(signal.functions_);
		return *this;
	}

	template <typename... A>
	signals::Connection Signal<A...>::connect(const Callback& callback) {
		auto c = std::make_shared<signals::Connection::Info>(this);
		functions_.push_back({c, callback});
		return signals::Connection(c);
	}

	template <typename... A>
	template <typename... Params>
	void Signal<A...>::operator()(Params&&... a) {
		invoke(std::forward<Params>(a)...);
	}

	template <typename... A>
	template <typename... Params>
	void Signal<A...>::invoke(Params&&... a) {
		const auto size = static_cast<int>(functions_.size());
		// Using index instead of foreach in order to be able to add callbacks during iteration.
		for (int i = 0; i < size && i < functions_.size(); ++i) {
			functions_[i].callback(a...);
		}
	}

	template <typename... A>
	void Signal<A...>::clear() {
		for (auto& [info, callback] : functions_) {
			info->signal = nullptr;
		}
		functions_.clear();
	}

	template <typename... A>
	int Signal<A...>::size() const noexcept {
		return static_cast<int>(functions_.size());
	}

	template <typename... A>
	bool Signal<A...>::empty() const noexcept {
		return functions_.empty();
	}

	template <typename... A>
	void Signal<A...>::disconnect(size_t id) {
		auto it = std::find_if(functions_.begin(), functions_.end(), [id](const auto& pair) {
			if (signals::Connection::calculateHash(pair.info) == id) {
				pair.info->signal = nullptr;
				return true;
			}
			return false;
		});
		if (it != functions_.end()) {
			functions_.erase(it);
		}
	}

}

#endif
