#ifndef MW_SIGNAL_H
#define MW_SIGNAL_H

#include "signals/connection.h"

#include <functional>
#include <list>
#include <memory>

namespace mw {

	// Is a class that holds functions. A slot/callbacks class.
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
		using ConnectionInfoPtr = std::shared_ptr<signals::ConnectionInfo>;

		void disconnect(int id) override {
			functions_.remove_if([&](Pair& pair) {
				if (pair.connectionInfo_->id_ == id) {
					pair.connectionInfo_->signal_ = nullptr;
					return true;
				}
				return false;
			});
		}

		struct Pair {
			Pair(const Callback& callback, const ConnectionInfoPtr& connectionInfo) : callback_(callback), connectionInfo_(connectionInfo) {
			}

			ConnectionInfoPtr connectionInfo_;
			Callback callback_;
		};

		int id_; // The id mapped to the last added function.
		std::list<Pair> functions_; // All mapped callbacks.
	};
	
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
		ConnectionInfoPtr c = std::make_shared<signals::ConnectionInfo>(++id_, this);
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

} // Namespace mw.

#endif // MW_SIGNAL_H
