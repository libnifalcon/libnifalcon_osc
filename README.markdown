# libnifalcon OSC Server #

By Kyle Machulis (http://www.nonpolynomial.com) and Stephen Sinclair (http://www.music.mcgill.ca/~sinclair)

Version 1.0, September 21, 2009

## Description ##

libnifalcon_osc is a simple server application for accessing the Novint Falcon through the Open Sound Control protocol. This provides access to the falcon through all languages and applications with OSC bindings. While this is certainly not the preferred way of accessing the falcon (we recommend direct code integration with libnifalcon for that), it's certainly the easiest and good for rapid prototyping and evaluation.

## Requirements ##

* cmake
  * (http://www.cmake.org)
* libnifalcon (libnifalcon and libnifalcon\_cli\_base classes required)
  * (http://libnifalcon.nonpolynomial.com)
* liblo
  * (http://liblo.sourceforge.net)

## Applications Tested ##

* SuperCollider
  * http://supercollider.sourceforge.net
  * Works great, keeps up with 1khz requirement. SuperCollider example and class code in examples directory
* ChucK
  * http://chuck.cs.princeton.edu/
  * Works decently (a little slower than SC). Code in example directory
* Max
  * http://www.cycling74.com
  * Untested, not recommended. Use np_nifalcon external. Available at http://www.sourceforge.net/projects/libnifalcon
* PureData
  * http://www.puredata.info
  * Untested, not recommended. Use np_nifalcon external. Available at http://www.sourceforge.net/projects/libnifalcon


