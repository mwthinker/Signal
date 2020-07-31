#ifndef SIGNAL_MW_SIGNAL_H
#define SIGNAL_MW_SIGNAL_H

#include <vector>
#include <memory>
#include <functional>

namespace mw {

	template <class...>
	class Signal;

	namespace signals {

		// A connection Object remembers a connection. Automatically disconnects when
		// all copies goes out of scope.
		class Connection {
		public:
			template <typename...> friend class ::mw::Signal;

			Connection() = default;
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
			Connection(const InfoPtr& c)
				: info_{c} {
			}

			InfoPtr info_;
		};

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
		const auto size = static_cast<int>(functions_.size());
		// Using index instead of foreach in order to be able to add callbacks during iteration.
		for (int i = 0; i < size; ++i) {
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
		// Remove id from vector.
		for (auto& pair : functions_) {
			if (pair.info->id == id) {
				pair.info->signal = nullptr;
				std::swap(pair, functions_.back());
				functions_.pop_back();
				break;
			}
		}
	}

}

#endif
