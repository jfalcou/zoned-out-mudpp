----------------------------------------------------------------------------------------------------
-- MUDpp Zone - Lobby
----------------------------------------------------------------------------------------------------

lobby = { start = 0
        , name = "The Adventurers' Lobby"
        , desc = "The lobby is the zone where every adventurer appears."
        , rooms = { [0] = { name = "a luxurious bedroom"
                          , desc = "A fabulous, exotic room decorated with taste and luxury.\n\r"
                                .. "North, a #b@ydoor## opens on a corridor."
                          , exit =  { n = { type   = "door"
                                          , state  = exit_state.open
                                          , target = 1
                                          , desc   = "A finely carved wooden door made of mahogany."
                                          }
                                    }
                          }
                  , [1] = { name = "a wooden corridor"
                          , desc = "A corridor made of exotic wood is subtly lighted by various candles."
                          , exit =  { s = { type   = "door"
                                          , state  = exit_state.closed
                                          , target = 0
                                          , desc   = "A finely carved wooden door made of mahogany."
                                          }
                                    }
                           }
                  -- , [2] = { name = "the main room"
                  --         , desc = "A lively room, full of noises and music. It's warm and inviting."
                  --               ..  "North, a wooden frame opens on a #b@ylarge yard##.\n\r"
                  --         , exit = {s=1,n=3}
                  --         }
                  -- , [3] = { name = "the Inn garden"
                  --         , desc = "You are standing in a fabulous, exotic room.\n\r"
                  --               .. "Down, you find a road..."
                  --         , exit = {s=2}
                  --         }
                  }
        }
