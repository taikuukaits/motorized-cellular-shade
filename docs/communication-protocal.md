# Discovery

Technically, there are two types of discovery that happen - discovery on Home Assistant and discovery with shade-boss. 

Live configuration of the shade is possible with ShadeBoss. If you do not need live configuration, the shade should automatically appear in Home Assistant with MQTT Integrations discovery feature. 

# Hello & Announce

All shades listen to the "Greetings" MQTT channel and this is how shades can be discovered through ShadeBoss. There are two payloads the shades will listen for, the Announce payload and Hello payload. By default, this topic is the same for all shades so that ShadeBoss can easily find any new shade. 

Any Shade that revieces Announce will respond with it's device id. This is useful for finding all shades online or doing a heartbeat, though not particularly useful for finding a new shade. 

Only shades that are 'not setup' (currently that means having a max_steps of zero)will respond to the Hello payload with it's device id. This is useful for finding new shades.

Once we have a shade's device id, we can communicate with it over MQTT because (by default) it's communication topics follow a common template that only varies by device id. So if we know your device id, ShadeBoss can figure out each shade's calibration topics.