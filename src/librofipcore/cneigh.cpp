/*
 * cdptneigh.cc
 *
 *  Created on: 03.07.2013
 *      Author: andreas
 */

#include "cneigh.hpp"
#include "cipcore.hpp"

using namespace ipcore;


void
cneigh_in4::handle_dpt_open(rofl::crofdpt& dpt)
{
	try {
		rofcore::cnetlink& netlink = rofcore::cnetlink::get_instance();

		// the neighbour ...
		const rofcore::crtneigh_in4& rtn =
				netlink.get_links().get_link(get_ifindex()).get_neighs_in4().get_neigh(get_nbindex());

		// ... reachable via this link (crtlink) ...
		const rofcore::crtlink& rtl =
				netlink.get_links().get_link(rtn.get_ifindex());

		// ... and the link's dpt representation (clink) needed for OFP related data
		const ipcore::clink& dpl =
				cipcore::get_instance().get_link_by_ifindex(rtn.get_ifindex());



		// local outgoing interface mac address
		const rofl::cmacaddr& eth_src 	= rtl.get_hwaddr();

		// neighbour mac address
		const rofl::cmacaddr& eth_dst 	= rtn.get_lladdr();

		// local outgoing interface => OFP portno
		uint32_t out_portno 			= dpl.get_ofp_port_no();



		rofl::openflow::cofflowmod fe(dpt.get_version());

		switch (state) {
		case STATE_DETACHED: {
			fe.set_command(rofl::openflow::OFPFC_ADD);
		} break;
		case STATE_ATTACHED: {
			fe.set_command(rofl::openflow::OFPFC_MODIFY_STRICT);
		} break;
		}

		fe.set_buffer_id(rofl::openflow::base::get_ofp_no_buffer(dpt.get_version()));
		fe.set_idle_timeout(0);
		fe.set_hard_timeout(0);
		fe.set_priority(0xfffe);
		fe.set_table_id(get_table_id());
		fe.set_flags(rofl::openflow13::OFPFF_SEND_FLOW_REM);

		fe.set_match().set_eth_type(rofl::fipv4frame::IPV4_ETHER);
		fe.set_match().set_ipv4_dst(rtn.get_dst());

		rofl::cindex index(0);

		fe.set_instructions().add_inst_apply_actions().set_actions().
				add_action_set_field(index++).set_oxm(rofl::openflow::coxmatch_ofb_eth_src(eth_src));
		fe.set_instructions().set_inst_apply_actions().set_actions().
				add_action_set_field(index++).set_oxm(rofl::openflow::coxmatch_ofb_eth_dst(eth_dst));
		fe.set_instructions().set_inst_apply_actions().set_actions().
				add_action_output(index++).set_port_no(out_portno);

		dpt.send_flow_mod_message(rofl::cauxid(0), fe);

		state = STATE_ATTACHED;

	} catch (rofcore::eNetLinkNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in4][handle_dpt_open] unable to find link" << std::endl << *this;

	} catch (rofcore::crtneigh::eRtNeighNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in4][handle_dpt_open] unable to find neighbour" << std::endl << *this;

	} catch (rofcore::crtlink::eRtLinkNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in4][handle_dpt_open] unable to find address" << std::endl << *this;

	} catch (rofl::eRofDptNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in4][handle_dpt_open] unable to find data path" << std::endl << *this;

	} catch (...) {
		rofcore::logging::error << "[cipcore][neigh_in4][handle_dpt_open] unexpected error" << std::endl << *this;

	}
}



void
cneigh_in4::handle_dpt_close(rofl::crofdpt& dpt)
{
	try {
		rofcore::cnetlink& netlink = rofcore::cnetlink::get_instance();

		// the neighbour ...
		const rofcore::crtneigh_in4& rtn =
				netlink.get_links().get_link(get_ifindex()).get_neighs_in4().get_neigh(get_nbindex());


		rofl::openflow::cofflowmod fe(dpt.get_version());

		fe.set_command(rofl::openflow::OFPFC_DELETE_STRICT);
		fe.set_buffer_id(rofl::openflow::base::get_ofp_no_buffer(dpt.get_version()));
		fe.set_idle_timeout(0);
		fe.set_hard_timeout(0);
		fe.set_priority(0xfffe);
		fe.set_table_id(get_table_id());

		fe.set_match().set_eth_type(rofl::fipv4frame::IPV4_ETHER);
		fe.set_match().set_ipv4_dst(rtn.get_dst());

		dpt.send_flow_mod_message(rofl::cauxid(0), fe);

		state = STATE_DETACHED;

	} catch (rofcore::eNetLinkNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in4][handle_dpt_close] unable to find link" << std::endl << *this;

	} catch (rofcore::crtneigh::eRtNeighNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in4][handle_dpt_close] unable to find neighbour" << std::endl << *this;

	} catch (rofcore::crtlink::eRtLinkNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in4][handle_dpt_close] unable to find address" << std::endl << *this;

	} catch (rofl::eRofDptNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in4][handle_dpt_close] unable to find data path" << std::endl << *this;

	} catch (...) {
		rofcore::logging::error << "[cipcore][neigh_in4][handle_dpt_close] unexpected error" << std::endl << *this;

	}
}



void
cneigh_in6::handle_dpt_open(rofl::crofdpt& dpt)
{
	try {
		rofcore::cnetlink& netlink = rofcore::cnetlink::get_instance();

		// the neighbour ...
		const rofcore::crtneigh_in6& rtn =
				netlink.get_links().get_link(get_ifindex()).get_neighs_in6().get_neigh(get_nbindex());

		// ... reachable via this link (crtlink) ...
		const rofcore::crtlink& rtl =
				netlink.get_links().get_link(rtn.get_ifindex());

		// ... and the link's dpt representation (clink) needed for OFP related data
		const ipcore::clink& dpl =
				cipcore::get_instance().get_link_by_ifindex(rtn.get_ifindex());



		// local outgoing interface mac address
		const rofl::cmacaddr& eth_src 	= rtl.get_hwaddr();

		// neighbour mac address
		const rofl::cmacaddr& eth_dst 	= rtn.get_lladdr();

		// local outgoing interface => OFP portno
		uint32_t out_portno 			= dpl.get_ofp_port_no();



		rofl::openflow::cofflowmod fe(dpt.get_version());

		switch (state) {
		case STATE_DETACHED: {
			fe.set_command(rofl::openflow::OFPFC_ADD);
		} break;
		case STATE_ATTACHED: {
			fe.set_command(rofl::openflow::OFPFC_MODIFY_STRICT);
		} break;
		}

		fe.set_buffer_id(rofl::openflow::base::get_ofp_no_buffer(dpt.get_version()));
		fe.set_idle_timeout(0);
		fe.set_hard_timeout(0);
		fe.set_priority(0xfffe);
		fe.set_table_id(get_table_id());
		fe.set_flags(rofl::openflow13::OFPFF_SEND_FLOW_REM);

		fe.set_match().set_eth_type(rofl::fipv6frame::IPV6_ETHER);
		fe.set_match().set_ipv6_dst(rtn.get_dst());

		rofl::cindex index(0);

		fe.set_instructions().add_inst_apply_actions().set_actions().
				add_action_set_field(index++).set_oxm(rofl::openflow::coxmatch_ofb_eth_src(eth_src));
		fe.set_instructions().set_inst_apply_actions().set_actions().
				add_action_set_field(index++).set_oxm(rofl::openflow::coxmatch_ofb_eth_dst(eth_dst));
		fe.set_instructions().set_inst_apply_actions().set_actions().
				add_action_output(index++).set_port_no(out_portno);

		dpt.send_flow_mod_message(rofl::cauxid(0), fe);

		state = STATE_ATTACHED;

	} catch (rofcore::eNetLinkNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in6][handle_dpt_open] unable to find link" << std::endl << *this;

	} catch (rofcore::crtneigh::eRtNeighNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in6][handle_dpt_open] unable to find neighbour" << std::endl << *this;

	} catch (rofcore::crtlink::eRtLinkNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in6][handle_dpt_open] unable to find address" << std::endl << *this;

	} catch (rofl::eRofDptNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in6][handle_dpt_open] unable to find data path" << std::endl << *this;

	} catch (...) {
		rofcore::logging::error << "[cipcore][neigh_in6][handle_dpt_open] unexpected error" << std::endl << *this;

	}
}



void
cneigh_in6::handle_dpt_close(rofl::crofdpt& dpt)
{
	try {
		rofcore::cnetlink& netlink = rofcore::cnetlink::get_instance();

		// the neighbour ...
		const rofcore::crtneigh_in6& rtn =
				netlink.get_links().get_link(get_ifindex()).get_neighs_in6().get_neigh(get_nbindex());


		rofl::openflow::cofflowmod fe(dpt.get_version());

		fe.set_command(rofl::openflow::OFPFC_DELETE_STRICT);
		fe.set_buffer_id(rofl::openflow::base::get_ofp_no_buffer(dpt.get_version()));
		fe.set_idle_timeout(0);
		fe.set_hard_timeout(0);
		fe.set_priority(0xfffe);
		fe.set_table_id(get_table_id());

		fe.set_match().set_eth_type(rofl::fipv6frame::IPV6_ETHER);
		fe.set_match().set_ipv6_dst(rtn.get_dst());

		dpt.send_flow_mod_message(rofl::cauxid(0), fe);

		state = STATE_DETACHED;

	} catch (rofcore::eNetLinkNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in6][handle_dpt_close] unable to find link" << std::endl << *this;

	} catch (rofcore::crtneigh::eRtNeighNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in6][handle_dpt_close] unable to find neighbour" << std::endl << *this;

	} catch (rofcore::crtlink::eRtLinkNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in6][handle_dpt_close] unable to find address" << std::endl << *this;

	} catch (rofl::eRofDptNotFound& e) {
		rofcore::logging::error << "[cipcore][neigh_in6][handle_dpt_close] unable to find data path" << std::endl << *this;

	} catch (...) {
		rofcore::logging::error << "[cipcore][neigh_in6][handle_dpt_close] unexpected error" << std::endl << *this;

	}
}
