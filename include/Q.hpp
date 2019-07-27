#pragma once

#include <functional>
#include <future>
#include <vector>

namespace Q
{

/**
 * @brief The main promise class.
 * 
 * @tparam Value The type of value stored in the promise.
 */
template <typename Value>
class Promise
{
public:
	/**
	 * @brief For readability.
	 * 
	 */
	typedef std::function<void(Value)> Callback;

	/**
	 * @brief Initialize the promise with a function to run.
	 * 
	 * @param fn The function to asynchronously run on initialization.
	 * 
	 */
	Promise(std::function<Value()> fn)
	{
		// Save the function.
		mFunction = fn;
	}

	/**
	 * @brief Attach a callback once the async function returns.
	 * 
	 * @param callback A callback to pass the value to once done.
	 * @return Promise& A reference to this for chaining.
	 */
	Promise& then(Callback callback)
	{
		// Append the callback to the list of callbacks
		mCallbacks.push_back(callback);
		return *this;
	}

	/**
	 * @brief Call after chaining then() to start the thread.
	 * 
	 */
	void exec()
	{
		// Launch the asynchronous promise thread.
		mTask = std::async(std::launch::async, [this]() -> void {
			// Run the function.
			Value v = mFunction();
			// Grab all then() callbacks, and pass the resulting value.
			for (auto& callback : mCallbacks)
			{
				callback(v);
			}
		});
	}

	/**
	 * @brief Pause until the task has completed. 
	 * 
	 */
	void wait()
	{
		mTask.wait();
	}

private:
	/// The function to asynchronously run.
	std::function<Value()> mFunction;
	/// The future instance.
	std::future<void> mTask;
	/// All callbacks to be chained once the future returns.
	std::vector<Callback> mCallbacks;
};

}