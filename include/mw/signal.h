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
			Connection(const Connection& Connection) = default;
			Connection(Connection&& Connection) noexcept = default;

			Connection& operator=(const Connection& Connection) = default;
			Connection& operator=(Connection&& Connection) noexcept = default;
			
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
				SignalInterface& operator=(const SignalInterface& SignalInterface) = delete;

				virtual void disconnect(int id) = 0;
			};

			struct ConnectionInfo {
				ConnectionInfo(size_t id, SignalInterface* signal) : signal_(signal), id_(id) {
				}

				SignalInterface* signal_;
				const size_t id_;
			};

			using ConnectionInfoPtr = std::shared_ptr<ConnectionInfo>;
		
			// Is called from mw::Signal to bind a connection.
			Connection(const ConnectionInfoPtr& c) : connectionInfo_(c) {
			}

			ConnectionInfoPtr connectionInfo_;
		};

	}

	// A function container, in which the functions stored can be called. A slot/callbacks class.
	template <class... A>
	class Signal : public signals::Connection::SignalInterface {
	public:
		using Callback = std::function<void(A...)>;

		Signal();
		~Signal();

		Signal(const Signal&) = delete;
		Signal& operator=(const Signal&) = delete;

		Signal(Signal&&) noexcept;
		Signal& operator=(Signal&&) noexcept;

		signals::Connection connect(const Callback& callback);
		
		void operator()(A... a);

		void clear();

		size_t size() const noexcept;

		bool empty() const noexcept;

	private:
		using ConnectionInfoPtr = signals::Connection::ConnectionInfoPtr;

		void disconnect(int id) override;

		struct Pair {
			Pair(const Callback& callback, const ConnectionInfoPtr& connectionInfo)
				: callback_(callback), connectionInfo_(connectionInfo) {
			}

			ConnectionInfoPtr connectionInfo_;
			Callback callback_;
		};

		std::vector<Pair> functions_; // All mapped callbacks.
		size_t id_; // The id mapped to the last added function.
	};

	// ------------ Definitions ------------

	inline void signals::Connection::disconnect() {
		if (connectionInfo_ && connectionInfo_->signal_ != nullptr) {
			connectionInfo_->signal_->disconnect(connectionInfo_->id_);
		}
	}
	
	inline bool signals::Connection::connected() const {
		return connectionInfo_ && connectionInfo_->signal_ != nullptr;
	}
	
	template <class... A>
	Signal<A...>::Signal() : id_(0) {
	}
	
	template <class... A>
	Signal<A...>::Signal::~Signal() {
		clear();
	}

	template <class... A>
	Signal<A...>::Signal(Signal<A...>&& signal) noexcept : functions_(std::move(signal.functions_)), id_(signal.id_) {
		signal.id_ = 0;
	}
	
	template <class... A>
	Signal<A...>& Signal<A...>::operator=(Signal<A...>&& signal) noexcept {
		functions_ = std::move(signal.functions_);
		id_ = signal.id_;
		signal.id_ = 0;
		return *this;
	}

	template <class... A>
	signals::Connection Signal<A...>::connect(const Callback& callback) {
		auto c = std::make_shared<signals::Connection::ConnectionInfo>(++id_, this);
		functions_.emplace_back(callback, c);
		return signals::Connection(c);
	}

	template <class... A>
	void Signal<A...>::operator()(A... a) {
		for (Pair& pair : functions_) {
			pair.callback_(a...);
		}
	}

	template <class... A>
	void Signal<A...>::clear() {
		for (Pair& pair : functions_) {
			pair.connectionInfo_->signal_ = nullptr;
		}
	}
	
	template <class... A>
	size_t Signal<A...>::size() const noexcept {
		return functions_.size();
	}

	template <class... A>
	bool Signal<A...>::empty() const noexcept{
		return functions_.empty();
	}

	template <class... A>
	void Signal<A...>::disconnect(int id) {
		// Remove id from vector.
		for (auto& pair : functions_) {
			if (pair.connectionInfo_->id_ == id) {
				pair.connectionInfo_->signal_ = nullptr;
				std::swap(pair, functions_.back());
				functions_.pop_back();
				break;
			}
		}
	}

} // Namespace mw.

#endif // SIGNAL_MW_SIGNAL_H
