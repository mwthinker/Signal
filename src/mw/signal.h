#ifndef MW_SIGNAL_H
#define MW_SIGNAL_H

#include "signals/connection.h"

#include <functional>
#include <vector>
#include <memory>

namespace mw {

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
		using ConnectionInfoPtr = std::shared_ptr<signals::ConnectionInfo>;

		void disconnect(int id) override {
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

		struct Pair {
			Pair(const Callback& callback, const ConnectionInfoPtr& connectionInfo) : callback_(callback), connectionInfo_(connectionInfo) {
			}

			ConnectionInfoPtr connectionInfo_;
			Callback callback_;
		};

		int id_; // The id mapped to the last added function.
		std::vector<Pair> functions_; // All mapped callbacks.
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
