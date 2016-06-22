/*
 * clinkcache.h
 *
 *  Created on: 25.06.2013
 *      Author: andreas
 */

#ifndef CNETLINK_H_
#define CNETLINK_H_ 1

#include <netlink/cache.h>
#include <netlink/object.h>
#include <netlink/route/link.h>
#include <netlink/route/addr.h>
#include <netlink/route/neighbour.h>

#include <exception>

#include <roflibs/netlink/crtlinks.hpp>
#include <roflibs/netlink/clogging.hpp>

#include <rofl/common/cthread.hpp>

namespace rofcore {

class eNetLinkBase : public std::runtime_error {
public:
  eNetLinkBase(const std::string &__arg) : std::runtime_error(__arg){};
};
class eNetLinkCritical : public eNetLinkBase {
public:
  eNetLinkCritical(const std::string &__arg) : eNetLinkBase(__arg){};
};
class eNetLinkNotFound : public eNetLinkBase {
public:
  eNetLinkNotFound(const std::string &__arg) : eNetLinkBase(__arg){};
};
class eNetLinkFailed : public eNetLinkBase {
public:
  eNetLinkFailed(const std::string &__arg) : eNetLinkBase(__arg){};
};

class cnetlink_common_observer;

class cnetlink : public rofl::cthread_env {
  enum nl_cache_t {
    NL_LINK_CACHE,
    NL_NEIGH_CACHE,
  };

  enum timer {
    NL_TIMER_RESYNC,
  };

  rofl::cthread thread;
  struct nl_sock *sock;
  struct nl_cache_mngr *mngr;
  std::map<enum nl_cache_t, struct nl_cache *> caches;
  std::set<cnetlink_common_observer *> observers;

  crtlinks
      rtlinks; // all links in system => key:ifindex, value:crtlink instance

  std::set<int> missing_links;

public:
  std::map<int, crtneighs_ll> neighs_ll;

  friend std::ostream &operator<<(std::ostream &os, const cnetlink &netlink) {
    os << rofcore::indent(0) << "<cnetlink>" << std::endl;
    rofcore::indent i(2);
    os << netlink.rtlinks;
    return os;
  };

  static void route_link_cb(struct nl_cache *cache, struct nl_object *obj,
                            int action, void *data);

  static void route_neigh_cb(struct nl_cache *cache, struct nl_object *obj,
                             int action, void *data);

  static cnetlink &get_instance();

  void subscribe(cnetlink_common_observer *subscriber) {
    observers.insert(subscriber);
  };

  void unsubscribe(cnetlink_common_observer *subscriber) {
    observers.erase(subscriber);
  };

  const crtlinks &get_links() const { return rtlinks; };

  crtlinks &set_links() { return rtlinks; };

private:
  enum cnetlink_event_t {
    EVENT_NONE,
    EVENT_UPDATE_LINKS,
  };

  cnetlink();

  ~cnetlink() override;

  void init_caches();

  void destroy_caches();

  void handle_wakeup(rofl::cthread &thread) override{};

  void handle_read_event(rofl::cthread &thread, int fd) override;

  void handle_write_event(rofl::cthread &thread, int fd) override;

  void handle_timeout(rofl::cthread &thread, uint32_t timer_id,
                      const std::list<unsigned int> &ttypes) override;

  void set_neigh_timeout();

public:
  void notify_link_created(unsigned int ifindex);

  void notify_link_updated(const crtlink &newlink);

  void notify_link_deleted(unsigned int ifindex);

  void notify_neigh_ll_created(unsigned int ifindex, unsigned int adindex);

  void notify_neigh_ll_updated(unsigned int ifindex, unsigned int adindex);

  void notify_neigh_ll_deleted(unsigned int ifindex, unsigned int adindex);

  void add_neigh_ll(int ifindex, uint16_t vlan, const rofl::caddress_ll &addr);

  void drop_neigh_ll(int ifindex, uint16_t vlan, const rofl::caddress_ll &addr);
};

}; // end of namespace rofcore

#endif /* CLINKCACHE_H_ */
