----------------------------------------------------------------------------------------------------
-- MUDpp Zone - Lobby
----------------------------------------------------------------------------------------------------

lobby = { start = 0
        , name = "The Adventurers' Lobby"
        , desc = "The lobby is the zone where every adventurer appears."
        , rooms = { [0] = { name = "a luxurious bedroom"
                          , desc = "You are standing in a fabulous, exotic room.\n"
                                .. "North, a door opens on a #b@ycorridor##."
                          , exit = {n=1}
                          }
                  , [1] = { name = "a wooden corridor"
                          , desc = "A corridor made of exotic wood is subtly lighted by "
                                .. "various candles. It leads to a #b@ybedroom## south."
                          , exit = {s=0}
                          }
                  }
        }
