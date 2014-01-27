Simple ECC implementation for learning purposes.

This project will be implemented in three phases:

1. Implement primitives and ECC key generation.			[DONE]
2. Implement ECC general purpose encryption/decryption.	[IN PROGRESS]
3. Implement ECC sign/verify.							[DEFINED]

Implementation Notes:

This project took a bit more effort than I expected, largely due to the fact that I 
had to implement routines to manage numbers of arbitrary length. I also had some initial
difficulties with the modular arithmetic that outlined a gap in my assumptions around
modular addition.

To test out my implementation, I used a piece of software called "Sage" 
(http://www.sagemath.org/doc/index.html), which is a very high quality, easy to use,
mathematic system which is able to perform the operations I was attempting to perform.
Key to these are: math with integers of arbitrary length, calculations over finite fields,
and actual ECC operations on defined curves. I used these to create some test values and
to simply play around with some of the concepts s I could make sure I understood them.

Lastly, there are ~85 test cases to test out the various levels of the application. I used
a unit testing framework called Catch, which is simple to build and use (basically, it 
is distributed as a single header file) and builds to an executable (EccToolTests). Note:
this test executable, due to the operations it does internally, may take several seconds 
to complete. Be patient :)

Routines/Classes implemented:
I implemented a class called BigInteger which can do operations on integer of arbitrary
length. It is "reasonably" efficient (although I know it can be improved, given time). I
also took the time to implement nearly all integer math/manipulation related C++ operators
for this class. Example: All comparison operators (==, !=, <, >, <=, >=), all basic math
operators (+, +=, -, -=, *, *=, /, /=, %, %=), and one binary shift operator (<<=). These
operators made the mathematics much easier to program and to read in the code.

Possible improvements: 
- The implementation could be more efficient by implementing some of the more "clever" 
	theorems for binary or number manipulation.

I implemented a class for math over an elliptic curve (called EllipticCurve). This class
uses a set of domain parameters to define an elliptic curve, then can do elliptic curve
operations via a "Point" representation and the BigInteger class to handle the primitive
math. Mathematical operations mod n are implemented here (such as addition, subtraction,
division, and multiplication over a finite field). It uses these operations and several
formulas/algorithms to do operations on the elliptic curve, such as point addition, point
doubling, point inversion, and point multiplication by a scalar. It is also able to handle
operations with the defined "Point at Infinity". It is implemented generally,
so can support virtually all defined elliptic curve domain parameters.

Possible improvements:
- The mathematical operations mod n are done via functions which have to be called by 
	name, not as overloaded C++ operators. There may be a way to implement these in a 
	wrapper class around BigInteger (ModularInteger, etc.) which overloads the basic 
	math operators and could make the code more clear.
- The point operations are also done via named functions rather than operators. The reason
	they are this way is that they require both a point and a curve in order to run the
	operations. This could possibly be cleaned up by saving a reference to the curve in
	the point, so the point could then call the curve to do the operation.
	
On top of these, I implemented a class to handle the ECC algorithm operations (EccAlg) 
such as GenerateKey, LoadKey, SaveKey, etc. Future operations to be implemented involve
encryption/decryption, sign/verify, etc.

Possible improvements:
- I am not completely sure of the function to generate random large numbers. I believe 
	I will want to revisit this in further development.
	
This is all controlled by a simple command line interface to which more functionality
will be added later.