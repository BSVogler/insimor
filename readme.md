## Motivation	
A Reinforcement Learning (RL) loop with a spiking neural network consists of several systems in a feedback-loop.

In my master thesis I showed how this RL framework is underlying many approaches to solve tasks with SNN. Because neuromorphic hardware is still experimental and not widely available it is easier to simulate it on von Neumann computer architectures. I used the established SNN framework NEST for the SNN back-end. It turned out that this is not optimised for fast closed loop simulations and simulation time is too slow to run on embedded processors for applications in robotics. In the diagram the SNN was implemented with NEST (the implementation). The required frequent updates every couple milliseconds on the edge of this system caused the slow speed by requiring data pushed to the back-end.
Variable, 2-50 ms

We attempt to solve this problem by writing a custom implementation optimised for this framework. One optimisation comes from the integration of all systems into the simulator back-end.

Another optimisation comes from multithreading. The simulation can run as a co-process where parameters are configured. The input is obtained by sensor data with a high frequency. The simulation typically is the bottleneck, thus it also runs constantly and taking the last input. It is expected that the performance impact of S2A is marginal. If not it could run as a co-process once a simulation has been performed. Further research is needed to evaluate the agents performance if the thread can read intermediate results or if it better to only use complete but slightly outdated results. Same is with the feedback function.


The system is then called:
Input, simulation, output, reward or utility function, short insimor.

## Project implementation details
There are two ways to integrate python with C/C++.
This projects contains a c-python/ctypes version and an extension (use with python import statement). Each version is split into its distinct directory.

The python extension is not working yet with python as this needs manual memory managment like reference counting.

For compiling the extension you need to link the libpython3.13.dylib (which is just a link to the python bin?).
First compile the C++ code of the engine to obtain the shared lib `libinsimou.dylib`.

Have an alias to the `libinsimou.dylib` in the same dir.

To test run `python3 testlib.py`


You can run the C++ code (main) when compiling and running the extension project in XCode. 



TODO:
embed back-end in cart-pole

### Performance Numbers
	Cycles/ms	
C++/Insimor core loop	51953,5	48,5278
Insimor python loop	0,613	
Python/NumericPyActor	0,89	
