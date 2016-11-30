# service-registration-tracker
Service Registration Tracker for libqi

Please find bellow an example of utilisation.

```c++
#include <qi/anymodule.hpp>
#include <qi/session.hpp>
#include <qi/log.hpp>

#include "serviceRegistrationTracker.hpp"

qiLogCategory("My Example Service");

class MyExampleService
{
public:
  MyExampleService(const qi::SessionPtr& session);

private:
  qi::SessionPtr _session;
  qi::AnyObject _theServiceINeed;
  ServiceRegistrationTracker _serRegTra;
};

MyExampleService::MyExampleService(const qi::SessionPtr& session)
  : _session(session)
  , _serRegTra(*session)
{
  // Function to be called when theServiceINeed is registered
  auto ontheServiceINeedRegistration = [this] {
    _theServiceINeed = _session->service("TheServiceINeed");
     qiLogInfo() << "Service I need registers";
    // Write all the code you want here (for example connect to signals)
  });
  
  // Function to be called when theServiceINeed is unregistered
  auto ontheServiceINeedUnregistration = [this] {
    qiLogInfo() << "Service I need unregisters";
    _octopusService.reset();
  };
  
  _serRegTra.addService("TheServiceINeed", ontheServiceINeedRegistration,
                         ontheServiceINeedUnregitration);
}
```

Important lines are :
```c++
#include "serviceRegistrationTracker.hpp"
```

```c++
 , _serRegTra(*session)
```

```c++
  _serRegTra.addService("TheServiceINeed", ontheServiceINeedRegistration,
                         ontheServiceINeedUnregitration);
```

Call the **addService** method for each service you need.

The **ServiceRegistrationTracker** will call **ontheServiceINeedRegistration** function :
* When the corresponding service registers.
* At the execution of **addService** if the corresponding service is already registered when calling this method.

The **ServiceRegistrationTracker** will call **ontheServiceINeedUnregistration** function :
* When the corresponding service unregisters.
