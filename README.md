# Protocol Analyzer
<h2><b>Fuzz testing framework for network protocols.</b></h2>
--------------

[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/Vitaliy-Grigoriev/Protocol-Analyzer/blob/master/LICENSE)
[![Coverity Scan](https://img.shields.io/coverity/scan/10369.svg)](https://scan.coverity.com/projects/vitaliy-grigoriev-protocol-analyzer)
[![Build Status](https://travis-ci.org/Vitaliy-Grigoriev/Protocol-Analyzer.svg?branch=master)](/Vitaliy-Grigoriev/Protocol-Analyzer)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/62752273b4bc42d7af29b182c97295d3)](https://www.codacy.com/app/vit.link420/Protocol-Analyzer?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Vitaliy-Grigoriev/Protocol-Analyzer&amp;utm_campaign=Badge_Grade)

<h3><b>The framework under the deep development.</b> Version 0.0.8.</h3>

--------------
<h2><b>The main purposes of the framework</b></h2>

* Verification of the operation of proprietary network protocols in accordance with the declared description;

* Detection of undeclared capabilities and undefined behavior in the implementation of proprietary network protocols;

* Audit of security and search for changes from the last scan period.

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

* Compiler, supported C++17 ('clang-4.0.1', 'gcc-7.0' or higher).

