# safebikesimframework
This is the Safebike Training Simulator's core layer over which the Safebike Training Simulator have been developed.

This software layer has been developed in order to have an uniform SDK implementing all operations needed for implementing a distributed simulation architecture.

You can find a complete **doxygen documentation** [here](http://robertosartoridev.com/doxygen/safebikesimframework/namespaces.html).

Briefly, this the list of the frameworks included in this SDK:

### [FrameworkCore](http://robertosartoridev.com/doxygen/safebikesimframework/namespaceFrameworkCore.html)

Implements the core functions like error managements, tracing, inter-process comunications, etc.
The implemented classes are:

* **CError**: error management class
* **CExtendedString**: extend standard std::string with useful operations
* **CGenericType**: implement a dynamc-binding variable plus dynamic structure vatriables using a special syntax
* **CSharedStruct**: inter-process shared structures
* **CSynch**: synchronization implemented at three levels, object instance level, intra-process level and inter-process level.
* **CThread**: multi-thread management with rich features
* **CTime**: high-precision tic/toc timing functions
* **CTrace**: messages trace management

### [FrameworkEnvironment](http://robertosartoridev.com/doxygen/safebikesimframework/namespaceFrameworkEnvironment.html)
Implements a class for the management of a workspace.
The implemented classes are:

* **CEnvironment**: implements an environment object which can be used to set/get variables, save to/load from file, merge two environments, auto-trace environment changes, lock/unlock for multthread-safe operations.

### [FrameworkCANBus](http://robertosartoridev.com/doxygen/safebikesimframework/namespaceFrameworkCanBus.html)

Implements a set of classes to configure, modelize, read/write a CAN network offering a high-level modelling of the network configuration.
With the classes of this framework you can programmatically describe the CAN network by defininng the ECUs, the messages mapped on the ECUs and the signals packing mapping.
The library will pack/unpack signals, synch to signals, synch to messages, let to define callbacks to be called when some messages or signals are received, write messages or signals to the CAN network.

### [FrameworkIPC](http://robertosartoridev.com/doxygen/safebikesimframework/namespaceFrameworkIPC.html)

Implements the Inter-Process Comunication layer. The implemented classes are:

* **CP2P**: low-level inter-process comunication, connect/disconnect and read/write operations are implemented in this class
* **CIPC**: high-level application layer for inter-process comunication, implements the concept of communication channels and publish data to channels and synch for data receiving events from a channel.

### [FrameworkLUA](http://robertosartoridev.com/doxygen/safebikesimframework/namespaceFrameworkLUA.html)

Implements LUA script parsing. The implemented classes are:

* **CWrapperLua**: LUA wrapper class, implements all needed to read/run scripts and read/write vars into the LUA environment

### [FrameworkMath](http://robertosartoridev.com/doxygen/safebikesimframework/namespaceFrameworkMath.html)

Implements some math specifically needed for the road management like 2D points interpolation, etc.

### [FrameworkPlugin](http://robertosartoridev.com/doxygen/safebikesimframework/namespaceFrameworkPlugin.html)

Implements a mechanism for plugin implementation. The implemented classes are:

* **CPlugin**: his is the abstract class to be inerhited to create a plugin
* **CpluginManager**: this class implements the plugin manager that scan for/loads/initialize/unloads the plugins

### [FrameworkQueueTools](http://robertosartoridev.com/doxygen/safebikesimframework/namespaceFrameworkQueueTools.html)

Implements some classes for the management of command queues. Command queues are queue where objects are command to be executed, the management is basically to add commands asynchronously and let the queue to process such command list anytime is needed. The implemented classes are:

* **CCommandQueueableClass**: this class is used to implement a queueable commands
* **CCommandQueueManager**: manager for the command queue, allows to push new commands and process the queue when needed

### [FrameworkSimArc](http://robertosartoridev.com/doxygen/safebikesimframework/namespaceFrameworkSimArc.html)

Implements the classes needed for coordinating a simulation. In particular, an instance CSimControl class will have the simulation control role and it will initialize/start/pause/stop the whole simulation processes while each simunlation process will have a single instance of the CSimProcess class to be controlled by the simulation controller. The implemented classes are:

* **CSimControl**: controls the simulation by initializing/starting/pausing/stopping the simulation processes
* **CSimProcess**: implements a process that can be controlled by the simulation controller
* **CSimProcessDelegate**: this instance will receive the events by the CSimProcess instance in order to react to the initialize/start/pause/stop messages.
