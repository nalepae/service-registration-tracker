#ifndef SERVICEREGISTRATIONTRACKER_HPP
#define SERVICEREGISTRATIONTRACKER_HPP

#include <mutex>
#include <qi/session.hpp>

class ServiceRegistrationTracker
{
public:
  ServiceRegistrationTracker(qi::Session& session);
  ~ServiceRegistrationTracker();

  void addService(std::string serviceName, std::function<void()> regisFunc,
                  std::function<void()> unRegisFunc);

private:
  std::map<std::string, std::function<void()>> _mapOfRegistrationFunction;
  std::map<std::string, std::function<void()>> _mapOfUnregistrationFunction;

  qi::SignalLink _signalLinkRegister = qi::SignalBase::invalidSignalLink;
  qi::SignalLink _signalLinkUnregister = qi::SignalBase::invalidSignalLink;

  qi::Session& _session;

  std::mutex _mutex;
};

#endif // SERVICEREGISTRATIONTRACKER_HPP
