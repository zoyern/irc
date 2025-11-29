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




	  server
    .networks(network)
        .listen(http)
            .router()
                .set_server(&netflix)

                // ═══════════════════════════════════════════
                // GET
                // ═══════════════════════════════════════════
                .route("GET ")
                    .then(log_request)

                    // /api
                    .path("/api")
                        .then(check_api_key)

                        // /api v1
                        .path("v1")
                            .then(check_rate_limit)

                            // /api/v1/profiles
                            .path("profiles")
                                .node("", profiles_list)

                                .path("{id}")
                                    .node("", profiles_get)
                                    .node("avatar", profiles_avatar)
                                    .node("preferences", profiles_prefs)
                                    .node("viewing-history", profiles_history)

                            // /api/v1/videos
                            .path("videos")
                                .node("", videos_list)
                                .node("trending", videos_trending)

                                .path("{id}")
                                    .node("", videos_get)
                                    .node("metadata", videos_metadata)

                                    .path("streams")
                                        .node("{quality}", videos_stream)

                            // /api/v1/search
                            .path("search")
                                .node("", search_query)
                                .node("suggestions", search_suggestions)

                        // /api v2
                        .path("v2")
                            .path("profiles")
                                .node("", profiles_v2)
                                .node("{id}", profiles_v2_get)

                    // FRONTEND (reset racine interne)
                    .path("/")
                        .node("", homepage)
                        .node("browse", browse_page)

                        .path("watch")
                            .node("{id}", watch_page)

                // ═══════════════════════════════════════════
                // POST
                // ═══════════════════════════════════════════
                .route("POST ")
                    .path("/api/v1")

                        .path("auth")
                            .node("login", auth_login)
                            .node("logout", auth_logout)
                        .path("profiles", profiles_create);