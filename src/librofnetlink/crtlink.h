/*
 * crtlink.h
 *
 *  Created on: 27.06.2013
 *      Author: andreas
 */

#ifndef CRTLINK_H_
#define CRTLINK_H_ 1

#include <exception>
#include <algorithm>
#include <ostream>
#include <string>
#include <map>
#include <set>

#include <rofl/common/caddress.h>
#include <rofl/common/logging.h>
#include <rofl/common/cmemory.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <netlink/object.h>
#include <netlink/route/link.h>
#include <inttypes.h>
#ifdef __cplusplus
}
#endif

#include "crtaddrs.h"
#include "crtneighs.h"

namespace rofcore {

class crtlink {
public:

	class eRtLinkBase		: public std::runtime_error {
	public:
		eRtLinkBase(const std::string& __arg) : std::runtime_error(__arg) {};
	};
	class eRtLinkNotFound	: public eRtLinkBase {
	public:
		eRtLinkNotFound(const std::string& __arg) : eRtLinkBase(__arg) {};
	};
	class eRtLinkExists		: public eRtLinkBase {
	public:
		eRtLinkExists(const std::string& __arg) : eRtLinkBase(__arg) {};
	};

public:

	/**
	 *
	 */
	crtlink() :
		flags(0),
		af(0),
		arptype(0),
		ifindex(0),
		mtu(0) {};

	/**
	 *
	 */
	crtlink(struct rtnl_link *link) :
		flags(0),
		af(0),
		arptype(0),
		ifindex(0),
		mtu(0)
	{
		char s_buf[128];
		memset(s_buf, 0, sizeof(s_buf));

		nl_object_get((struct nl_object*)link); // increment reference counter by one

		devname.assign(rtnl_link_get_name(link));
		maddr 	= rofl::cmacaddr(nl_addr2str(rtnl_link_get_addr(link), 		s_buf, sizeof(s_buf)));
		bcast 	= rofl::cmacaddr(nl_addr2str(rtnl_link_get_broadcast(link), s_buf, sizeof(s_buf)));
		flags 	= rtnl_link_get_flags(link);
		af 		= rtnl_link_get_family(link);
		arptype = rtnl_link_get_arptype(link);
		ifindex	= rtnl_link_get_ifindex(link);
		mtu 	= rtnl_link_get_mtu(link);

		nl_object_put((struct nl_object*)link); // decrement reference counter by one
	};

	/**
	 *
	 */
	virtual
	~crtlink() {};

	/**
	 *
	 */
	crtlink(const crtlink& rtlink) { *this = rtlink; };

	/**
	 *
	 */
	crtlink&
	operator= (const crtlink& rtlink) {
		if (this == &rtlink)
			return *this;

		devname = rtlink.devname;
		maddr	= rtlink.maddr;
		bcast	= rtlink.bcast;
		flags	= rtlink.flags;
		af		= rtlink.af;
		arptype	= rtlink.arptype;
		ifindex	= rtlink.ifindex;
		mtu		= rtlink.mtu;

		addrs_in4	= rtlink.addrs_in4;
		addrs_in6	= rtlink.addrs_in6;
		neighs_in4	= rtlink.neighs_in4;
		neighs_in6	= rtlink.neighs_in6;

		return *this;
	};

	/**
	 *
	 */
	bool
	operator== (const crtlink& rtlink) {
		return ((devname == rtlink.devname) && (ifindex == rtlink.ifindex) && (maddr == rtlink.maddr));
	}

public:

	/**
	 *
	 */
	const crtaddrs_in4&
	get_addrs_in4() const { return addrs_in4; };

	/**
	 *
	 */
	crtaddrs_in4&
	set_addrs_in4() { return addrs_in4; };

	/**
	 *
	 */
	const crtaddrs_in6&
	get_addrs_in6() const { return addrs_in6; };

	/**
	 *
	 */
	crtaddrs_in6&
	set_addrs_in6() { return addrs_in6; };


	/**
	 *
	 */
	const crtneighs_in4&
	get_neighs_in4() const { return neighs_in4; };

	/**
	 *
	 */
	crtneighs_in4&
	set_neighs_in4() { return neighs_in4; };

	/**
	 *
	 */
	const crtneighs_in6&
	get_neighs_in6() const { return neighs_in6; };

	/**
	 *
	 */
	crtneighs_in6&
	set_neighs_in6() { return neighs_in6; };



public:


	/**
	 *
	 */
	const std::string&
	get_devname() const { return devname; };


	/**
	 *
	 */
	const rofl::cmacaddr&
	get_hwaddr() const { return maddr; };


	/**
	 *
	 */
	const rofl::cmacaddr&
	get_broadcast() const { return bcast; };


	/**
	 *
	 */
	unsigned int
	get_flags() const { return flags; };


	/**
	 *
	 */
	int
	get_family() const { return af; };


	/**
	 *
	 */
	unsigned int
	get_arptype() const { return arptype; };


	/**
	 *
	 */
	int
	get_ifindex() const { return ifindex; };


	/**
	 *
	 */
	unsigned int
	get_mtu() const { return mtu; };


public:


	/**
	 *
	 */
	friend std::ostream&
	operator<< (std::ostream& os, crtlink const& rtlink) {
		os << rofl::indent(0) << "<crtlink: >" << std::endl;
		os << rofl::indent(2) << "<devname: " << rtlink.devname 	<< " >" << std::endl;
		os << rofl::indent(2) << "<maddr: >" << std::endl;
		{ rofl::indent i(4); os << rtlink.maddr; };
		os << rofl::indent(2) << "<bcast: >" << std::endl;
		{ rofl::indent i(4); os << rtlink.bcast; };
		os << rofl::indent(2) << "<flags: " << (std::hex) << rtlink.flags << (std::dec) << " >" << std::endl;
		os << rofl::indent(2) << "<af: " << rtlink.af 				<< " >" << std::endl;
		os << rofl::indent(2) << "<arptype: " << rtlink.arptype 	<< " >" << std::endl;
		os << rofl::indent(2) << "<ifindex: " << rtlink.ifindex 	<< " >" << std::endl;
		os << rofl::indent(2) << "<mtu: " << rtlink.mtu 			<< " >" << std::endl;

		{ rofl::indent i(2); os << rtlink.addrs_in4; };
		{ rofl::indent i(2); os << rtlink.addrs_in6; };
		{ rofl::indent i(2); os << rtlink.neighs_in4; };
		{ rofl::indent i(2); os << rtlink.neighs_in6; };

		return os;
	};


	/**
	 *
	 */
	class crtlink_find_by_ifindex : public std::unary_function<crtlink,bool> {
		unsigned int ifindex;
	public:
		crtlink_find_by_ifindex(unsigned int ifindex) :
			ifindex(ifindex) {};
		bool operator() (crtlink const& rtl) {
			return (ifindex == rtl.ifindex);
		};
		bool operator() (std::pair<unsigned int, crtlink> const& p) {
			return (ifindex == p.second.ifindex);
		};
		bool operator() (std::pair<unsigned int, crtlink*> const& p) {
			return (ifindex == p.second->ifindex);
		};
	};


	/**
	 *
	 */
	class crtlink_find_by_devname : public std::unary_function<crtlink,bool> {
		std::string devname;
	public:
		crtlink_find_by_devname(std::string const& devname) :
			devname(devname) {};
		bool operator() (crtlink const& rtl) {
			return (devname == rtl.devname);
		};
		bool operator() (std::pair<unsigned int, crtlink> const& p) {
			return (devname == p.second.devname);
		};
#if 0
		bool operator() (std::pair<unsigned int, crtlink*> const& p) {
			return (devname == p.second->devname);
		};
#endif
	};

private:

	std::string				devname;	// device name (e.g. eth0)
	rofl::cmacaddr			maddr;		// mac address
	rofl::cmacaddr			bcast; 		// broadcast address
	unsigned int			flags;		// link flags
	int						af;			// address family (AF_INET, AF_UNSPEC, ...)
	unsigned int			arptype;	// ARP type (e.g. ARPHDR_ETHER)
	int						ifindex;	// interface index
	unsigned int			mtu;		// maximum transfer unit

	crtaddrs_in4			addrs_in4;
	crtaddrs_in6			addrs_in6;
	crtneighs_in4			neighs_in4;
	crtneighs_in6			neighs_in6;

};

}; // end of namespace dptmap

#endif /* CRTLINK_H_ */
