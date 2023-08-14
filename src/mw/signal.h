#ifndef SIGNAL_MW_SIGNAL_H
#define SIGNAL_MW_SIGNAL_H

#include <vector>
#include <memory>
#include <functional>

namespace mw {

	template <typename...>
	class Signal;

	namespace signals {

		/// @brief Used to disconnect a slot from a signal
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

				virtual void disconnect(size_t hash) = 0;
			};

			struct Key {
				SignalInterface* signal;
			};
			using KeyPtr = std::shared_ptr<Key>;

			static inline size_t calculateHash(const KeyPtr& key) {
				return std::hash<KeyPtr>{}(key);
			}

			// Is called from mw::Signal to bind a connection.
			explicit Connection(KeyPtr c)
				: key_{std::move(c)} {
			}

			KeyPtr key_;
		};

		/// @brief Automatically disconnects slots when going out of scope.
		class ScopedConnection {
		public:
			ScopedConnection() = default;
			ScopedConnection(const Connection& connection)
				: connection_{connection} {
			}
			~ScopedConnection() {
				connection_.disconnect();
			}

			ScopedConnection(const ScopedConnection&) = delete;
			ScopedConnection& operator=(const ScopedConnection&) = delete;
			
			ScopedConnection(ScopedConnection&&) = delete;
			ScopedConnection& operator=(ScopedConnection&&) = delete;

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

		/// @brief Automatically disconnects all connections stored when going out of scope.
		class ScopedConnections {
		public:
			ScopedConnections() = default;
			
			ScopedConnections(std::initializer_list<Connection> connections)
				: connections_(connections.begin(), connections.end()) {
			}

			~ScopedConnections() {
				clear();
			}

			ScopedConnections(const ScopedConnections&) = delete;
			ScopedConnections& operator=(const ScopedConnections&) = delete;

			ScopedConnections(ScopedConnections&&) = delete;
			ScopedConnections& operator=(ScopedConnections&&) = delete;

			void operator+=(const Connection& connection) {
				connections_.emplace_back(connection);
			}

			void operator+=(std::initializer_list<Connection> connections) {
				connections_.insert(connections_.end(), connections.begin(), connections.end());
			}

			/// @brief Removes all connections.
			void clear() {
				for (auto& connection : connections_) {
					connection.disconnect();
				}
				connections_.clear();
			}

			/// @brief Removes all unconnected connections, i.e. all connections with no callback assigned.
			void cleanUp() {
				std::erase_if(connections_, [](Connection& connection) {
					if (connection.connected()) {
						connection.disconnect();
						return true;
					}
					return false;
				});
			}

			/// @brief Return the number of connections.
			/// @return number of connections.
			int size() const {
				return static_cast<int>(connections_.size());
			}

		private:
			std::vector<Connection> connections_;
		};

	}

	/// @brief Contains a list of functions that can be called. A slot/callbacks class.
	/// @tparam ...A the slots invoke arguments
	template <typename... Args>
	class Signal : public signals::Connection::SignalInterface {
	public:
		using Callback = std::function<void(Args...)>;

		Signal() = default;
		~Signal();

		Signal(const Signal&) = delete;
		Signal& operator=(const Signal&) = delete;

		Signal(Signal&&) noexcept;
		Signal& operator=(Signal&&) noexcept;

		[[nodiscard]] signals::Connection connect(const Callback& callback);

		template <typename... Params>
		void operator()(Params&&... params);

		template <typename... Params>
		void invoke(Params&&... params);

		template <typename T, typename... TArgs>
		[[nodiscard]] signals::Connection connect(T* object, void(T::* ptr)(TArgs... args)) {
			return connect([object, ptr](Args... args) {
				(object->*ptr)(args...);
			});
		}

		void clear();

		int size() const noexcept;

		bool empty() const noexcept;

	private:
		using KeyPtr = signals::Connection::KeyPtr;

		void disconnect(size_t hash) override;

		struct KeyCallback {
			KeyPtr key;
			Callback callback;
		};

		std::vector<KeyCallback> callbacks_;
	};

	
	/// @brief Can be used as public member function of Signal, only adding slots are
	/// available for outside code.
	/// @tparam Friend the class that contains all functionality of the underlaying signal object
	/// @tparam ...Args the slots invoke arguments
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
		if (key_ && key_->signal != nullptr) {
			key_->signal->disconnect(Connection::calculateHash(key_));
		}
	}

	inline bool signals::Connection::connected() const {
		return key_ && key_->signal != nullptr;
	}

	template <typename... Args>
	Signal<Args...>::Signal::~Signal() {
		clear();
	}

	template <typename... Args>
	Signal<Args...>::Signal(Signal<Args...>&& signal) noexcept
		: callbacks_{std::move(signal.callbacks_)} {}

	template <typename... Args>
	Signal<Args...>& Signal<Args...>::operator=(Signal<Args...>&& signal) noexcept {
		callbacks_ = std::move(signal.callbacks_);
		return *this;
	}

	template <typename... Args>
	signals::Connection Signal<Args...>::connect(const Callback& callback) {
		auto c = std::make_shared<signals::Connection::Key>(this);
		callbacks_.push_back({c, callback});
		return signals::Connection(c);
	}

	template <typename... Args>
	template <typename... Params>
	void Signal<Args...>::operator()(Params&&... a) {
		invoke(std::forward<Params>(a)...);
	}

	template <typename... Args>
	template <typename... Params>
	void Signal<Args...>::invoke(Params&&... a) {
		const auto size = static_cast<int>(callbacks_.size());
		// Using index instead of foreach in order to be able to add callbacks during iteration.
		for (int i = 0; i < size && i < callbacks_.size(); ++i) {
			callbacks_[i].callback(a...);
		}
	}

	template <typename... Args>
	void Signal<Args...>::clear() {
		for (auto& [key, callback] : callbacks_) {
			key->signal = nullptr;
		}
		callbacks_.clear();
	}

	template <typename... Args>
	int Signal<Args...>::size() const noexcept {
		return static_cast<int>(callbacks_.size());
	}

	template <typename... Args>
	bool Signal<Args...>::empty() const noexcept {
		return callbacks_.empty();
	}

	template <typename... Args>
	void Signal<Args...>::disconnect(size_t hash) {
		auto it = std::find_if(callbacks_.begin(), callbacks_.end(), [hash](const KeyCallback& keyCallback) {
			if (signals::Connection::calculateHash(keyCallback.key) == hash) {
				return true;
			}
			return false;
		});
		if (it != callbacks_.end()) {
			it->key->signal = nullptr;
			callbacks_.erase(it);
		}
	}

}

#endif
