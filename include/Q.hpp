#pragma once

#include <functional>
#include <future>
#include <string>
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
	/// Callback type called on .then();
	typedef std::function<void(Value)> Callback;
	/// Callback type called on .fail();
	typedef std::function<void(std::string)> ErrorCallback;

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
	 * @brief Attach a callback to be called on an exception.
	 * 
	 * @param callback The callback to attach. Passed exception details as a string.
	 * @return Promise& Reference to this for chaining.
	 */
	Promise& fail(ErrorCallback callback)
	{
		mErrorCallbacks.push_back(callback);
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
			Value v;

			// Tracking if any of the many catch() statements fire.
			bool errored = false;
			std::string errorMsg = "";
			try
			{
				v = mFunction();
			}
			catch (const std::exception& e)
			{
				errored = true;
				errorMsg = e.what();
			}
			catch (const std::string& s)
			{
				errored = true;
				errorMsg = s;
			}
			catch (...)
			{
				errored = true;
				errorMsg = "Unknown exception type.";
			}

			// If we errored, call all error callbacks, and return.
			if (errored)
			{
				for (auto& callback : mErrorCallbacks)
				{
					callback(errorMsg);
				}
				return;
			}

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
	/// Callbacks to call on an error.
	std::vector<ErrorCallback> mErrorCallbacks;
};

}