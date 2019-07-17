# Protocol Analyzer (NetProtoFuzz)
<h2><b>Protocol-oriented fuzz testing framework for network protocols.</b></h2>

[![license](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/Vitaliy-Grigoriev/Protocol-Analyzer/blob/master/LICENSE)
[![Coverity Scan](https://img.shields.io/coverity/scan/10369.svg)](https://scan.coverity.com/projects/vitaliy-grigoriev-protocol-analyzer)
[![Build Status](https://travis-ci.org/Vitaliy-Grigoriev/Protocol-Analyzer.svg?branch=master)](/Vitaliy-Grigoriev/Protocol-Analyzer)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/62752273b4bc42d7af29b182c97295d3)](https://www.codacy.com/app/vit.link420/Protocol-Analyzer?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=Vitaliy-Grigoriev/Protocol-Analyzer&amp;utm_campaign=Badge_Grade)
[![SonarQube Security Badge](https://sonarcloud.io/api/project_badges/measure?project=Protocol-Analyzer&metric=security_rating)](https://sonarcloud.io/api/project_badges/measure?project=Protocol-Analyzer&metric=security_rating)
[![SonarQube Vulnerabilities Badge](https://sonarcloud.io/api/project_badges/measure?project=Protocol-Analyzer&metric=vulnerabilities)](https://sonarcloud.io/api/project_badges/measure?project=Protocol-Analyzer&metric=vulnerabilities)
[![SonarQube Bags Badge](https://sonarcloud.io/api/project_badges/measure?project=Protocol-Analyzer&metric=bugs)](https://sonarcloud.io/api/project_badges/measure?project=Protocol-Analyzer&metric=bugs)
[![Lgtm alerts](https://img.shields.io/lgtm/alerts/g/Vitaliy-Grigoriev/Protocol-Analyzer.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/Vitaliy-Grigoriev/Protocol-Analyzer/alerts/)
[![Lgtm language grade: C/C++](https://img.shields.io/lgtm/grade/cpp/g/Vitaliy-Grigoriev/Protocol-Analyzer.svg?logo=lgtm&logoWidth=18)](https://lgtm.com/projects/g/Vitaliy-Grigoriev/Protocol-Analyzer/context:cpp)
[![CodeFactor](https://www.codefactor.io/repository/github/vitaliy-grigoriev/protocol-analyzer/badge)](https://www.codefactor.io/repository/github/vitaliy-grigoriev/protocol-analyzer)


<h3><b>Framework and Scanner under the deep development.</b> Version 0.0.12.</h3>

--------------
<h2><b>The main purposes of the framework</b></h2>

* Verification of the operation of proprietary network protocols in accordance with the declared description;

* Detection of undeclared capabilities and undefined behavior in the implementation of proprietary network protocols;

* Audit of security and search for changes from the last scan period.

--------------
<h2><b>Current goals</b></h2>

* Development the basis of network interaction for the needs of the framework;
  * Epoll Socket State Pool Interface for all Sockets under the observation;
  * Managed Socket Pool for unique Interface to take and control sockets;
  * UDP Socket Interface;
  * RAW Socket Interface;
  * ✓ Netlink Socket Interface for interact with Linux Kernel;
  * Create two error interfaces: POSIX-oriented, C++ exceptions.

* Development an interface for describing the structure of Network Protocol;
  * Implement libJSON as submodule of framework for parsing input configs.

* Development an interface for create Tasks and manage them;
* Development an interface for store Global information and Settings in runtime;
* ✓ Development main data class for working with binary data and build all infrastructure around it;
* Create a low-level and high-level API for analyzer library;
* ✓ Development Callback infrastructure through the framework.

--------------
<h2><b>Requirements for building the project</b></h2>

* OpenSSL >= 1.0.2h (libssl-dev) or 1.1.1 for using TLSv1.3;

* CMake >= 3.8.0 (for C++17 standard support);

* Compiler, supported full C++17 standard ('clang-6.0.0', 'gcc-7.0', icc-19.0 or higher).

--------------
<h2><b>Build</b></h2>

1. Go to 'build.sh' script.
2. Uncomment (define) the necessary parameters 'CXX' and 'BUILD_TYPE'.
3. Run the build script: 'chmod +x build.sh && ./build.sh'.
