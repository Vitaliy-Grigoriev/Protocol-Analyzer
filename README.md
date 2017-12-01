# Protocol Analyzer
<h1><b>Fuzz testing framework for network protocols.</b></h1>
<h2>The main purposes of this framework:</h2>

<b>1.</b> Verification of the operation of proprietary network protocols in accordance with the declared description;

<b>2.</b> Detection of undeclared capabilities and undefined behavior in the implementation of proprietary network protocols;

<b>3.</b> Audit of security and search for changes from the last scan period.

--------------

[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/Vitaliy-Grigoriev/Protocol-Analyzer/blob/master/LICENSE)
[![Coverity Scan](https://img.shields.io/coverity/scan/10369.svg)](https://scan.coverity.com/projects/vitaliy-grigoriev-protocol-analyzer)
[![Build Status](https://travis-ci.org/Vitaliy-Grigoriev/Protocol-Analyzer.svg?branch=master)](/Vitaliy-Grigoriev/Protocol-Analyzer)

<h3><b>The framework under the deep development.</b> Version 0.0.8.</h3>

--------------

<h2><b>Current goals</b></h2>

* Development the basis of network interaction for the needs of the framework;
  * Epoll Socket State Pool interface for all Sockets under the observation;
  * Managed Socket Pool for unique interface to take and control sockets;
  * UDP Socket interface;
  * RAW Socket interface;
  * Netlink interface for interact with Linux Kernel;
  * Create two error interfaces: POSIX-oriented, C++ exceptions.

* Development an unique interface for describing the Network Protocol;
  * Implement libJSON as submodule of framework for parsing input configs.

* Development an unique interface for create Tasks and manage them;
* Development an unique interface for store Global information and Settings in runtime;
* Development main data class for working with binary data and build all infrastructure around it.

--------------

<h2><b>Requirements for building</b></h2>

<h4>The following package is required to build the Protocol Analyzer:</h4>

* OpenSSL >= 1.0.2h (libssl-dev)

* CMake >= 3.8.0 (for C++17 standard support)

* Compiler, supported C++17 (for example: 'clang-4.0.1' or higher).

