#ifndef YAGA_UTILITY_UPDATE_NOTIFIER
#define YAGA_UTILITY_UPDATE_NOTIFIER

#include <boost/signals2.hpp>

namespace yaga
{

template<typename T>
class UpdateNotifier
{
public:
  typedef typename boost::signals2::signal<void(T)>::slot_type Handler;
  typedef boost::signals2::connection Connection;
  Connection onUpdate(const Handler& handler) { return signal_.connect(handler); }
  void onUpdate(const Connection& handler) { return signal_.disconnect(handler); }
protected:
  void fireUpdate(T id) { signal_(id); }
  template<typename P>
  void fireUpdate(P id) { signal_(static_cast<T>(id)); }
private:
  boost::signals2::signal<void(T)> signal_;
};

} // !namespace yaga

#endif // !YAGA_UTILITY_UPDATE_NOTIFIER
