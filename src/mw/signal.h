#ifndef MW_SIGNAL_H
#define MW_SIGNAL_H

#include <functional>
#include <vector>
#include <memory>

namespace mw {

	template <class... A>
	class Signal;

	namespace signals {

		class Connection;

		// Is used by mw::signal. Is not to be used elsewhere.
		class SignalInterface {
		protected:
			friend class Connection;

			SignalInterface() = default;
			~SignalInterface() = default;

		private:
			SignalInterface(const SignalInterface&) = delete;
			SignalInterface& operator=(const SignalInterface& SignalInterface) = delete;

			virtual void disconnect(int id) = 0;
		};

		// A connection Object remembers a connection and gives infomation
		// if the connection is active or not.		
		class Connection {
		public:
			// Creates a empty connection. By default the connection is not active.
			Connection() = default;

			// Disconnect the active connection. The callback associated to this connection
			// will disconnect from the corresponding slot.
			void disconnect();

			// Returns true if the connection is still active else false.
			bool connected() const;

		private:
			template<class... A> friend class Signal;

			struct ConnectionInfo {
				ConnectionInfo(int id, SignalInterface* signal) : signal_(signal), id_(id) {
				}

				SignalInterface* signal_;
				const int id_;
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
	class Signal : public signals::SignalInterface {
	public:
		using Callback = std::function<void(A...)>;

		Signal();
		~Signal();

		signals::Connection connect(const Callback& callback);
		
		void operator()(A... a);

		void clear();

		int size() const;

	private:
		using ConnectionInfoPtr = std::shared_ptr<signals::Connection::ConnectionInfo>;

		void disconnect(int id) override;

		struct Pair {
			Pair(const Callback& callback, const ConnectionInfoPtr& connectionInfo) : callback_(callback), connectionInfo_(connectionInfo) {
			}

			ConnectionInfoPtr connectionInfo_;
			Callback callback_;
		};

		int id_; // The id mapped to the last added function.
		std::vector<Pair> functions_; // All mapped callbacks.
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
	Signal<A...>::Signal() {
		id_ = 0;
	}
	
	template <class... A>
	Signal<A...>::Signal::~Signal() {
		clear();
	}

	template <class... A>
	signals::Connection Signal<A...>::connect(const Callback& callback) {
		ConnectionInfoPtr c = std::make_shared<signals::Connection::ConnectionInfo>(++id_, this);
		functions_.push_back(Pair(callback, c));
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
	int Signal<A...>::size() const {
		return functions_.size();
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

#endif // MW_SIGNAL_H
