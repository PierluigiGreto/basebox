/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <cassert>
#include <cerrno>
#include <linux/if_ether.h>

#include "cbasebox.hpp"

#include "roflibs/netlink/cpacketpool.hpp"
#include "roflibs/of-dpa/ofdpa_datatypes.hpp"

namespace basebox {

using rofcore::logging;

struct vlan_hdr {
  struct ethhdr eth; // vid + cfi + pcp
  uint16_t vlan;     // ethernet type
} __attribute__((packed));

/*static*/ bool cbasebox::keep_on_running = true;

void cbasebox::handle_dpt_open(rofl::crofdpt &dpt) {

  if (rofl::openflow13::OFP_VERSION < dpt.get_version()) {
    logging::error << "[cbasebox][handle_dpt_open] datapath "
                   << "attached with invalid OpenFlow protocol version: "
                   << (int)dpt.get_version() << std::endl;
    return;
  }

#ifdef DEBUG
  dpt.set_conn(rofl::cauxid(0))
      .set_trace(true)
      .set_journal()
      .log_on_stderr(true)
      .set_max_entries(64);
  dpt.set_conn(rofl::cauxid(0))
      .set_tcp_journal()
      .log_on_stderr(true)
      .set_max_entries(16);
#endif

  logging::debug << "[cbasebox][handle_dpt_open] dpid: " << dpt.get_dpid().str()
                 << std::endl;
  logging::debug << "[cbasebox][handle_dpt_open] dpt: " << dpt << std::endl;

  dpt.send_features_request(rofl::cauxid(0));
  dpt.send_desc_stats_request(rofl::cauxid(0), 0);
  dpt.send_port_desc_stats_request(rofl::cauxid(0), 0);

  // todo timeout?
}

void cbasebox::handle_wakeup(rofl::cthread &thread) {
  logging::crit << "[cbasebox][handle_wakeup] XXX not implemented" << std::endl;
}

void cbasebox::handle_dpt_close(const rofl::cdptid &dptid) {
  logging::notice << "[cbasebox][handle_dpt_close] dptid: " << dptid.str()
                  << std::endl;
}

void cbasebox::handle_conn_terminated(rofl::crofdpt &dpt,
                                      const rofl::cauxid &auxid) {
  logging::crit << "[cbasebox][" << __FUNCTION__ << "]: XXX not implemented"
                << std::endl;
}

void cbasebox::handle_conn_refused(rofl::crofdpt &dpt,
                                   const rofl::cauxid &auxid) {
  logging::crit << "[cbasebox][" << __FUNCTION__ << "]: XXX not implemented"
                << std::endl;
}

void cbasebox::handle_conn_failed(rofl::crofdpt &dpt,
                                  const rofl::cauxid &auxid) {
  logging::crit << "[cbasebox][" << __FUNCTION__ << "]: XXX not implemented"
                << std::endl;
}

void cbasebox::handle_conn_negotiation_failed(rofl::crofdpt &dpt,
                                              const rofl::cauxid &auxid) {
  logging::crit << "[cbasebox][" << __FUNCTION__ << "]: XXX not implemented"
                << std::endl;
}

void cbasebox::handle_conn_congestion_occured(rofl::crofdpt &dpt,
                                              const rofl::cauxid &auxid) {
  logging::crit << "[cbasebox][" << __FUNCTION__ << "]: XXX not implemented"
                << std::endl;
}

void cbasebox::handle_conn_congestion_solved(rofl::crofdpt &dpt,
                                             const rofl::cauxid &auxid) {
  logging::crit << "[cbasebox][" << __FUNCTION__ << "]: XXX not implemented"
                << std::endl;
}

void cbasebox::handle_features_reply(
    rofl::crofdpt &dpt, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_features_reply &msg) {
  logging::debug << "[cbasebox][handle_features_reply] dpid: "
                 << dpt.get_dpid().str() << std::endl
                 << msg;
}

void cbasebox::handle_desc_stats_reply(
    rofl::crofdpt &dpt, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_desc_stats_reply &msg) {
  logging::debug << "[cbasebox][handle_desc_stats_reply] dpt: " << std::endl
                 << dpt << std::endl
                 << msg;
}

void cbasebox::handle_packet_in(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                                rofl::openflow::cofmsg_packet_in &msg) {
  logging::debug << "[cbasebox][handle_packet_in] dpid: "
                 << dpt.get_dpid().str() << " pkt received: " << std::endl
                 << msg;

  logging::debug << __FUNCTION__ << ": handle message" << std::endl << msg;

#if 0 // XXX FIXME check if needed
  if (this->dptid != dpt.get_dptid()) {
    logging::error << "[cbasebox][" << __FUNCTION__
                   << "] wrong dptid received" << std::endl;
    return;
  }
#endif

  switch (msg.get_table_id()) {
  case OFDPA_FLOW_TABLE_ID_SA_LOOKUP:
    this->handle_srcmac_table(dpt, msg);
    break;

  case OFDPA_FLOW_TABLE_ID_ACL_POLICY:
    this->handle_acl_policy_table(dpt, msg);
    break;
  default:
    break;
  }
}

void cbasebox::handle_flow_removed(rofl::crofdpt &dpt,
                                   const rofl::cauxid &auxid,
                                   rofl::openflow::cofmsg_flow_removed &msg) {
  logging::debug << "[cbasebox][handle_flow_removed] dpid: "
                 << dpt.get_dpid().str() << " pkt received: " << std::endl
                 << msg;

#if 0 // XXX FIXME check if needed
  if (this->dptid != dpt.get_dptid()) {
    logging::error << "[cbasebox][" << __FUNCTION__
                   << "] wrong dptid received" << std::endl;
    return;
  }
#endif

  switch (msg.get_table_id()) {
  case OFDPA_FLOW_TABLE_ID_BRIDGING:
    this->handle_bridging_table_rm(dpt, msg);
    break;
  default:
    break;
  }
}

void cbasebox::handle_port_status(rofl::crofdpt &dpt, const rofl::cauxid &auxid,
                                  rofl::openflow::cofmsg_port_status &msg) {
  logging::debug << "[cbasebox][handle_port_status] dpid: "
                 << dpt.get_dpid().str() << " pkt received: " << std::endl
                 << msg;

  // XXX FIXME not implemented
  logging::warn << __FUNCTION__ << ": not implemented" << std::endl;
}

void cbasebox::handle_error_message(rofl::crofdpt &dpt,
                                    const rofl::cauxid &auxid,
                                    rofl::openflow::cofmsg_error &msg) {
  logging::info << "[cbasebox][handle_error_message] dpid: "
                << dpt.get_dpid().str() << " pkt received: " << std::endl
                << msg;

  // XXX FIXME not implemented
  logging::warn << __FUNCTION__ << ": not implemented" << std::endl;
}

void cbasebox::handle_port_desc_stats_reply(
    rofl::crofdpt &dpt, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_port_desc_stats_reply &msg) {

  logging::debug << "[cbasebox][handle_port_desc_stats_reply] dpid: "
                 << dpt.get_dpid().str() << " pkt received: " << std::endl
                 << msg;
  init(dpt);
}

void cbasebox::handle_port_desc_stats_reply_timeout(rofl::crofdpt &dpt,
                                                    uint32_t xid) {

  logging::debug << "[cbasebox][handle_port_desc_stats_reply_timeout] dpid: "
                 << dpt.get_dpid().str() << std::endl;
}

void cbasebox::handle_experimenter_message(
    rofl::crofdpt &dpt, const rofl::cauxid &auxid,
    rofl::openflow::cofmsg_experimenter &msg) {

  logging::debug << "[cbasebox][" << __FUNCTION__
                 << "] dpid: " << dpt.get_dpid().str()
                 << " pkt received: " << std::endl
                 << msg;

  uint32_t experimenterId = msg.get_exp_id();
  uint32_t experimenterType = msg.get_exp_type();
  uint32_t xidExperimenterCAR = msg.get_xid();

  if (experimenterId == BISDN) {
    switch (experimenterType) {
    case QUERY_FLOW_ENTRIES:
      dpt.send_experimenter_message(auxid, xidExperimenterCAR, experimenterId,
                                    RECEIVED_FLOW_ENTRIES_QUERY);
      nbi->resend_state();
      break;
    }
  }
}

void cbasebox::handle_srcmac_table(rofl::crofdpt &dpt,
                                   rofl::openflow::cofmsg_packet_in &msg) {
#if 0 // XXX FIXME currently disabled
  using rofl::openflow::cofport;
  using rofcore::cnetlink;
  using rofcore::crtlink;
  using rofl::caddress_ll;

  logging::info << __FUNCTION__ << ": in_port=" << msg.get_match().get_in_port()
                << std::endl;

  struct ethhdr *eth = (struct ethhdr *)msg.get_packet().soframe();

  uint16_t vlan = 0;
  if (ETH_P_8021Q == be16toh(eth->h_proto)) {
    vlan = be16toh(((struct vlan_hdr *)eth)->vlan) & 0xfff;
    logging::debug << __FUNCTION__ << ": vlan=0x" << std::hex << vlan
                   << std::dec << std::endl;
  }

  // TODO this has to be improved
  const cofport &port = dpt.get_ports().get_port(msg.get_match().get_in_port());
  const crtlink &rtl =
      cnetlink::get_instance().get_links().get_link(port.get_name());

  if (0 == vlan) {
    vlan = rtl.get_pvid();
  }

  // update bridge fdb
  try {
    const caddress_ll srcmac(eth->h_source, ETH_ALEN);
    cnetlink::get_instance().add_neigh_ll(rtl.get_ifindex(), vlan, srcmac);
    bridge.add_mac_to_fdb(dpt, msg.get_match().get_in_port(), vlan, srcmac,
                          false);
  } catch (rofcore::eNetLinkNotFound &e) {
    logging::notice << __FUNCTION__ << ": cannot add neighbor to interface"
                    << std::endl;
  } catch (rofcore::eNetLinkFailed &e) {
    logging::crit << __FUNCTION__ << ": netlink failed" << std::endl;
  }
#endif
}

void cbasebox::handle_acl_policy_table(rofl::crofdpt &dpt,
                                       rofl::openflow::cofmsg_packet_in &msg) {
  using rofl::openflow::cofport;

  try {
    const cofport &port =
        dpt.get_ports().get_port(msg.get_match().get_in_port());

    rofl::cpacket *pkt = rofcore::cpacketpool::get_instance().acquire_pkt();
    *pkt = msg.get_packet();

    tap_man->get_dev(of_port_to_port_id.at(port.get_port_no())).enqueue(pkt);
  } catch (rofcore::ePacketPoolExhausted &e) {
    logging::error << __FUNCTION__ << " ePacketPoolExhausted: " << e.what()
                   << std::endl;
  } catch (std::exception &e) {
    logging::error << __FUNCTION__ << " exception: " << e.what() << std::endl;
  }
}

void cbasebox::handle_bridging_table_rm(
    rofl::crofdpt &dpt, rofl::openflow::cofmsg_flow_removed &msg) {
#if 0 // XXX FIXME disabled for tapdev refactoring:
// this is used only for srcmac learning, which is disabled
  using rofl::cmacaddr;
  using rofl::openflow::cofport;
  using rofcore::cnetlink;
  using rofcore::ctapdev;

  logging::info << __FUNCTION__ << ": handle message" << std::endl << msg;

  cmacaddr eth_dst;
  uint16_t vlan = 0;
  try {
    eth_dst = msg.get_match().get_eth_dst();
    vlan = msg.get_match().get_vlan_vid() & 0xfff;
  } catch (rofl::openflow::eOxmNotFound &e) {
    logging::error << __FUNCTION__ << ": failed to get eth_dst or vlan"
                   << std::endl;
    return;
  }

  // TODO this has to be improved 
  uint32_t portno = msg.get_cookie();
  const cofport &port = dpt.get_ports().get_port(portno);
  const ctapdev &tapdev = get_tap_dev(dpt.get_dptid(), port.get_name());

  try {
    // update bridge fdb
    cnetlink::get_instance().drop_neigh_ll(tapdev.get_ifindex(), vlan,
    eth_dst);
  } catch (rofcore::eNetLinkFailed &e) {
    logging::crit << __FUNCTION__ << ": netlink failed: " << e.what()
                  << std::endl;
  }
#endif
}

void cbasebox::init(rofl::crofdpt &dpt) {
  using rofl::openflow::cofport;

  std::deque<std::string> ports;

  /* init 1:1 port mapping */
  try {
    for (const auto &i : dpt.get_ports().keys()) {
      const cofport &port = dpt.get_ports().get_port(i);
      ports.push_back(port.get_name());
    }

    std::deque<std::pair<int, std::string>> devs =
        tap_man->register_tapdevs(ports, *this);

    for (auto i : devs) {
      const rofl::openflow::cofport &port = dpt.get_ports().get_port(i.second);
      of_port_to_port_id[port.get_port_no()] = i.first;
      port_id_to_of_port[i.first] = port.get_port_no();
    }

    tap_man->start();

    logging::info << "ports initialized" << std::endl;

  } catch (std::exception &e) {
    logging::error << "[cbasebox][" << __FUNCTION__ << "] ERROR: unknown error "
                   << e.what() << std::endl;
  }
}

int cbasebox::enqueue(rofcore::ctapdev *tapdev, rofl::cpacket *pkt) {
  using rofl::openflow::cofport;
  using std::map;
  int rv = 0;

  assert(tapdev && "no tapdev");
  assert(pkt && "invalid enque");
  struct ethhdr *eth = (struct ethhdr *)pkt->soframe();

  if (eth->h_dest[0] == 0x33 && eth->h_dest[1] == 0x33) {
    logging::debug << "[cbasebox][" << __FUNCTION__
                   << "]: drop multicast packet" << std::endl;
    rv = -ENOTSUP;
    goto errout;
  }

  try {
    rofl::crofdpt &dpt = set_dpt(this->dptid, true);
    if (not dpt.is_established()) {
      logging::warn << "[cbasebox][" << __FUNCTION__
                    << "] not connected, dropping packet" << std::endl;
      rv = -ENOTCONN;
      goto errout;
    }

    // TODO move to separate function:
    uint32_t portno =
        dpt.get_ports().get_port(tapdev->get_devname()).get_port_no();

    /* only send packet-out if we can determine a port-no */
    if (portno) {
      logging::debug << "[cbasebox][" << __FUNCTION__
                     << "]: send pkt-out, pkt:" << std::endl
                     << *pkt;

      rofl::openflow::cofactions actions(dpt.get_version());
      //			//actions.set_action_push_vlan(rofl::cindex(0)).set_eth_type(rofl::fvlanframe::VLAN_CTAG_ETHER);
      //			//actions.set_action_set_field(rofl::cindex(1)).set_oxm(rofl::openflow::coxmatch_ofb_vlan_vid(tapdev->get_pvid()));
      actions.set_action_output(rofl::cindex(0)).set_port_no(portno);

      dpt.send_packet_out_message(
          rofl::cauxid(0),
          rofl::openflow::base::get_ofp_no_buffer(dpt.get_version()),
          rofl::openflow::base::get_ofpp_controller_port(dpt.get_version()),
          actions, pkt->soframe(), pkt->length());
    }
  } catch (rofl::eRofDptNotFound &e) {
    logging::error << "[cbasebox][" << __FUNCTION__
                   << "] no data path attached, dropping outgoing packet"
                   << std::endl;

  } catch (rofl::eRofBaseNotFound &e) {
    logging::crit << "[cbasebox][" << __FUNCTION__ << "]: " << e.what()
                  << std::endl;

  } catch (rofl::openflow::ePortsNotFound &e) {
    logging::error << __FUNCTION__ << ": invalid port for packet out"
                   << std::endl;
    rv = -EINVAL;
    goto errout;
  }

errout:

  rofcore::cpacketpool::get_instance().release_pkt(pkt);
  return rv;
}

int cbasebox::l2_addr_remove_all_in_vlan(uint32_t port, uint16_t vid) noexcept {
  int rv = 0;
  try {
    rofl::crofdpt &dpt = set_dpt(dptid, true);
    uint32_t of_port = port_id_to_of_port.at(port);
    fm_driver.remove_bridging_unicast_vlan_all(dpt, of_port, vid);
  } catch (rofl::eRofBaseNotFound &e) {
    // TODO log error
    rv = -EINVAL;
  }
  return rv;
}

int cbasebox::l2_addr_add(uint32_t port, uint16_t vid,
                          const rofl::cmacaddr &mac) noexcept {
  int rv = 0;
  try {
    rofl::crofdpt &dpt = set_dpt(dptid, true);
    // XXX have the knowlege here about filtered/unfiltered?
    uint32_t of_port = port_id_to_of_port.at(port);
    fm_driver.add_bridging_unicast_vlan(dpt, of_port, vid, mac, true, false);
  } catch (rofl::eRofBaseNotFound &e) {
    // TODO log error
    rv = -EINVAL;
  }
  return rv;
}

int cbasebox::l2_addr_remove(uint32_t port, uint16_t vid,
                             const rofl::cmacaddr &mac) noexcept {
  int rv = 0;
  try {
    rofl::crofdpt &dpt = set_dpt(dptid, true);
    uint32_t of_port = port_id_to_of_port.at(port);
    fm_driver.remove_bridging_unicast_vlan(dpt, of_port, vid, mac);
  } catch (rofl::eRofBaseNotFound &e) {
    // TODO log error
    rv = -EINVAL;
  }
  return rv;
}

int cbasebox::ingress_port_vlan_accept_all(uint32_t port) noexcept {
  int rv = 0;
  try {
    rofl::crofdpt &dpt = set_dpt(dptid, true);
    uint32_t of_port = port_id_to_of_port.at(port);
    fm_driver.enable_port_vid_allow_all(dpt, of_port);
  } catch (rofl::eRofBaseNotFound &e) {
    // TODO log error
    rv = -EINVAL;
  }
  return rv;
}

int cbasebox::ingress_port_vlan_drop_accept_all(uint32_t port) noexcept {
  int rv = 0;
  try {
    rofl::crofdpt &dpt = set_dpt(dptid, true);
    uint32_t of_port = port_id_to_of_port.at(port);
    fm_driver.disable_port_vid_allow_all(dpt, of_port);
  } catch (rofl::eRofBaseNotFound &e) {
    // TODO log error
    rv = -EINVAL;
  }
  return rv;
}

int cbasebox::ingress_port_vlan_add(uint32_t port, uint16_t vid,
                                    bool pvid) noexcept {
  int rv = 0;
  try {
    rofl::crofdpt &dpt = set_dpt(dptid, true);
    uint32_t of_port = port_id_to_of_port.at(port);
    if (pvid) {
      fm_driver.enable_port_pvid_ingress(dpt, of_port, vid);
    } else {
      fm_driver.enable_port_vid_ingress(dpt, of_port, vid);
    }
  } catch (rofl::eRofBaseNotFound &e) {
    // TODO log error
    rv = -EINVAL;
  }
  return rv;
}

int cbasebox::ingress_port_vlan_remove(uint32_t port, uint16_t vid,
                                       bool pvid) noexcept {
  int rv = 0;
  try {
    rofl::crofdpt &dpt = set_dpt(dptid, true);
    uint32_t of_port = port_id_to_of_port.at(port);
    if (pvid) {
      fm_driver.disable_port_pvid_ingress(dpt, of_port, vid);
    } else {
      fm_driver.disable_port_vid_ingress(dpt, of_port, vid);
    }
  } catch (rofl::eRofBaseNotFound &e) {
    // TODO log error
    rv = -EINVAL;
  }
  return rv;
}

int cbasebox::egress_port_vlan_accept_all(uint32_t port) noexcept {
  int rv = 0;
  try {
    rofl::crofdpt &dpt = set_dpt(dptid, true);
    uint32_t of_port = port_id_to_of_port.at(port);
    fm_driver.enable_group_l2_unfiltered_interface(dpt, of_port);
  } catch (rofl::eRofBaseNotFound &e) {
    // TODO log error
    rv = -EINVAL;
  }
  return rv;
}

int cbasebox::egress_port_vlan_drop_accept_all(uint32_t port) noexcept {
  int rv = 0;
  try {
    rofl::crofdpt &dpt = set_dpt(dptid, true);
    uint32_t of_port = port_id_to_of_port.at(port);
    fm_driver.disable_group_l2_unfiltered_interface(dpt, of_port);
  } catch (rofl::eRofBaseNotFound &e) {
    // TODO log error
    rv = -EINVAL;
  }
  return rv;
}

int cbasebox::egress_port_vlan_add(uint32_t port, uint16_t vid,
                                   bool untagged) noexcept {
  int rv = 0;
  try {
    rofl::crofdpt &dpt = set_dpt(dptid, true);
    uint32_t of_port = port_id_to_of_port.at(port);
    fm_driver.enable_group_l2_interface(dpt, of_port, vid, untagged);
  } catch (rofl::eRofBaseNotFound &e) {
    // TODO log error
    rv = -EINVAL;
  }
  return rv;
}

int cbasebox::egress_port_vlan_remove(uint32_t port, uint16_t vid,
                                      bool untagged) noexcept {
  int rv = 0;
  try {
    rofl::crofdpt &dpt = set_dpt(dptid, true);
    uint32_t of_port = port_id_to_of_port.at(port);
    fm_driver.disable_group_l2_interface(dpt, of_port, vid, untagged);
  } catch (rofl::eRofBaseNotFound &e) {
    // TODO log error
    rv = -EINVAL;
  }
  return rv;
}

int cbasebox::subscribe_to(enum swi_flags flags) noexcept {
  int rv = 0;
  try {
    rofl::crofdpt &dpt = set_dpt(dptid, true);
    if (flags & switch_interface::SWIF_ARP) {
      fm_driver.enable_policy_arp(dpt, 0, -1);
    }
  } catch (rofl::eRofBaseNotFound &e) {
    // TODO log error
    rv = -EINVAL;
  }
  return rv;
}

} // namespace basebox
