# Q

POSIX Single-Header Basic NodeJS promises in C++.

## Usage

Include the Q.hpp header, and use your cxx compiler's `-pthread` option to link the posix threading library.

## Example

#include <iostream>
#include "Q.hpp"

```cpp
#include <iostream>
#include "Q.hpp"

int main()
{
	// Create a promise to execute a really long calculation.
	Q::Promise<double> p([]() -> double {
		double res = 0;
		for (double i = 0; i < 1000000000; ++i)
		{
			res += i;
		}
		return res;
	});
	// Callback once the promise returns.
	p.then([](double value) -> void {
		 std::cout << "Got value " << value << "\n";
	 }).exec();   //< Execute the callback.

	// Do other statements while the callback runs..
	std::cout << "Running long calculation...\n";
	// Stall current thread until the promise returns, and the then() clause executes.
	p.wait();

	std::cout << "Calculation finished.\n";

	return 0;
}
```

## TODO

* ~~`.catch();`~~ Implemented as `.fail();`