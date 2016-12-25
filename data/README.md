# Baterrystats evaluations/approximations

[Used guide](https://developer.android.com/studio/profile/battery-historian.html)

Search in bugreports for `Estimated power use`

## Test device
All test done on

* Sony Experia Z1 Compact
* Android 5.1.1
* Software Version 14.6.A.1.236
* disabled power saving modes

## Test setting
* 1 hour each
* Bluetooth/WiFi enabled on test start
* Bluetooth/WiFi disabled on test end

## Bluetooth enabled
`bugreport_1h_bt_enabled.txt`

## Bluetooth enabled and connected over RFCOMM insecure connection
`bugreport_1h_connected.txt`

## Bluetooth enabled and indefinitely discoverable
`bugreport_1h_discoverable.txt`

## Bluetooth enabled, running non-stop device discovery
`bugreport_1h_discovery.txt`

## Bluetooth enabled, connected and transceiving data
`bugreport_1h_transmitting.txt`

For this test two devices pass an integer, incrementing the received value before transmitting it back.

## WiFi video live stream
`bugreport_1h_wifi_stream.txt`

Included as an every day task/typical use-case to put previous evaluations into perspective.
