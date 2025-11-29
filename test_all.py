#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
+-----------------------------------------------------------------------+
|  SOCKELL IRC SERVER - AUTOMATED TEST SUITE                            |
|  Tests all IRC routes: PASS, NICK, USER, JOIN, PART, PRIVMSG, etc.    |
+-----------------------------------------------------------------------+

Usage:
    python3 test_all.py [port] [password]
    
    Default: port=6667, password=test123
"""

import socket
import sys
import time
import random
import string

# Config
HOST = "127.0.0.1"
PORT = 6667
PASSWORD = "test123"
TIMEOUT = 2.0

# Colors
G = "\033[92m"  # Green
R = "\033[91m"  # Red
Y = "\033[93m"  # Yellow
C = "\033[96m"  # Cyan
D = "\033[2m"   # Dim
B = "\033[1m"   # Bold
X = "\033[0m"   # Reset


class IRC:
    """Simple IRC client for testing"""
    
    def __init__(self):
        self.sock = None
        self.buf = ""
    
    def connect(self):
        self.sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.sock.settimeout(TIMEOUT)
        self.sock.connect((HOST, PORT))
        self.buf = ""
        return True
    
    def close(self):
        if self.sock:
            try: self.sock.close()
            except: pass
            self.sock = None
    
    def send(self, msg):
        if not msg.endswith("\r\n"):
            msg += "\r\n"
        self.sock.sendall(msg.encode())
    
    def recv(self, timeout=TIMEOUT):
        self.sock.settimeout(timeout)
        try:
            data = self.sock.recv(8192).decode('utf-8', errors='replace')
            self.buf += data
            return data
        except socket.timeout:
            return ""
    
    def recv_all(self, timeout=0.5):
        """Receive all available data"""
        result = ""
        end = time.time() + timeout
        while time.time() < end:
            try:
                self.sock.settimeout(0.1)
                data = self.sock.recv(8192).decode('utf-8', errors='replace')
                if data:
                    result += data
                    self.buf += data
            except socket.timeout:
                break
            except:
                break
        return result
    
    def wait_for(self, pattern, timeout=TIMEOUT):
        """Wait until pattern is found in received data"""
        end = time.time() + timeout
        while time.time() < end:
            if pattern in self.buf:
                return self.buf
            try:
                self.sock.settimeout(0.1)
                data = self.sock.recv(4096).decode('utf-8', errors='replace')
                if data:
                    self.buf += data
            except socket.timeout:
                continue
            except:
                break
        return self.buf
    
    def clear(self):
        self.buf = ""
    
    def register(self, nick, pwd=PASSWORD):
        """Complete IRC registration and wait for welcome"""
        self.send(f"PASS {pwd}")
        self.send(f"NICK {nick}")
        self.send(f"USER {nick} 0 *:Test User {nick}")
        # Wait for 004 (last welcome message)
        return self.wait_for("004", timeout=2)


class Tests:
    def __init__(self):
        self.ok = 0
        self.fail = 0
    
    def test(self, name, cond, info=""):
        if cond:
            self.ok += 1
            print(f"  {G}✓{X} {name}")
        else:
            self.fail += 1
            print(f"  {R}✗{X} {name}")
            if info:
                print(f"    {Y}→ {info}{X}")
        return cond
    
    def section(self, name):
        print(f"\n{C}{B}━━━ {name} ━━━{X}")
    
    def summary(self):
        total = self.ok + self.fail
        print(f"\n{B}{'=' *70}{X}")
        if self.fail == 0:
            print(f"{G}{B}✓ ALL {total} TESTS PASSED{X}")
        else:
            print(f"{R}{B}✗ {self.fail}/{total} TESTS FAILED{X}")
            print(f"{G}✓ {self.ok} passed{X}")
        print(f"{B}{'=' *70}{X}")
        return self.fail == 0


def rnd():
    return "t" + ''.join(random.choices(string.ascii_lowercase, k=5))


# ═══════════════════════════════════════════════════════════════════════════════
#   TEST SUITES
# ═══════════════════════════════════════════════════════════════════════════════

def test_connection(t):
    t.section("CONNECTION")
    
    c = IRC()
    try:
        c.connect()
        t.test("TCP connection", True)
        c.close()
    except Exception as e:
        t.test("TCP connection", False, str(e))
    
    clients = []
    try:
        for i in range(3):
            c = IRC()
            c.connect()
            clients.append(c)
        t.test("Multiple connections (3)", len(clients) == 3)
    except Exception as e:
        t.test("Multiple connections", False, str(e))
    finally:
        for c in clients:
            c.close()


def test_registration(t):
    t.section("REGISTRATION")
    
    # Wrong password
    c = IRC()
    try:
        c.connect()
        c.send(f"PASS wrongpassword")
        c.send(f"NICK testwrong")
        c.send(f"USER testwrong 0 *:Test")
        data = c.wait_for("464", timeout=1)
        t.test("Wrong password → 464", "464" in data)
        c.close()
    except Exception as e:
        t.test("Wrong password", False, str(e))
    
    # Correct registration
    c = IRC()
    try:
        c.connect()
        data = c.register("testok")
        t.test("Registration → 001 RPL_WELCOME", "001" in data)
        t.test("Registration → 002 RPL_YOURHOST", "002" in data)
        t.test("Registration → 003 RPL_CREATED", "003" in data)
        t.test("Registration → 004 RPL_MYINFO", "004" in data)
        c.close()
    except Exception as e:
        t.test("Correct registration", False, str(e))
    
    # Duplicate nick
    c1 = IRC()
    c2 = IRC()
    try:
        c1.connect()
        c1.register("dupnick")
        
        c2.connect()
        c2.send(f"PASS {PASSWORD}")
        c2.send("NICK dupnick")
        c2.send("USER dup 0 *:Test")
        data = c2.wait_for("433", timeout=1)
        t.test("Duplicate nick → 433", "433" in data)
    except Exception as e:
        t.test("Duplicate nick", False, str(e))
    finally:
        c1.close()
        c2.close()
    
    # Invalid nick
    c = IRC()
    try:
        c.connect()
        c.send(f"PASS {PASSWORD}")
        c.send("NICK 123bad")
        data = c.wait_for("432", timeout=1)
        t.test("Invalid nick (digit start) → 432", "432" in data)
        c.close()
    except Exception as e:
        t.test("Invalid nick", False, str(e))


def test_channels(t):
    t.section("CHANNELS")
    
    c = IRC()
    try:
        c.connect()
        c.register("chantest")
        c.clear()
        
        c.send("JOIN #testchan")
        data = c.wait_for("366", timeout=2)
        t.test("JOIN → JOIN message", "JOIN" in data and "#testchan" in data)
        t.test("JOIN → 353 RPL_NAMREPLY", "353" in data)
        t.test("JOIN → 366 RPL_ENDOFNAMES", "366" in data)
        t.test("JOIN creator → @nick", "@chantest" in data)
        
        c.clear()
        c.send("PART #testchan :Bye!")
        data = c.wait_for("PART", timeout=1)
        t.test("PART → PART message", "PART" in data and "#testchan" in data)
        
        c.close()
    except Exception as e:
        t.test("Channel ops", False, str(e))
    
    # Invalid channel
    c = IRC()
    try:
        c.connect()
        c.register("badchan")
        c.clear()
        c.send("JOIN nochanprefix")
        data = c.wait_for("403", timeout=1)
        t.test("JOIN without # → 403", "403" in data)
        c.close()
    except Exception as e:
        t.test("Invalid channel", False, str(e))


def test_messaging(t):
    t.section("MESSAGING")
    
    c1 = IRC()
    c2 = IRC()
    try:
        c1.connect()
        c1.register("sender")
        
        c2.connect()
        c2.register("receiver")
        
        c1.send("JOIN #msgtest")
        c1.wait_for("366", timeout=1)
        c2.send("JOIN #msgtest")
        c2.wait_for("366", timeout=1)
        
        c1.clear()
        c2.clear()
        
        # Channel message
        c1.send("PRIVMSG #msgtest :Hello channel!")
        time.sleep(0.3)
        data = c2.recv_all(0.5)
        t.test("PRIVMSG to channel", "Hello channel" in data)
        
        c2.clear()
        
        # Direct message
        c1.send("PRIVMSG receiver :Hello direct!")
        time.sleep(0.3)
        data = c2.recv_all(0.5)
        t.test("PRIVMSG to user", "Hello direct" in data)
        
        c2.clear()
        
        # Notice
        c1.send("NOTICE receiver :Notice msg")
        time.sleep(0.3)
        data = c2.recv_all(0.5)
        t.test("NOTICE to user", "Notice msg" in data)
        
    except Exception as e:
        t.test("Messaging", False, str(e))
    finally:
        c1.close()
        c2.close()
    
    # Unknown nick
    c = IRC()
    try:
        c.connect()
        c.register("msgfail")
        c.clear()
        c.send("PRIVMSG nosuchuser :Hello?")
        data = c.wait_for("401", timeout=1)
        t.test("PRIVMSG unknown → 401", "401" in data)
        c.close()
    except Exception as e:
        t.test("PRIVMSG unknown", False, str(e))


def test_operator(t):
    t.section("OPERATOR")
    
    c1 = IRC()
    c2 = IRC()
    
    try:
        c1.connect()
        c1.register("operator")
        c1.send("JOIN #optest")
        c1.wait_for("366", timeout=1)
        
        c2.connect()
        c2.register("target")
        c2.send("JOIN #optest")
        c2.wait_for("366", timeout=1)
        
        c1.clear()
        c2.clear()
        
        # TOPIC
        c1.send("TOPIC #optest :New Topic!")
        time.sleep(0.3)
        data = c2.recv_all(0.5)
        t.test("TOPIC set → broadcast", "TOPIC" in data and "New Topic" in data)
        
        c1.clear()
        c1.send("TOPIC #optest")
        data = c1.wait_for("332", timeout=1)
        t.test("TOPIC query → 332", "332" in data and "New Topic" in data)
        
        c2.clear()
        
        # KICK
        c1.send("KICK #optest target :Get out!")
        time.sleep(0.3)
        data = c2.recv_all(0.5)
        t.test("KICK → message", "KICK" in data and "Get out" in data)
        
    except Exception as e:
        t.test("Operator commands", False, str(e))
    finally:
        c1.close()
        c2.close()
    
    # INVITE
    c1 = IRC()
    c2 = IRC()
    try:
        c1.connect()
        c1.register("inviter")
        c1.send("JOIN #invtest")
        c1.wait_for("366", timeout=1)
        
        c2.connect()
        c2.register("invitee")
        
        c1.clear()
        c2.clear()
        
        c1.send("INVITE invitee #invtest")
        time.sleep(0.3)
        d1 = c1.recv_all(0.5)
        d2 = c2.recv_all(0.5)
        t.test("INVITE → 341", "341" in d1)
        t.test("INVITE → received", "INVITE" in d2)
        
    except Exception as e:
        t.test("INVITE", False, str(e))
    finally:
        c1.close()
        c2.close()


def test_modes(t):
    t.section("MODES")
    
    c = IRC()
    try:
        c.connect()
        c.register("modetest")
        c.send("JOIN #modetest")
        c.wait_for("366", timeout=1)
        c.clear()
        
        # Query
        c.send("MODE #modetest")
        data = c.wait_for("324", timeout=1)
        t.test("MODE query → 324", "324" in data)
        
        c.clear()
        c.send("MODE #modetest +i")
        data = c.wait_for("MODE", timeout=1)
        t.test("MODE +i → broadcast", "MODE" in data and "+i" in data)
        
        c.clear()
        c.send("MODE #modetest +t")
        data = c.wait_for("MODE", timeout=1)
        t.test("MODE +t → broadcast", "MODE" in data and "+t" in data)
        
        c.clear()
        c.send("MODE #modetest +k secretkey")
        data = c.wait_for("MODE", timeout=1)
        t.test("MODE +k → broadcast", "MODE" in data and "+k" in data)
        
        c.clear()
        c.send("MODE #modetest +l 50")
        data = c.wait_for("MODE", timeout=1)
        t.test("MODE +l → broadcast", "MODE" in data and "+l" in data)
        
        c.close()
    except Exception as e:
        t.test("MODE", False, str(e))


def test_bot(t):
    t.section("BOT COMMANDS")
    
    c = IRC()
    try:
        c.connect()
        c.register("bottest")
        c.send("JOIN #bottest")
        c.wait_for("366", timeout=1)
        c.clear()
        
        c.send("PRIVMSG #bottest :!help")
        data = c.wait_for("SockBot", timeout=1)
        t.test("!help → SockBot", "SockBot" in data)
        
        c.clear()
        c.send("PRIVMSG #bottest :!time")
        data = c.wait_for("time", timeout=1)
        t.test("!time → response", "time" in data.lower() or "Time" in data)
        
        c.clear()
        c.send("PRIVMSG #bottest :!stats")
        data = c.wait_for("Users", timeout=1)
        t.test("!stats → response", "Users" in data or "users" in data.lower())
        
        c.clear()
        c.send("PRIVMSG #bottest :!roll")
        data = c.wait_for("rolled", timeout=1)
        t.test("!roll → response", "rolled" in data)
        
        c.clear()
        c.send("PRIVMSG #bottest :!uptime")
        data = c.wait_for("Uptime", timeout=1)
        t.test("!uptime → response", "ptime" in data.lower())
        
        c.close()
    except Exception as e:
        t.test("Bot commands", False, str(e))


def test_ping(t):
    t.section("PING/PONG")
    
    c = IRC()
    try:
        c.connect()
        c.register("pingtest")
        c.clear()
        
        c.send("PING :token123")
        data = c.wait_for("PONG", timeout=1)
        t.test("PING → PONG", "PONG" in data)
        t.test("PONG has token", "token123" in data)
        
        c.close()
    except Exception as e:
        t.test("PING/PONG", False, str(e))


def test_quit(t):
    t.section("QUIT")
    
    c1 = IRC()
    c2 = IRC()
    
    try:
        c1.connect()
        c1.register("quitter")
        c1.send("JOIN #quitest")
        c1.wait_for("366", timeout=1)
        
        c2.connect()
        c2.register("watcher")
        c2.send("JOIN #quitest")
        c2.wait_for("366", timeout=1)
        c2.clear()
        
        c1.send("QUIT :Goodbye cruel world!")
        time.sleep(0.3)
        data = c2.recv_all(0.5)
        t.test("QUIT → broadcast", "QUIT" in data)
        t.test("QUIT → reason", "Goodbye" in data)
        
    except Exception as e:
        t.test("QUIT", False, str(e))
    finally:
        c1.close()
        c2.close()


def test_edge(t):
    t.section("EDGE CASES")
    
    # Empty PRIVMSG
    c = IRC()
    try:
        c.connect()
        c.register("edgetest")
        c.clear()
        c.send("PRIVMSG #test :")
        data = c.wait_for("412", timeout=1)
        t.test("Empty PRIVMSG → 412", "412" in data)
        c.close()
    except Exception as e:
        t.test("Empty PRIVMSG", False, str(e))
    
    # Unknown command
    c = IRC()
    try:
        c.connect()
        c.register("unknown")
        c.clear()
        c.send("FAKECOMMAND arg1")
        data = c.wait_for("421", timeout=1)
        t.test("Unknown cmd → 421", "421" in data)
        c.close()
    except Exception as e:
        t.test("Unknown cmd", False, str(e))
    
    # Long message
    c = IRC()
    try:
        c.connect()
        c.register("longmsg")
        c.send("JOIN #longtest")
        c.wait_for("366", timeout=1)
        c.clear()
        c.send(f"PRIVMSG #longtest :{'A' *400}")
        time.sleep(0.2)
        t.test("Long message (400)", True)
        c.close()
    except Exception as e:
        t.test("Long message", False, str(e))


def test_concurrent(t):
    t.section("CONCURRENT")
    
    clients = []
    try:
        for i in range(10):
            c = IRC()
            c.connect()
            c.register(f"conc{i}")
            c.send("JOIN #concurrent")
            c.wait_for("366", timeout=1)
            clients.append(c)
        
        t.test("10 clients registered", len(clients) == 10)
        
        for c in clients:
            c.clear()
        
        clients[0].send("PRIVMSG #concurrent :Broadcast test!")
        time.sleep(0.5)
        
        received = 0
        for c in clients[1:]:
            data = c.recv_all(0.3)
            if "Broadcast test" in data:
                received += 1
        
        t.test(f"Broadcast ({received}/9)", received >= 7)
        
    except Exception as e:
        t.test("Concurrent", False, str(e))
    finally:
        for c in clients:
            c.close()


# ═══════════════════════════════════════════════════════════════════════════════
#   MAIN
# ═══════════════════════════════════════════════════════════════════════════════

def main():
    global PORT, PASSWORD
    
    if len(sys.argv) >= 2:
        PORT = int(sys.argv[1])
    if len(sys.argv) >= 3:
        PASSWORD = sys.argv[2]
    
    print()
    print(f"{B}+-----------------------------------------------------------------------+{X}")
    print(f"{B}|{X}         {C}SOCKELL IRC SERVER - AUTOMATED TEST SUITE{X}                   {B}|{X}")
    print(f"{B}+-----------------------------------------------------------------------+{X}")
    print()
    print(f"  Target: {C}{HOST}:{PORT}{X}")
    print(f"  Password: {Y}{PASSWORD}{X}")
    
    # Check server
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(1)
        s.connect((HOST, PORT))
        s.close()
    except:
        print()
        print(f"{R}+-----------------------------------------------------------------------+{X}")
        print(f"{R}|  ERROR: Server not running on {HOST}:{PORT}{X}")
        print(f"{R}+-----------------------------------------------------------------------+{X}")
        print()
        print(f"  Start your server first:")
        print(f"    {C}./ircserv {PORT} {PASSWORD}{X}")
        print()
        sys.exit(1)
    
    t = Tests()
    
    test_connection(t)
    test_registration(t)
    test_channels(t)
    test_messaging(t)
    test_operator(t)
    test_modes(t)
    test_bot(t)
    test_ping(t)
    test_quit(t)
    test_edge(t)
    test_concurrent(t)
    
    ok = t.summary()
    print()
    
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()
