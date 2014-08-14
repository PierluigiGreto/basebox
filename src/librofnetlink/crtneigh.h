/*
 * crtneigh.h
 *
 *  Created on: 03.07.2013
 *      Author: andreas
 */

#ifndef CRTNEIGH_H_
#define CRTNEIGH_H_ 1

#include <ostream>

#ifdef __cplusplus
extern "C" {
#endif
#include <inttypes.h>
#include <netlink/route/neighbour.h>
#ifdef __cplusplus
}
#endif

#include <rofl/common/caddress.h>

#include "clogging.h"

namespace rofcore {

class crtneigh {
public:

	class eRtNeighBase		: public std::runtime_error {
	public:
		eRtNeighBase(const std::string& __arg) : std::runtime_error(__arg) {};
	};
	class eRtNeighNotFound	: public eRtNeighBase {
	public:
		eRtNeighNotFound(const std::string& __arg) : eRtNeighBase(__arg) {};
	};
	class eRtNeighExists		: public eRtNeighBase {
	public:
		eRtNeighExists(const std::string& __arg) : eRtNeighBase(__arg) {};
	};

public:

	/**
	 *
	 */
	crtneigh() :
		state(0),
		flags(0),
		ifindex(0),
		lladdr(rofl::cmacaddr("00:00:00:00:00:00")),
		family(0),
		type(0) {};

	/**
	 *
	 */
	virtual
	~crtneigh() {};

	/**
	 *
	 */
	crtneigh(
			const crtneigh& rtneigh) { *this = rtneigh; };

	/**
	 *
	 */
	crtneigh&
	operator= (
			const crtneigh& neigh) {
		if (this == &neigh)
			return *this;

		state		= neigh.state;
		flags		= neigh.flags;
		ifindex		= neigh.ifindex;
		lladdr		= neigh.lladdr;
		family 		= neigh.family;
		type		= neigh.type;

		return *this;
	};

	/**
	 *
	 */
	crtneigh(
			struct rtnl_neigh* neigh) :
				state(0),
				flags(0),
				ifindex(0),
				lladdr(rofl::cmacaddr("00:00:00:00:00:00")),
				family(0),
				type(0)
	{
		char s_buf[128];

		nl_object_get((struct nl_object*)neigh); // increment reference counter by one

		state	= rtnl_neigh_get_state(neigh);
		flags	= rtnl_neigh_get_flags(neigh);
		ifindex	= rtnl_neigh_get_ifindex(neigh);
		family	= rtnl_neigh_get_family(neigh);
		type	= rtnl_neigh_get_type(neigh);

		memset(s_buf, 0, sizeof(s_buf));
		nl_addr2str(rtnl_neigh_get_lladdr(neigh), s_buf, sizeof(s_buf));
		if (std::string(s_buf) != std::string("none"))
			lladdr 	= rofl::cmacaddr(nl_addr2str(rtnl_neigh_get_lladdr(neigh), s_buf, sizeof(s_buf)));
		else
			lladdr 	= rofl::cmacaddr("00:00:00:00:00:00");

		nl_object_put((struct nl_object*)neigh); // decrement reference counter by one
	};


	/**
	 *
	 */
	bool
	operator== (const crtneigh& rtneigh) {
		return ((state 		== rtneigh.state) 	&&
				(flags 		== rtneigh.flags) 	&&
				(ifindex 	== rtneigh.ifindex) &&
				(lladdr 	== rtneigh.lladdr) 	&&
				(family 	== rtneigh.family) 	&&
				(type 		== rtneigh.type));
	};

public:

	/**
	 *
	 */
	int
	get_state() const { return state; };

	/**
	 *
	 */
	std::string
	get_state_s() const {
		std::string str;

		switch (state) {
		case NUD_INCOMPLETE: 	str = std::string("INCOMPLETE"); 	break;
		case NUD_REACHABLE: 	str = std::string("REACHABLE"); 	break;
		case NUD_STALE:			str = std::string("STALE");			break;
		case NUD_DELAY:			str = std::string("DELAY");			break;
		case NUD_PROBE:			str = std::string("PROBE");			break;
		case NUD_FAILED:		str = std::string("FAILED");		break;
		default:				str = std::string("UNKNOWN");		break;
		}

		return str;
	};


	/**
	 *
	 */
	unsigned int
	get_flags() const { return flags; };

	/**
	 *
	 */
	int
	get_ifindex() const { return ifindex; };

	/**
	 *
	 */
	const rofl::cmacaddr&
	get_lladdr() const { return lladdr; };

	/**
	 *
	 */
	int
	get_family() const { return family; };

	/**
	 *
	 */
	int
	get_type() const { return type; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, crtneigh const& neigh) {
		os << rofcore::indent(0) << "<crtneigh: >" << std::endl;
		os << rofcore::indent(2) << "<state: " 	<< neigh.state 		<< " >" << std::endl;
		os << rofcore::indent(2) << "<flags: " 	<< neigh.flags 		<< " >" << std::endl;
		os << rofcore::indent(2) << "<ifindex: " 	<< neigh.ifindex 	<< " >" << std::endl;
		os << rofcore::indent(2) << "<lladdr: " 	<< neigh.lladdr 	<< " >" << std::endl;
		os << rofcore::indent(2) << "<family: " 	<< neigh.family 	<< " >" << std::endl;
		os << rofcore::indent(2) << "<type: " 		<< neigh.type 		<< " >" << std::endl;
		return os;
	};

private:

	int				state;
	unsigned int	flags;
	int				ifindex;
	rofl::cmacaddr	lladdr;
	int				family;
	int				type;
};




class crtneigh_in4 : public crtneigh {
public:

	/**
	 *
	 */
	crtneigh_in4() {};

	/**
	 *
	 */
	virtual
	~crtneigh_in4() {};

	/**
	 *
	 */
	crtneigh_in4(
			const crtneigh_in4& neigh) { *this = neigh; };

	/**
	 *
	 */
	crtneigh_in4&
	operator= (
			const crtneigh_in4& neigh) {
		if (this == &neigh)
			return *this;
		crtneigh::operator= (neigh);
		dst = neigh.dst;
		return *this;
	};

	/**
	 *
	 */
	crtneigh_in4(
			struct rtnl_neigh* neigh) :
				crtneigh(neigh) {

		nl_object_get((struct nl_object*)neigh); // increment reference counter by one

		char s_buf[128];
		memset(s_buf, 0, sizeof(s_buf));

		std::string s_dst;
		nl_addr2str(rtnl_neigh_get_dst(neigh), s_buf, sizeof(s_buf));
		if (std::string(s_buf) != std::string("none"))
			s_dst.assign(nl_addr2str(rtnl_neigh_get_dst(neigh), s_buf, sizeof(s_buf)));
		else
			s_dst.assign("0.0.0.0/0");

		dst = rofl::caddress_in4(s_dst.c_str());

		nl_object_put((struct nl_object*)neigh); // decrement reference counter by one
	};

	/**
	 *
	 */
	bool
	operator== (const crtneigh_in4& rtneigh) {
		return ((crtneigh::operator== (rtneigh)) && (dst == rtneigh.dst));
	};

public:

	/**
	 *
	 */
	const rofl::caddress_in4
	get_dst() const { return dst; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const crtneigh_in4& neigh) {
		os << rofcore::indent(0) << "<crtneigh_in4: >" << std::endl;
		os << rofcore::indent(2) << "<dst: >" << std::endl;
		rofcore::indent i(4); os << neigh.dst;
		return os;
	};


	/**
	 *
	 */
	class crtneigh_in4_find_by_dst {
		rofl::caddress_in4 dst;
	public:
		crtneigh_in4_find_by_dst(const rofl::caddress_in4& dst) :
			dst(dst) {};
		bool
		operator() (const std::pair<uint16_t,crtneigh_in4>& p) {
			return (p.second.dst == dst);
		};
	};


private:

	rofl::caddress_in4	dst;
};


/**
 *
 */
class crtneigh_in4_find : public std::unary_function<crtneigh_in4,bool> {
	crtneigh_in4 rtneigh;
public:
	crtneigh_in4_find(const crtneigh_in4& rtneigh) :
		rtneigh(rtneigh) {};
	bool operator() (const crtneigh_in4& rtn) {
		return (rtneigh == rtn);
	};
	bool operator() (const std::pair<unsigned int, crtneigh_in4>& p) {
		return (rtneigh == p.second);
	};
#if 0
	bool operator() (const std::pair<unsigned int, crtneigh_in4*>& p) {
		return (rtneigh == *(p.second));
	};
#endif
};




class crtneigh_in6 : public crtneigh {
public:

	/**
	 *
	 */
	crtneigh_in6() {};

	/**
	 *
	 */
	virtual
	~crtneigh_in6() {};

	/**
	 *
	 */
	crtneigh_in6(
			const crtneigh_in6& neigh) { *this = neigh; };

	/**
	 *
	 */
	crtneigh_in6&
	operator= (
			const crtneigh_in6& neigh) {
		if (this == &neigh)
			return *this;
		crtneigh::operator= (neigh);
		dst = neigh.dst;
		return *this;
	};

	/**
	 *
	 */
	crtneigh_in6(
			struct rtnl_neigh* neigh) :
				crtneigh(neigh) {

		nl_object_get((struct nl_object*)neigh); // increment reference counter by one

		char s_buf[128];
		memset(s_buf, 0, sizeof(s_buf));

		std::string s_dst;
		nl_addr2str(rtnl_neigh_get_dst(neigh), s_buf, sizeof(s_buf));
		if (std::string(s_buf) != std::string("none"))
			s_dst.assign(nl_addr2str(rtnl_neigh_get_dst(neigh), s_buf, sizeof(s_buf)));
		else
			s_dst.assign("::");

		dst = rofl::caddress_in6(s_dst.c_str());

		nl_object_put((struct nl_object*)neigh); // decrement reference counter by one
	};


	/**
	 *
	 */
	bool
	operator== (const crtneigh_in6& rtneigh) {
		return ((crtneigh::operator== (rtneigh)) && (dst == rtneigh.dst));
	};

public:

	/**
	 *
	 */
	const rofl::caddress_in6
	get_dst() const { return dst; };

public:

	friend std::ostream&
	operator<< (std::ostream& os, const crtneigh_in6& neigh) {
		os << rofcore::indent(0) << "<crtneigh_in6: >" << std::endl;
		os << rofcore::indent(2) << "<dst: >" << std::endl;
		rofcore::indent i(4); os << neigh.dst;
		return os;
	};


	/**
	 *
	 */
	class crtneigh_in6_find_by_dst {
		rofl::caddress_in6 dst;
	public:
		crtneigh_in6_find_by_dst(const rofl::caddress_in6& dst) :
			dst(dst) {};
		bool
		operator() (const std::pair<uint16_t,crtneigh_in6>& p) {
			return (p.second.dst == dst);
		};
	};


private:

	rofl::caddress_in6	dst;
};

/**
 *
 */
class crtneigh_in6_find : public std::unary_function<crtneigh_in6,bool> {
	crtneigh_in6 rtneigh;
public:
	crtneigh_in6_find(const crtneigh_in6& rtneigh) :
		rtneigh(rtneigh) {};
	bool operator() (const crtneigh_in6& rtn) {
		return (rtneigh == rtn);
	};
	bool operator() (const std::pair<unsigned int, crtneigh_in6>& p) {
		return (rtneigh == p.second);
	};
#if 0
	bool operator() (const std::pair<unsigned int, crtneigh_in6*>& p) {
		return (rtneigh == *(p.second));
	};
#endif
};


}; // end of namespace

#endif /* CRTNEIGH_H_ */

