#ifndef SRC_ROFLIBS_OF_DPA_SWITCH_BEHAVIOR_OFDPA_HPP_
#define SRC_ROFLIBS_OF_DPA_SWITCH_BEHAVIOR_OFDPA_HPP_

#include <string>

#include <baseboxd/switch_behavior.hpp>

#include <roflibs/of-dpa/ofdpa_bridge.hpp>

#include "roflibs/netlink/cnetdev.hpp"
#include "roflibs/netlink/cnetlink.hpp"
#include "roflibs/netlink/cnetlink_observer.hpp"
#include "roflibs/netlink/ctapdev.hpp"

#include <rofl/common/locking.hpp>

namespace basebox {

class eSwitchBehaviorBaseErr : public std::runtime_error {
public:
  eSwitchBehaviorBaseErr(const std::string &__arg)
      : std::runtime_error(__arg) {}
};

class eLinkNoDptAttached : public eSwitchBehaviorBaseErr {
public:
  eLinkNoDptAttached(const std::string &__arg)
      : eSwitchBehaviorBaseErr(__arg) {}
};

class eLinkTapDevNotFound : public eSwitchBehaviorBaseErr {
public:
  eLinkTapDevNotFound(const std::string &__arg)
      : eSwitchBehaviorBaseErr(__arg) {}
};

class switch_behavior_ofdpa
    : public switch_behavior,
      public rofcore::cnetdev_owner,
      public rofcore::auto_reg_cnetlink_common_observer {

  enum ExperimenterMessageType {
    QUERY_FLOW_ENTRIES, ///< query flow entries from controller
    RECEIVED_FLOW_ENTRIES_QUERY
  };

  enum ExperimenterId {
    BISDN = 0xFF0000B0 ///< should be registered as ONF-Managed Experimenter ID
                       ///(OUI)
  };

public:
  switch_behavior_ofdpa(rofl::crofdpt &dpt);

  virtual ~switch_behavior_ofdpa();

  virtual void init() { init_ports(); }

  virtual int get_switch_type() { return 1; }

  virtual void handle_packet_in(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                                rofl::openflow::cofmsg_packet_in &msg);

  virtual void handle_flow_removed(rofl::crofdpt &dpt,
                                   const rofl::cauxid &auxid,
                                   rofl::openflow::cofmsg_flow_removed &msg);

  virtual void
  handle_experimenter_message(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                              rofl::openflow::cofmsg_experimenter &msg);

private:
  std::map<rofl::cdptid, std::map<std::string, rofcore::ctapdev *>> devs;
  rofl::rofl_ofdpa_fm_driver fm_driver;
  ofdpa_bridge bridge;
  rofl::crofdpt &dpt;
  mutable rofl::crwlock devs_rwlock;

  void init_ports();

  void handle_srcmac_table(const rofl::crofdpt &dpt,
                           rofl::openflow::cofmsg_packet_in &msg);

  void handle_acl_policy_table(const rofl::crofdpt &dpt,
                               rofl::openflow::cofmsg_packet_in &msg);

  void handle_bridging_table_rm(const rofl::crofdpt &dpt,
                                rofl::openflow::cofmsg_flow_removed &msg);

  void send_full_state(rofl::crofdpt &dpt);

  /**
   *
   */
  void clear_tap_devs(const rofl::cdptid &dpid) {
    rofl::AcquireReadWriteLock lock(devs_rwlock);
    while (not devs[dpid].empty()) {
      std::map<std::string, rofcore::ctapdev *>::iterator it =
          devs[dpid].begin();
      drop_tap_dev(dpid, it->first);
    }
  }

  /**
   *
   */
  rofcore::ctapdev &add_tap_dev(const rofl::cdptid &dpid,
                                const std::string &devname, uint16_t pvid,
                                const rofl::caddress_ll &hwaddr) {
    rofl::AcquireReadWriteLock lock(devs_rwlock);
    if (devs[dpid].find(devname) != devs[dpid].end()) {
      delete devs[dpid][devname];
    }
    devs[dpid][devname] =
        new rofcore::ctapdev(this, dpid, devname, pvid, hwaddr);
    return *(devs[dpid][devname]);
  }

  /**
   *
   */
  rofcore::ctapdev &set_tap_dev(const rofl::cdptid &dpid,
                                const std::string &devname, uint16_t pvid,
                                const rofl::caddress_ll &hwaddr) {
    rofl::AcquireReadWriteLock lock(devs_rwlock);
    if (devs[dpid].find(devname) == devs[dpid].end()) {
      devs[dpid][devname] =
          new rofcore::ctapdev(this, dpid, devname, pvid, hwaddr);
    }
    return *(devs[dpid][devname]);
  }

  /**
   *
   */
  rofcore::ctapdev &set_tap_dev(const rofl::cdptid &dpid,
                                const std::string &devname) {
    rofl::AcquireReadWriteLock lock(devs_rwlock);
    if (devs[dpid].find(devname) == devs[dpid].end()) {
      throw rofcore::eTapDevNotFound(
          "cbasebox::set_tap_dev() devname not found");
    }
    return *(devs[dpid][devname]);
  }

  /**
   *
   */
  rofcore::ctapdev &set_tap_dev(const rofl::cdptid &dpid,
                                const rofl::caddress_ll &hwaddr) {
    std::map<std::string, rofcore::ctapdev *>::iterator it;
    rofl::AcquireReadWriteLock lock(devs_rwlock);
    if ((it = find_if(devs[dpid].begin(), devs[dpid].end(),
                      rofcore::ctapdev::ctapdev_find_by_hwaddr(
                          dpid, hwaddr))) == devs[dpid].end()) {
      throw rofcore::eTapDevNotFound(
          "cbasebox::set_tap_dev() hwaddr not found");
    }
    return *(it->second);
  }

  /**
   *
   */
  const rofcore::ctapdev &get_tap_dev(const rofl::cdptid &dpid,
                                      const std::string &devname) const {
    rofl::AcquireReadLock lock(devs_rwlock);
    if (devs.find(dpid) == devs.end()) {
      throw rofcore::eTapDevNotFound("cbasebox::get_tap_dev() dpid not found");
    }
    if (devs.at(dpid).find(devname) == devs.at(dpid).end()) {
      throw rofcore::eTapDevNotFound(
          "cbasebox::get_tap_dev() devname not found");
    }
    return *(devs.at(dpid).at(devname));
  }

  /**
   *
   */
  const rofcore::ctapdev &get_tap_dev(const rofl::cdptid &dpid,
                                      const rofl::caddress_ll &hwaddr) const {
    rofl::AcquireReadLock lock(devs_rwlock);
    if (devs.find(dpid) == devs.end()) {
      throw rofcore::eTapDevNotFound("cbasebox::get_tap_dev() dpid not found");
    }
    std::map<std::string, rofcore::ctapdev *>::const_iterator it;
    if ((it = find_if(devs.at(dpid).begin(), devs.at(dpid).end(),
                      rofcore::ctapdev::ctapdev_find_by_hwaddr(
                          dpid, hwaddr))) == devs.at(dpid).end()) {
      throw rofcore::eTapDevNotFound(
          "cbasebox::get_tap_dev() hwaddr not found");
    }
    return *(it->second);
  }

  /**
   *
   */
  void drop_tap_dev(const rofl::cdptid &dpid, const std::string &devname) {
    if (devs[dpid].find(devname) == devs[dpid].end()) {
      return;
    }
    delete devs[dpid][devname];
    devs[dpid].erase(devname);
  }

  /**
   *
   */
  bool has_tap_dev(const rofl::cdptid &dpid, const std::string &devname) const {
    rofl::AcquireReadLock lock(devs_rwlock);
    if (devs.find(dpid) == devs.end()) {
      return false;
    }
    return (not(devs.at(dpid).find(devname) == devs.at(dpid).end()));
  }

  /**
   *
   */
  bool has_tap_dev(const rofl::cdptid &dpid,
                   const rofl::caddress_ll &hwaddr) const {
    rofl::AcquireReadLock lock(devs_rwlock);
    if (devs.find(dpid) == devs.end()) {
      return false;
    }
    std::map<std::string, rofcore::ctapdev *>::const_iterator it;
    if ((it = find_if(devs.at(dpid).begin(), devs.at(dpid).end(),
                      rofcore::ctapdev::ctapdev_find_by_hwaddr(
                          dpid, hwaddr))) == devs.at(dpid).end()) {
      return false;
    }
    return true;
  }

  /* IO */
  void enqueue(rofcore::cnetdev *netdev, rofl::cpacket *pkt);

  void enqueue(rofcore::cnetdev *netdev, std::vector<rofl::cpacket *> pkts);

  /* netlink */
  void link_created(unsigned int ifindex) noexcept override;

  void link_updated(const rofcore::crtlink &newlink) noexcept override;

  void link_deleted(unsigned int ifindex) noexcept override;

  void neigh_ll_created(unsigned int ifindex,
                        uint16_t nbindex) noexcept override;

  void neigh_ll_updated(unsigned int ifindex,
                        uint16_t nbindex) noexcept override;

  void neigh_ll_deleted(unsigned int ifindex,
                        uint16_t nbindex) noexcept override;
};
}
/* namespace basebox */

#endif /* SRC_ROFLIBS_OF_DPA_SWITCH_BEHAVIOR_OFDPA_HPP_ */
