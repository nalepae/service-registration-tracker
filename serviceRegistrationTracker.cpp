#include "serviceRegistrationTracker.hpp"

#include <boost/exception/diagnostic_information.hpp>

qiLogCategory("Service Registration Tracker");

namespace
{
  template<class Callback>
  void invokeCallback(Callback&& callback, std::string errorMessage)
  {
    try
    {
      callback();
    }
    catch (const std::exception& ex)
    {
     qiLogError() << "Service registration callback failed: " << errorMessage
                  << "\n  Details: " << ex.what();
    }
    catch (const boost::exception& ex)
    {
     qiLogError() << "Service registration callback failed: " << errorMessage
                  << "\n  Details: " << boost::diagnostic_information(ex);
    }
    catch (...)
    {
     qiLogError() << "Service registration callback failed: " << errorMessage
                  << "\n  Details: Unknown error";
    }
  }
}

ServiceRegistrationTracker::ServiceRegistrationTracker(qi::Session& session):
_session(session)
{
  // Registration
  _signalLinkRegister = session.serviceRegistered
  .connect([this] (int, const std::string service) {
    std::lock_guard<std::mutex> lock(_mutex);

    auto kv = _mapOfRegistrationFunction.find(service);
    if(kv != _mapOfRegistrationFunction.end())
      invokeCallback(kv->second, "Service registration callback");
  });

  // Unregistration
  _signalLinkUnregister = session.serviceUnregistered
  .connect([this] (int, const std::string service) {
    std::lock_guard<std::mutex> lock(_mutex);

    auto kv = _mapOfUnregistrationFunction.find(service);
    if(kv != _mapOfUnregistrationFunction.end())
      invokeCallback(kv->second, "Service unregistration callback");
  });
}

ServiceRegistrationTracker::~ServiceRegistrationTracker()
{
  _session.serviceRegistered.disconnect(_signalLinkRegister);
  _session.serviceUnregistered.disconnect(_signalLinkUnregister);
}

void ServiceRegistrationTracker::addService(std::string serviceName,
                                            std::function<void()> regFunc,
                                            std::function<void()> unregFunc)
{
  // Lock mutex
  std::lock_guard<std::mutex> lock(_mutex);

  // Fill the map
  _mapOfRegistrationFunction[serviceName] = regFunc;
  _mapOfUnregistrationFunction[serviceName] = unregFunc;

  if(_session.isConnected())
  {
    // Test if the service is already present
    qi::Future<qi::AnyObject> ft = _session.service(serviceName).async();
    if(!ft.hasError())
    {
      invokeCallback(regFunc, "Service registration on adding service");
    }
  }
}
