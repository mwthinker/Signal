#ifndef SIGNAL_MW_SIGNAL_H
#define SIGNAL_MW_SIGNAL_H

#include <vector>
#include <memory>
#include <functional>

namespace mw {

	template <class...>
	class Signal;

	namespace signals {

		// A connection remembers a connection.
		class Connection {
		public:
			template <class...> friend class ::mw::Signal;

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

				virtual void disconnect(int id) = 0;
			};

			struct Info {
				Info(int idValue, SignalInterface* signalPointer)
					: signal{signalPointer}
					, id{idValue} {
				}

				SignalInterface* signal;
				const int id;
			};

			using InfoPtr = std::shared_ptr<Info>;
		
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
				: connection_{connection}  {
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
			~ScopedConnections() {
				connections_.clear();
			}

			void operator+=(const Connection& scopedConnection) {
				connections_.push_back(scopedConnection);
			}

			void operator+=(std::initializer_list<Connection> connections) {
				connections_.insert(connections_.end(), connections.begin(), connections.end());
				int a = 1;
			}

			void disconnectAll() {
				connections_.clear();
			}

		private:
			std::vector<ScopedConnection> connections_;
		};

	}

	// A function container, in which the functions stored can be called. A slot/callbacks class.
	template <class... A>
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
		
		template <class... Params>
		void operator()(Params&&... params);

		template <class... Params>
		void invoke(Params&&... params);

		void clear();

		int size() const noexcept;

		bool empty() const noexcept;

	private:
		using InfoPtr = signals::Connection::InfoPtr;

		void disconnect(int id) override;

		struct Pair {
			InfoPtr info;
			Callback callback;
		};

		std::vector<Pair> functions_; // All mapped callbacks.
		int id_{}; // The id mapped to the last added function.
	};

	template <class... Args>
	class MacroSignal {
	public:
		using Callback = typename Signal<Args...>::Callback;

		MacroSignal(Signal<Args...>* signal)
			: signal_{signal} {
		}

		[[nodiscard]]
		signals::Connection operator+=(const Callback& callback) {
			return connect(callback);
		}

		[[nodiscard]]
		signals::Connection connect(const Callback& callback) {
			if (signal_) {
				return signal_->connect(callback);
			}
			return {};
		}

	private:
		MacroSignal() = default;
		
		MacroSignal(MacroSignal&& publicSignal)
			: signal_{std::exchange(publicSignal.signal_, nullptr)} {
		}
		
		MacroSignal& operator=(MacroSignal&& publicSignal) {
			signal_ = std::exchange(publicSignal.signal_, nullptr);
			return *this;
		}

		Signal<Args...>* signal_{};
	};

	template <class Friend, class... Args>
	class PublicSignal {
	public:
		using Callback = typename Signal<Args...>::Callback;

		friend Friend;

		[[nodiscard]]
		signals::Connection operator+=(const typename Signal<Args...>::Callback& callback) {
			return signal_.connect(callback);
		}

		[[nodiscard]]
		signals::Connection connect(const Callback& callback) {
			return signal_.connect(callback);
		}

	private:
		PublicSignal() = default;
		
		PublicSignal(PublicSignal&& publicSignal) noexcept
			: signal_{std::move(signal_)} {
		}
		
		PublicSignal& operator=(PublicSignal&& publicSignal) noexcept {
			signal_ = std::move(signal_);
			return *this;
		}

		template <class... Params>
		void operator()(Params&&... params) {
			signal_.invoke(std::forward<Params>(params)...);
		}

		template <class... Params>
		void invoke(Params&&... params) {
			signal_.invoke(std::forward<Params>(params));
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

		Signal<Args...> signal_{};
	};

#define MW_SIGNAL(name, ...) \
	private: \
mw::Signal<##__VA_ARGS__> name ##_; \
public: \
	mw::MacroSignal<##__VA_ARGS__> ## name = &## name ## _; \

	// ------------ Definitions ------------

	inline void signals::Connection::disconnect() {
		if (info_ && info_->signal != nullptr) {
			info_->signal->disconnect(info_->id);
		}
	}
	
	inline bool signals::Connection::connected() const {
		return info_ && info_->signal != nullptr;
	}
	
	template <class... A>
	Signal<A...>::Signal::~Signal() {
		clear();
	}

	template <class... A>
	Signal<A...>::Signal(Signal<A...>&& signal) noexcept
		: functions_{std::move(signal.functions_)}
		, id_{std::exchange(signal.id_, 0)} {
	}
	
	template <class... A>
	Signal<A...>& Signal<A...>::operator=(Signal<A...>&& signal) noexcept {
		functions_ = std::move(signal.functions_);
		id_ = std::exchange(signal.id_, 0);
		return *this;
	}

	template <class... A>
	signals::Connection Signal<A...>::connect(const Callback& callback) {
		auto c = std::make_shared<signals::Connection::Info>(++id_, this);
		functions_.push_back({c, callback});
		return signals::Connection(c);
	}

	template <class... A>
	template <class... Params>
	void Signal<A...>::operator()(Params&&... a) {
		invoke(std::forward<Params>(a)...);
	}

	template <class... A>
	template <class... Params>
	void Signal<A...>::invoke(Params&&... a) {
		const auto size = static_cast<int>(functions_.size());
		// Using index instead of foreach in order to be able to add callbacks during iteration.
		for (int i = 0; i < size && i < functions_.size(); ++i) {
			auto& [_, callback] = functions_[i];
			callback(a...);
		}
	}

	template <class... A>
	void Signal<A...>::clear() {
		for (auto& [info, callback] : functions_) {
			info->signal = nullptr;
		}
		functions_.clear();
	}
	
	template <class... A>
	int Signal<A...>::size() const noexcept {
		return static_cast<int>(functions_.size());
	}

	template <class... A>
	bool Signal<A...>::empty() const noexcept{
		return functions_.empty();
	}

	template <class... A>
	void Signal<A...>::disconnect(int id) {
		auto size = static_cast<int>(functions_.size());
		// Using index instead of foreach in order to be able to add callbacks during iteration.
		for (int i = 0; i < size; ++i) {
			auto& pair = functions_[i];
			if (pair.info->id == id) {
				pair.info->signal = nullptr;
				std::swap(pair, functions_.back());
				functions_.pop_back();
				--size;
				break;
			}
		}
	}

}

#endif
