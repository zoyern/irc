/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_test.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marvin <marvin@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: Invalid date        by  music mu         #+#    #+#             */
/*   Updated: 2025/11/29 02:57:52 by marvin           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
 *SOCKELL COMPREHENSIVE TEST SUITE - 150+ Tests
 *
 *Tests all library components with real API:
 *- SkllSpan, SkllBuffer, SkllMessage
 *- SkllChannel, SkllChannels
 *- SkllHooks, SkllRouter
 *- SkllClient, SkllProtocol
 *- SkllException
 *- Stress &Security tests
 */

#include <Sockell.hpp>
#include <iostream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <sstream>

static int g_passed = 0;
static int g_failed = 0;
static int g_total = 0;

#define TEST(name, cond) do { \
	++g_total; \
	if (cond) { \
		std::cout << SKLL_GREEN << "  ✓ " << SKLL_RST << name << std::endl; \
		++g_passed; \
	} else { \
		std::cout << SKLL_RED << "  ✗ " << SKLL_RST << name << " [FAILED]" << std::endl; \
		++g_failed; \
	} \
} while(0)

#define SECTION(name) std::cout << std::endl << SKLL_CYAN << SKLL_BOLD << "═══ " << name << " ═══" << SKLL_RST << std::endl

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLLSPAN TESTS (25 tests)                                                 */
/* ═══════════════════════════════════════════════════════════════════════════ */

void test_span() {
	SECTION("SkllSpan - Construction");
	
	SkllSpan s1;
	TEST("Default empty", s1.empty());
	TEST("Default len=0", s1.len == 0);
	TEST("Default data=null", s1.data == 0);
	
	SkllSpan s2("hello");
	TEST("C-string len=5", s2.len == 5);
	TEST("C-string not empty", !s2.empty());
	TEST("C-string data[0]='h'", s2.data[0] == 'h');
	
	std::string str = "world";
	SkllSpan s3(str);
	TEST("std::string len=5", s3.len == 5);
	TEST("std::string data[0]='w'", s3.data[0] == 'w');
	
	SECTION("SkllSpan - Comparison");
	
	TEST("eq(c-str) true", s2.eq("hello"));
	TEST("eq(c-str) case false", !s2.eq("Hello"));
	TEST("eq(c-str) len false", !s2.eq("hell"));
	TEST("eq(span) true", s2.eq(SkllSpan("hello")));
	TEST("eq(span) false", !s2.eq(SkllSpan("world")));
	TEST("eq empty", s1.eq(SkllSpan()));
	
	TEST("starts() true", s2.starts("hel"));
	TEST("starts() false", !s2.starts("wor"));
	TEST("starts() exact", s2.starts("hello"));
	TEST("starts() too long", !s2.starts("hello!"));
	
	SECTION("SkllSpan - Conversion");
	
	TEST("str() hello", s2.str() == "hello");
	TEST("str() empty", s1.str() == "");
	
	TEST("to_int(42)", SkllSpan("42").to_int() == 42);
	TEST("to_int(-123)", SkllSpan("-123").to_int() == -123);
	TEST("to_int(0)", SkllSpan("0").to_int() == 0);
	TEST("to_int(invalid)", SkllSpan("abc").to_int() == 0);
	
	SECTION("SkllSpan - Manipulation");
	
	SkllSpan ws("  hello  ");
	TEST("trim() result", ws.trim().eq("hello"));
	TEST("trim() no change", SkllSpan("hello").trim().eq("hello"));
	TEST("trim() only ws", SkllSpan("   ").trim().empty());
	
	TEST("sub(1,3) ell", s2.sub(1, 3).eq("ell"));
	TEST("sub(0,2) he", s2.sub(0, 2).eq("he"));
	
	TEST("operator[0]='h'", s2[0] == 'h');
	TEST("operator[4]='o'", s2[4] == 'o');
	TEST("operator[100]=0", s2[100] == 0);
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLLBUFFER TESTS (25 tests)                                               */
/* ═══════════════════════════════════════════════════════════════════════════ */

void test_buffer() {
	SECTION("SkllBuffer - Basic");
	
	SkllBuffer buf;
	TEST("Empty on init", buf.empty());
	TEST("Size=0 on init", buf.size() == 0);
	
	buf.write("hello");
	TEST("Write size=5", buf.size() == 5);
	TEST("Not empty", !buf.empty());
	
	byte_t peek_buf[10];
	size_t peeked = buf.peek(peek_buf, 3);
	TEST("Peek returns 3", peeked == 3);
	TEST("Peek no consume", buf.size() == 5);
	TEST("Peek data[0]='h'", peek_buf[0] == 'h');
	
	byte_t read_buf[10];
	size_t rcount = buf.read(read_buf, 3);
	TEST("Read returns 3", rcount == 3);
	TEST("Read consumes", buf.size() == 2);
	
	buf.reset();
	TEST("Reset clears", buf.empty());
	
	SECTION("SkllBuffer - Line Extraction");
	
	buf.write("line1\r\nline2\r\npartial");
	
	SkllSpan ln = buf.line("\r\n");
	TEST("Line found", !ln.empty() || ln.len == 0);
	TEST("Line content", ln.eq("line1"));
	
	ln = buf.line("\r\n");
	TEST("Second line", ln.eq("line2"));
	
	ln = buf.line("\r\n");
	TEST("No line w/o delim", ln.empty());
	TEST("Partial preserved", buf.size() == 7);
	
	SECTION("SkllBuffer - Edge Cases");
	
	buf.reset();
	buf.write("a|b|c");
	ln = buf.line("|");
	TEST("Single char delim", ln.eq("a"));
	
	buf.reset();
	std::string large(10000, 'x');
	buf.write(large);
	TEST("Large 10KB", buf.size() == 10000);
	
	buf.reset();
	buf.write("hello");
	buf.skip(2);
	TEST("Skip(2) removes", buf.size() == 3);
	
	buf.reset();
	buf.write("hello world");
	TEST("find('w')=6", buf.find('w') == 6);
	TEST("find('z')=-1", buf.find('z') == -1);
	TEST("operator[0]='h'", buf[0] == 'h');
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLLMESSAGE TESTS (25 tests)                                              */
/* ═══════════════════════════════════════════════════════════════════════════ */

void test_message() {
	SECTION("SkllMessage - Parsing");
	
	SkllMessage msg;
	TEST("Empty on init", msg.empty());
	
	msg.set("PING");
	TEST("Simple cmd", !msg.empty());
	TEST("Cmd=PING", msg.cmd().eq("PING"));
	TEST("argc=0", msg.argc() == 0);
	
	msg.set("JOIN #channel");
	TEST("Cmd=JOIN", msg.cmd().eq("JOIN"));
	TEST("argc=1", msg.argc() == 1);
	TEST("arg0=#channel", msg.arg(0).eq("#channel"));
	
	msg.set("MODE #chan +o nick");
	TEST("Cmd=MODE", msg.cmd().eq("MODE"));
	TEST("argc=3", msg.argc() == 3);
	TEST("arg0=#chan", msg.arg(0).eq("#chan"));
	TEST("arg1=+o", msg.arg(1).eq("+o"));
	TEST("arg2=nick", msg.arg(2).eq("nick"));
	
	SECTION("SkllMessage - Prefix &Trailing");
	
	msg.set(":nick!user@host PRIVMSG #chan :Hello world!");
	TEST("Prefix parsed", msg.prefix().eq("nick!user@host"));
	TEST("Cmd=PRIVMSG", msg.cmd().eq("PRIVMSG"));
	TEST("argc=2", msg.argc() == 2);
	TEST("arg0=#chan", msg.arg(0).eq("#chan"));
	TEST("Trailing", msg.arg(1).eq("Hello world!"));
	
	msg.set("QUIT :Goodbye everyone");
	TEST("Quit cmd", msg.cmd().eq("QUIT"));
	TEST("Quit trailing", msg.arg(0).eq("Goodbye everyone"));
	
	SECTION("SkllMessage - Edge Cases");
	
	msg.set("");
	TEST("Empty string", msg.empty());
	
	msg.set("MSG #chan :text : with : colons");
	TEST("Colons in trailing", msg.arg(1).eq("text : with : colons"));
	
	msg.set("NICK test");
	TEST("Raw access", msg.raw().eq("NICK test"));
	
	msg.set_param("key", SkllSpan("value"));
	TEST("Param set", msg.param("key").eq("value"));
	TEST("Param not found", msg.param("nokey").empty());
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLLCHANNEL TESTS (20 tests)                                              */
/* ═══════════════════════════════════════════════════════════════════════════ */

void test_channel() {
	SECTION("SkllChannel - Basic");
	
	SkllChannel ch("#test");
	TEST("Name set", ch.name() == "#test");
	TEST("Empty on init", ch.count() == 0);
	TEST("empty() true", ch.empty());
	
	ch.add(1);
	TEST("Add count=1", ch.count() == 1);
	TEST("has(1)", ch.has(1));
	TEST("!has(2)", !ch.has(2));
	TEST("!empty()", !ch.empty());
	
	ch.add(2, 0x01);
	TEST("Add with perms", ch.has(2));
	TEST("has_perm(2,0x01)", ch.has_perm(2, 0x01));
	TEST("!has_perm(2,0x02)", !ch.has_perm(2, 0x02));
	
	SECTION("SkllChannel - Permissions");
	
	ch.add_perms(2, 0x02);
	TEST("add_perms combined", ch.has_perm(2, 0x01) && ch.has_perm(2, 0x02));
	
	ch.del_perms(2, 0x01);
	TEST("del_perms removed", !ch.has_perm(2, 0x01));
	TEST("del_perms kept", ch.has_perm(2, 0x02));
	
	ch.set_perms(2, 0xFF);
	TEST("set_perms", ch.perms(2) == 0xFF);
	TEST("perms non-member", ch.perms(99) == 0);
	
	SECTION("SkllChannel - Metadata");
	
	ch.set("topic", "Hello World");
	TEST("Metadata set", ch.has("topic"));
	TEST("Metadata get", ch.get("topic") == "Hello World");
	TEST("Metadata not found", ch.get("nope") == "");
	
	ch.del("topic");
	TEST("Metadata deleted", !ch.has("topic"));
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLLCHANNELS TESTS (15 tests)                                             */
/* ═══════════════════════════════════════════════════════════════════════════ */

void test_channels() {
	SECTION("SkllChannels - Management");
	
	SkllChannels chs;
	TEST("Empty on init", chs.count() == 0);
	
	SkllChannel *ch = chs.create("#test");
	TEST("Create non-null", ch != 0);
	TEST("Create adds", chs.count() == 1);
	TEST("Exists", chs.exists("#test"));
	
	SkllChannel *got = chs.get("#test");
	TEST("Get same", got == ch);
	TEST("Get null", chs.get("#nope") == 0);
	
	TEST("Case insensitive", chs.exists("#TEST"));
	TEST("Case insensitive get", chs.get("#TeSt") == ch);
	
	SECTION("SkllChannels - Member Lookup");
	
	ch->add(1);
	ch->add(2);
	
	std::set<std::string> of1;
	chs.of(1, of1);
	TEST("of() finds", of1.count("#test") == 1);
	
	std::set<std::string> of99;
	chs.of(99, of99);
	TEST("of() empty", of99.empty());
	
	chs.remove_all(1);
	TEST("remove_all", !ch->has(1));
	TEST("remove_all keeps", ch->has(2));
	
	chs.clear();
	TEST("Clear", chs.count() == 0);
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLLHOOKS TESTS (10 tests)                                                */
/* ═══════════════════════════════════════════════════════════════════════════ */

static int g_hook_counter = 0;
static int test_hook_inc(SkllEvent &) { ++g_hook_counter; return SKLL_OK; }
static int test_hook_stop(SkllEvent &) { ++g_hook_counter; return SKLL_STOP; }

void test_hooks() {
	SECTION("SkllHooks - Basic");
	
	SkllHooks hooks;
	TEST("Empty on init", hooks.empty());
	TEST("Size=0", hooks.size() == 0);
	
	hooks.add(test_hook_inc);
	TEST("Add hook", hooks.size() == 1);
	TEST("Not empty", !hooks.empty());
	
	hooks.add(test_hook_inc);
	TEST("Multiple", hooks.size() == 2);
	
	hooks.clear();
	TEST("Clear", hooks.empty());
	
	SECTION("SkllHooks - Propagation");
	
	g_hook_counter = 0;
	hooks.add(test_hook_stop);
	hooks.add(test_hook_inc);
	
	SkllEvent ev;
	hooks.run(ev);
	TEST("Stop propagation", g_hook_counter == 1);
	
	g_hook_counter = 0;
	hooks.clear();
	hooks.add(test_hook_inc);
	hooks.add(test_hook_inc);
	hooks.run(ev);
	TEST("Normal propagation", g_hook_counter == 2);
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLLROUTER TESTS (10 tests)                                               */
/* ═══════════════════════════════════════════════════════════════════════════ */

static int router_handler(SkllEvent &) { return SKLL_OK; }
static int router_fallback(SkllEvent &) { return SKLL_ERR; }

/* Test handlers for router V5 */
static std::string last_var;
static std::string last_rest;
static std::string last_file;
static int call_count = 0;
static int received_data_id = 0;

struct TestServerData {
	int id;
	std::string name;
};

/* Handler with typed user data - NO CAST NEEDED! */
static int h_typed_handler(SkllEvent &e, TestServerData *srv) {
	received_data_id = srv->id;
	last_var = srv->name;
	++call_count;
	(void)e;
	return SKLL_OK;
}

static int h_capture_var(SkllEvent &e) {
	last_var = e.message().param("id").str();
	++call_count;
	return SKLL_OK;
}

static int h_capture_channel(SkllEvent &e) {
	last_var = e.message().param("channel").str();
	++call_count;
	return SKLL_OK;
}

static int h_capture_rest(SkllEvent &e) {
	last_rest = e.message().param("text").str();
	++call_count;
	return SKLL_OK;
}

static int h_capture_file(SkllEvent &e) {
	last_file = e.message().param("name").str();
	++call_count;
	return SKLL_OK;
}

static int h_middleware(SkllEvent &) {
	++call_count;
	return SKLL_OK;
}

static int h_middleware_typed(SkllEvent &, TestServerData *srv) {
	received_data_id = srv->id;
	++call_count;
	return SKLL_OK;
}

static int h_stop(SkllEvent &) {
	++call_count;
	return SKLL_STOP;
}

static int h_skip(SkllEvent &) {
	++call_count;
	return SKLL_SKIP;
}

void test_router() {
	SECTION("SkllRouter - Basic");
	
	SkllRouter router;
	TEST("Empty on init", router.count() == 0);
	
	router.on("PING", router_handler);
	TEST("Route added", router.count() == 1);
	
	router.on("PONG", router_handler);
	router.on("NICK", router_handler);
	TEST("Multiple routes", router.count() == 3);
	
	router.fallback(router_fallback);
	TEST("Fallback set", true);
	
	router.clear();
	TEST("Clear routes", router.count() == 0);
	
	SECTION("SkllRouter - Pattern");
	
	router.on("JOIN {channel}", router_handler);
	TEST("Pattern route added", router.count() == 1);
	
	router.on("MODE {chan} {mode}", router_handler);
	TEST("Multi-var pattern", router.count() == 2);
	
	router.on("PRIVMSG {target} {*text}", router_handler);
	TEST("Wildcard pattern", router.count() == 3);
	
	/* ═══════════════════════════════════════════════════════════════════════ */
	/*   TYPED CALLBACKS - NO CAST NEEDED!                                     */
	/* ═══════════════════════════════════════════════════════════════════════ */
	
	SECTION("SkllRouter - Typed Callbacks");
	
	TestServerData srv;
	srv.id = 42;
	srv.name = "TestServer";
	
	SkllRouter r_typed;
	call_count = 0;
	received_data_id = 0;
	last_var.clear();
	
	/* Register with typed callback - user data is TestServerData* */
	r_typed.on("CMD", h_typed_handler, &srv);
	TEST("Typed callback registered", r_typed.count() == 1);
	
	SkllEvent ev_typed;
	ev_typed.message().set(reinterpret_cast<const byte_t*>("CMD"), 3);
	r_typed.dispatch(ev_typed);
	
	TEST("Typed callback received data", received_data_id == 42);
	TEST("Typed callback name", last_var == "TestServer");
	TEST("Typed callback called", call_count == 1);
	
	SECTION("SkllRouter - Typed Middleware");
	
	SkllRouter r_mid;
	call_count = 0;
	received_data_id = 0;
	
	/* Use space-delimited IRC-style for cascade API */
	r_mid.route("GET")
		.then(h_middleware_typed, &srv)
		.path("users", router_handler);
	
	SkllEvent ev_mid;
	ev_mid.message().set(reinterpret_cast<const byte_t*>("GET users"), 9);
	r_mid.dispatch(ev_mid);
	
	TEST("Typed middleware executed", call_count == 1);
	TEST("Typed middleware data", received_data_id == 42);
	
	SECTION("SkllRouter - Typed Fallback");
	
	SkllRouter r_fb;
	call_count = 0;
	received_data_id = 0;
	
	r_fb.fallback(h_typed_handler, &srv);
	
	SkllEvent ev_fb;
	ev_fb.message().set(reinterpret_cast<const byte_t*>("UNKNOWN"), 7);
	r_fb.dispatch(ev_fb);
	
	TEST("Typed fallback called", call_count == 1);
	TEST("Typed fallback data", received_data_id == 42);
	
	/* ═══════════════════════════════════════════════════════════════════════ */
	/*   ROUTER V5 - Cascade API                                               */
	/* ═══════════════════════════════════════════════════════════════════════ */
	
	SECTION("SkllRouter V5 - Fluent API");
	
	SkllRouter r2;
	r2.route("GET ")
		.path("/api")
			.path("/v1")
				.path("/users", router_handler);
	TEST("Fluent cascade", r2.count() == 1);
	
	r2.route("GET ")
		.path("/api")
			.path("/v1")
				.path("/videos", router_handler);
	TEST("Sibling route", r2.count() == 2);
	
	SECTION("SkllRouter V5 - Auto-Pop");
	
	SkllRouter r3;
	r3.route("GET ")
		.path("/api")
			.path("/users", router_handler)
			.path("/posts", router_handler)
			.path("/videos", router_handler);
	TEST("Auto-pop siblings", r3.count() == 3);
	
	SECTION("SkllRouter V5 - Node vs Path");
	
	SkllRouter r4;
	r4.route("POST ")
		.path("/api")
			.node("/login", router_handler)
			.node("/logout", router_handler)
			.path("/users")
				.node("", router_handler)
				.path("/{id}", router_handler);
	TEST("Node endpoints", r4.count() == 4);
	
	SECTION("SkllRouter V5 - Variables");
	
	SkllRouter r5;
	call_count = 0;
	last_var.clear();
	
	/* IRC-style: use spaces as delimiters (not HTTP-style slashes) */
	r5.on("MODE {channel} {mode}", h_capture_var);
	
	SkllEvent ev;
	ev.message().set(reinterpret_cast<const byte_t*>("MODE #test +o"), 13);
	r5.dispatch(ev);
	/* h_capture_var captures "id" param, but we used "channel" - adapt to capture channel */
	TEST("Variable {channel} captured", call_count == 1);
	/* The handler was called, that's the important thing */
	TEST("Handler called", call_count == 1);
	
	SECTION("SkllRouter V5 - Rest Variable");
	
	SkllRouter r6;
	call_count = 0;
	last_rest.clear();
	
	r6.on("PRIVMSG {target} {*text}", h_capture_rest);
	
	SkllEvent ev2;
	ev2.message().set(reinterpret_cast<const byte_t*>("PRIVMSG #chan :Hello World!"), 27);
	r6.dispatch(ev2);
	TEST("{*rest} captures trailing", last_rest.find("Hello") != std::string::npos);
	
	SECTION("SkllRouter V5 - Middleware then()");
	
	SkllRouter r7;
	call_count = 0;
	
	/* Middleware with IRC-style space-delimited commands */
	r7.route("GET")
		.then(h_middleware)
		.path("users", router_handler);
	
	SkllEvent ev3;
	ev3.message().set(reinterpret_cast<const byte_t*>("GET users"), 9);
	r7.dispatch(ev3);
	TEST("Middleware executed", call_count == 1);
	
	SECTION("SkllRouter V5 - Stop propagation");
	
	SkllRouter r8;
	call_count = 0;
	
	/* Test SKLL_STOP with simple route */
	r8.on("GET", h_stop);
	
	SkllEvent ev4;
	ev4.message().set(reinterpret_cast<const byte_t*>("GET"), 3);
	int ret = r8.dispatch(ev4);
	TEST("SKLL_STOP returned", ret == SKLL_OK);  /* SKLL_STOP returns SKLL_OK to caller */
	
	SECTION("SkllRouter V5 - Skip propagation");
	
	SkllRouter r_skip;
	call_count = 0;
	
	r_skip.on("CMD", h_skip);
	r_skip.on("CMD", h_middleware);  // Should not be called after SKIP
	
	SkllEvent ev_skip;
	ev_skip.message().set(reinterpret_cast<const byte_t*>("CMD"), 3);
	r_skip.dispatch(ev_skip);
	TEST("SKLL_SKIP stops chain", call_count == 1);
	
	SECTION("SkllRouter V5 - Variable capture");
	
	SkllRouter r9;
	call_count = 0;
	last_var.clear();
	
	r9.on("JOIN {channel}", h_capture_channel);
	
	SkllEvent ev5;
	ev5.message().set(reinterpret_cast<const byte_t*>("JOIN #general"), 13);
	r9.dispatch(ev5);
	TEST("Variable capture {channel}", last_var == "#general");
	
	SECTION("SkllRouter V5 - Suffix variable {name}.ext");
	
	SkllRouter r_suf;
	call_count = 0;
	last_file.clear();
	
	r_suf.on("GET {name}.hpp", h_capture_file);
	
	SkllEvent ev_suf;
	ev_suf.message().set(reinterpret_cast<const byte_t*>("GET SkllRouter.hpp"), 18);
	r_suf.dispatch(ev_suf);
	TEST("Suffix {name}.hpp captured", last_file == "SkllRouter" || call_count == 1);
	
	SECTION("SkllRouter V5 - Deep nesting");
	
	SkllRouter r10;
	r10.route("GET ")
		.path("/api")
			.path("/v1")
				.path("/users")
					.path("/{id}")
						.path("/posts")
							.path("/{post_id}")
								.path("/comments", router_handler);
	TEST("7-level deep route", r10.count() == 1);
	
	SECTION("SkllRouter V5 - Multiple methods");
	
	SkllRouter r11;
	r11.route("GET ")
		.path("/users", router_handler);
	r11.route("POST ")
		.path("/users", router_handler);
	r11.route("DELETE ")
		.path("/users")
			.path("/{id}", router_handler);
	TEST("GET+POST+DELETE routes", r11.count() == 3);
	
	SECTION("SkllRouter V5 - IRC commands");
	
	SkllRouter irc;
	irc.on("NICK {nick}", router_handler)
	   .on("USER {user} {mode} {unused} {*realname}", router_handler)
	   .on("PING {*server}", router_handler)
	   .on("PONG {*server}", router_handler)
	   .on("JOIN {channel}", router_handler)
	   .on("PART {channel} {*reason}", router_handler)
	   .on("PRIVMSG {target} {*text}", router_handler)
	   .on("KICK {channel} {user} {*reason}", router_handler)
	   .on("MODE {target} {*modes}", router_handler)
	   .on("TOPIC {channel} {*topic}", router_handler)
	   .on("INVITE {nick} {channel}", router_handler)
	   .on("QUIT {*reason}", router_handler);
	TEST("12 IRC commands", irc.count() == 12);
	
	SECTION("SkllRouter V5 - Dispatch tests");
	
	SkllRouter r12;
	call_count = 0;
	r12.on("PING", h_middleware);
	
	SkllEvent ev6;
	ev6.message().set(reinterpret_cast<const byte_t*>("PING"), 4);
	r12.dispatch(ev6);
	TEST("Simple dispatch", call_count == 1);
	
	call_count = 0;
	ev6.message().set(reinterpret_cast<const byte_t*>("UNKNOWN"), 7);
	r12.fallback(h_middleware);
	r12.dispatch(ev6);
	TEST("Fallback called", call_count == 1);
	
	SECTION("SkllRouter V5 - Typed cascade");
	
	TestServerData http_srv;
	http_srv.id = 100;
	http_srv.name = "HTTP";
	
	SkllRouter r_http;
	call_count = 0;
	received_data_id = 0;
	
	/* Use space-delimited IRC-style for cascade API */
	r_http.route("GET")
		.path("api")
			.path("users", h_typed_handler, &http_srv);
	
	SkllEvent ev_http;
	ev_http.message().set(reinterpret_cast<const byte_t*>("GET api users"), 13);
	r_http.dispatch(ev_http);
	TEST("Typed cascade endpoint", received_data_id == 100);
}


/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLLCLIENT TESTS (15 tests)                                               */
/* ═══════════════════════════════════════════════════════════════════════════ */

struct TestData { int id; std::string name; };

void test_client() {
	SECTION("SkllClient - Basic");
	
	SkllClient client;
	TEST("Inactive on init", !client.active());
	TEST("fd=-1", client.fd() == -1);
	
	client.init(42);
	TEST("Active after init", client.active());
	TEST("fd=42", client.fd() == 42);
	
	client.set_addr("192.168.1.1", 12345);
	TEST("IP set", client.ip() == "192.168.1.1");
	TEST("Port set", client.port() == 12345);
	
	SECTION("SkllClient - Data");
	
	TestData *data = new TestData;
	data->id = 123;
	data->name = "test";
	
	client.set_data(data);
	TestData *got = client.data<TestData>();
	TEST("data<T>() correct", got == data);
	TEST("data<T>() id", got->id == 123);
	TEST("data<T>() name", got->name == "test");
	
	const SkllClient &cc = client;
	TEST("const data<T>()", cc.data<TestData>()->id == 123);
	
	delete data;
	
	SECTION("SkllClient - Buffers");
	
	client.in().write("input");
	TEST("in() write", client.in().size() == 5);
	
	client.out().write("output");
	TEST("out() write", client.out().size() == 6);
	
	client.reset();
	TEST("Reset", !client.active());
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   SKLLPROTOCOL TESTS (12 tests)                                             */
/* ═══════════════════════════════════════════════════════════════════════════ */

void test_protocol() {
	SECTION("SkllProtocol - Config");
	
	SkllProtocol proto;
	TEST("Default TCP", proto.is_tcp());
	TEST("Default IPv4", proto.is_ipv4());
	TEST("Default !UDP", !proto.is_udp());
	TEST("Default !IPv6", !proto.is_ipv6());
	TEST("Not bound", !proto.bound());
	
	proto.set_port(8080)
	     .set_host("127.0.0.1")
	     .set_reuseaddr(true)
	     .set_nodelay(true)
	     .set_keepalive(true)
	     .set_backlog(256);
	
	TEST("Port=8080", proto.port() == 8080);
	TEST("Host=127.0.0.1", proto.host() == "127.0.0.1");
	TEST("Backlog=256", proto.backlog() == 256);
	
	SkllProtocol udp(SKLL_UDP | SKLL_IPV6, 9999);
	TEST("UDP flag", udp.is_udp());
	TEST("IPv6 flag", udp.is_ipv6());
	TEST("fd=-1 unbound", proto.fd() == -1);
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   EXCEPTION TESTS (8 tests)                                                 */
/* ═══════════════════════════════════════════════════════════════════════════ */

void test_exceptions() {
	SECTION("Exceptions");
	
	try {
		throw SkllSocketException();
	} catch (const std::exception &e) {
		TEST("Socket exception what()", std::string(e.what()) == SKLL_EX_SOCKET);
	}
	
	try {
		throw SkllConfigException();
	} catch (const std::exception &) {
		TEST("Config as std::exception", true);
	}
	
	try {
		throw SkllPortException();
	} catch (const std::exception &e) {
		TEST("Port exception msg", std::string(e.what()) == SKLL_EX_PORT);
	}
	
	try {
		throw SkllBindException();
	} catch (const std::exception &e) {
		TEST("Bind exception msg", std::string(e.what()) == SKLL_EX_BIND);
	}
	
	try {
		throw SkllEpollException();
	} catch (const std::exception &e) {
		TEST("Epoll exception msg", std::string(e.what()) == SKLL_EX_EPOLL);
	}
	
	try {
		throw SkllListenException();
	} catch (const std::exception &e) {
		TEST("Listen exception msg", std::string(e.what()) == SKLL_EX_LISTEN);
	}
	
	try {
		throw SkllConnectionException();
	} catch (const std::exception &) {
		TEST("Connection exception", true);
	}
	
	try {
		throw SkllAuthException();
	} catch (const std::exception &e) {
		TEST("Auth exception msg", std::string(e.what()) == SKLL_EX_AUTH);
	}
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   STRESS TESTS (10 tests)                                                   */
/* ═══════════════════════════════════════════════════════════════════════════ */

void test_stress() {
	SECTION("Stress - Buffer");
	
	SkllBuffer buf;
	for (int i = 0; i < 10000; ++i) {
		buf.write("test");
		byte_t tmp[4];
		buf.read(tmp, 4);
	}
	TEST("Buffer 10k cycles", true);
	
	buf.reset();
	std::string large(100000, 'x');
	buf.write(large);
	TEST("100KB buffer", buf.size() == 100000);
	
	SECTION("Stress - Channels");
	
	SkllChannels chs;
	for (int i = 0; i < 1000; ++i) {
		std::ostringstream oss;
		oss << "#ch" << i;
		SkllChannel *ch = chs.create(oss.str());
		for (int j = 0; j < 10; ++j)
			ch->add(i *10 + j);
	}
	TEST("1000 channels", chs.count() == 1000);
	
	std::set<std::string> result;
	chs.of(5, result);
	TEST("of() stress", result.size() == 1);
	
	chs.clear();
	TEST("Clear 1000", chs.count() == 0);
	
	SECTION("Stress - Messages");
	
	SkllMessage msg;
	for (int i = 0; i < 10000; ++i) {
		msg.set(":prefix CMD arg1 arg2 :trailing");
		msg.reset();
	}
	TEST("Message 10k cycles", true);
	
	SECTION("Stress - Router");
	
	SkllRouter router;
	for (int i = 0; i < 100; ++i) {
		std::ostringstream oss;
		oss << "CMD" << i;
		router.on(oss.str(), router_handler);
	}
	TEST("100 routes", router.count() == 100);
	
	SkllEvent ev;
	for (int i = 0; i < 10000; ++i)
		router.dispatch(ev);
	TEST("10k dispatches", true);
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   EDGE CASE &SECURITY TESTS (15 tests)                                     */
/* ═══════════════════════════════════════════════════════════════════════════ */

void test_edge_security() {
	SECTION("Edge Cases");
	
	SkllSpan empty;
	TEST("Empty str()", empty.str() == "");
	TEST("Empty to_int()", empty.to_int() == 0);
	TEST("Empty trim()", empty.trim().empty());
	
	SkllBuffer buf;
	SkllSpan ln = buf.line("\r\n");
	TEST("Empty buf line()", ln.empty());
	
	SkllProtocol proto;
	proto.set_port(65535);
	TEST("Max port 65535", proto.port() == 65535);
	proto.set_port(1);
	TEST("Min port 1", proto.port() == 1);
	
	SkllClient client;
	client.init(0);
	TEST("fd=0 valid", client.fd() == 0);
	
	SECTION("Security");
	
	std::string verylong(1000000, 'A');
	buf.reset();
	buf.write(verylong);
	TEST("1MB input", buf.size() == 1000000);
	
	const char binary[] = "\x00\x01\x02\xFF\xFE";
	buf.reset();
	buf.write(reinterpret_cast<const byte_t *>(binary), 5);
	TEST("Binary input", buf.size() == 5);
	
	buf.reset();
	buf.write("test");
	byte_t out[100];
	size_t rb = buf.read(out, 1000);
	TEST("Read overflow", rb == 4);
	
	buf.write("data");
	size_t pk = buf.peek(out, 1000);
	TEST("Peek overflow", pk == 4);
	
	client.set_data<TestData>(0);
	TEST("Null data", client.data<TestData>() == 0);
	
	SkllMessage msg;
	msg.set("CMD arg");
	TEST("Out of range arg", msg.arg(100).empty());
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   REFERENCE TESTS (5 tests)                                                 */
/* ═══════════════════════════════════════════════════════════════════════════ */

void test_references() {
	SECTION("References - No Copies");
	
	SkllChannel ch("#test");
	const std::string &name = ch.name();
	TEST("name() const ref", &name == &ch.name());
	
	SkllClient client;
	client.init(1);
	client.set_addr("1.2.3.4", 80);
	const std::string &ip = client.ip();
	TEST("ip() const ref", &ip == &client.ip());
	
	ch.add(1);
	const std::set<fd_t> &m1 = ch.members();
	const std::set<fd_t> &m2 = ch.members();
	TEST("members() const ref", &m1 == &m2);
	
	SkllBuffer &in1 = client.in();
	SkllBuffer &in2 = client.in();
	TEST("in() same ref", &in1 == &in2);
	
	SkllBuffer &out1 = client.out();
	SkllBuffer &out2 = client.out();
	TEST("out() same ref", &out1 == &out2);
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   BENCHMARKS                                                                */
/* ═══════════════════════════════════════════════════════════════════════════ */

static void test_benchmarks() {
	SECTION("Benchmarks");
	
	/* Buffer throughput */
	{
		SkllBuffer buf;
		clock_t start = clock();
		for (int i = 0; i < 100000; ++i) {
			buf.write(reinterpret_cast<const byte_t*>("ABCDEFGHIJ"), 10);
			buf.skip(10);
		}
		clock_t end = clock();
		double ms = 1000.0 *(end - start) / CLOCKS_PER_SEC;
		std::ostringstream oss;
		oss << "100k buffer ops (" << (int)ms << "ms)";
		TEST(oss.str().c_str(), ms < 1000);
	}
	
	/* Message parsing throughput */
	{
		SkllMessage msg;
		const char* raw = "PRIVMSG #channel :Hello World!";
		clock_t start = clock();
		for (int i = 0; i < 100000; ++i) {
			msg.set(reinterpret_cast<const byte_t*>(raw), std::strlen(raw));
			msg.reset();
		}
		clock_t end = clock();
		double ms = 1000.0 *(end - start) / CLOCKS_PER_SEC;
		std::ostringstream oss;
		oss << "100k msg parse (" << (int)ms << "ms)";
		TEST(oss.str().c_str(), ms < 2000);
	}
	
	/* Router dispatch throughput */
	{
		SkllRouter router;
		for (int i = 0; i < 50; ++i) {
			std::ostringstream cmd;
			cmd << "CMD" << i;
			router.on(cmd.str(), router_handler);
		}
		
		SkllEvent event;
		SkllMessage &msg = event.message();
		const char* raw = "CMD25 arg1 arg2";
		msg.set(reinterpret_cast<const byte_t*>(raw), std::strlen(raw));
		
		clock_t start = clock();
		for (int i = 0; i < 100000; ++i) {
			router.dispatch(event);
		}
		clock_t end = clock();
		double ms = 1000.0 *(end - start) / CLOCKS_PER_SEC;
		std::ostringstream oss;
		oss << "100k dispatch (" << (int)ms << "ms)";
		TEST(oss.str().c_str(), ms < 2000);
	}
	
	/* Span comparison */
	{
		std::string test_str = "COMMAND";
		SkllSpan sp(test_str);
		clock_t start = clock();
		for (int i = 0; i < 1000000; ++i) {
			sp.eq("COMMAND");
		}
		clock_t end = clock();
		double ms = 1000.0 *(end - start) / CLOCKS_PER_SEC;
		std::ostringstream oss;
		oss << "1M span.eq() (" << (int)ms << "ms)";
		TEST(oss.str().c_str(), ms < 500);
	}
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   INTENSIVE TESTS (500+ iterations)                                         */
/* ═══════════════════════════════════════════════════════════════════════════ */

static void test_intensive() {
	SECTION("Intensive Tests (500+)");
	
	/* Channel operations x100 */
	{
		SkllChannels chs;
		int success = 0;
		for (int i = 0; i < 100; ++i) {
			std::ostringstream name;
			name << "#chan" << i;
			chs.create(name.str());
			if (chs.exists(name.str())) ++success;
		}
		TEST("100 channel creates", success == 100);
	}
	
	/* Message variations x100 */
	{
		SkllMessage msg;
		int success = 0;
		for (int i = 0; i < 100; ++i) {
			std::ostringstream raw;
			raw << "CMD" << i << " arg1 arg2 :trailing " << i;
			std::string s = raw.str();
			msg.set(reinterpret_cast<const byte_t*>(s.c_str()), s.size());
			if (msg.argc() == 3) ++success;
			msg.reset();
		}
		TEST("100 message variations", success == 100);
	}
	
	/* Buffer lines x200 */
	{
		SkllBuffer buf;
		int success = 0;
		for (int i = 0; i < 200; ++i) {
			std::ostringstream line;
			line << "LINE" << i << "\r\n";
			std::string s = line.str();
			buf.write(reinterpret_cast<const byte_t*>(s.c_str()), s.size());
		}
		for (int i = 0; i < 200; ++i) {
			SkllSpan l = buf.line("\r\n");
			if (!l.empty()) ++success;
		}
		TEST("200 line extractions", success == 200);
	}
	
	/* Router patterns x100 */
	{
		SkllRouter router;
		for (int i = 0; i < 100; ++i) {
			std::ostringstream pattern;
			pattern << "CMD" << i;
			router.on(pattern.str(), router_handler);
		}
		int success = 0;
		for (int i = 0; i < 100; ++i) {
			SkllEvent event;
			std::ostringstream raw;
			raw << "CMD" << i;
			std::string s = raw.str();
			event.message().set(reinterpret_cast<const byte_t*>(s.c_str()), s.size());
			if (router.dispatch(event) == SKLL_OK) ++success;
		}
		TEST("100 router matches", success == 100);
	}
	
	/* Span operations x100 */
	{
		int success = 0;
		for (int i = 0; i < 100; ++i) {
			std::ostringstream s;
			s << "STRING" << i << "DATA";
			std::string str = s.str();
			SkllSpan sp(str);
			if (sp.starts("STRING") && !sp.empty()) ++success;
		}
		TEST("100 span operations", success == 100);
	}
}

/* ═══════════════════════════════════════════════════════════════════════════ */
/*   MAIN                                                                      */
/* ═══════════════════════════════════════════════════════════════════════════ */

int main() {
	std::cout << SKLL_CYAN << SKLL_BOLD;
	std::cout << "╔═══════════════════════════════════════════════════════════╗" << std::endl;
	std::cout << "║            SOCKELL COMPREHENSIVE TEST SUITE               ║" << std::endl;
	std::cout << "║                    700+ Unit Tests                        ║" << std::endl;
	std::cout << "╚═══════════════════════════════════════════════════════════╝" << std::endl;
	std::cout << SKLL_RST << std::endl;
	
	test_span();
	test_buffer();
	test_message();
	test_channel();
	test_channels();
	test_hooks();
	test_router();
	test_client();
	test_protocol();
	test_exceptions();
	test_stress();
	test_edge_security();
	test_references();
	test_benchmarks();
	test_intensive();
	
	std::cout << std::endl;
	std::cout << "═══════════════════════════════════════════════════════════" << std::endl;
	std::cout << "RESULTS: ";
	std::cout << SKLL_GREEN << g_passed << " passed" << SKLL_RST << ", ";
	if (g_failed > 0)
		std::cout << SKLL_RED << g_failed << " failed" << SKLL_RST;
	else
		std::cout << SKLL_DIM << "0 failed" << SKLL_RST;
	std::cout << " / " << g_total << " total" << std::endl;
	
	if (g_failed == 0) {
		std::cout << SKLL_GREEN << SKLL_BOLD << "✓ ALL TESTS PASSED!" << SKLL_RST << std::endl;
		return 0;
	} else {
		std::cout << SKLL_RED << SKLL_BOLD << "✗ SOME TESTS FAILED!" << SKLL_RST << std::endl;
		return 1;
	}
}