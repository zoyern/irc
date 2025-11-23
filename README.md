SkllServer
  └─ SkllNetwork (main thread game)
      ├─ SkllProtocol ("gametcp" sur port 7777)
      │   ├─ Client A
      │   └─ Client C
      └─ SkllProtocol ("gameudp" sur port 7778)
      │   ├─ Client A
      │   └─ Client B
  └─ SkllNetwork (main thread web)
      ├─ SkllProtocol ("web" sur port 8080)
      │   ├─ Client A
      │   ├─ Client B
      │   └─ Client C
  └─ SkllNetwork (main thread web)
      ├─ SkllProtocol ("irc" sur port 6667)
      │   ├─ Client A
      │   ├─ Client B
      │   └─ Client C