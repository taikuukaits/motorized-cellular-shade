(Recommend doing this on a PC with WiFi)

Navigate to Home Assistant: http://homeassistant.local:8123/
Add On: ESPHome Device Builder
Open Web UI
Click "New Device"
Click "Continue"
Click "Empty Configuration"
Name it something ie, "Bay Window 5"
Click "Edit" on newly created device. 
Copy/paste motorized-shade.yaml (but replace with your REDACTED fields or copy/paste from existing shade)
Remember to replace name and friendly name with your devices. 
Then click install. 
Select "manual download".
Save file when completed. 

Navigate to "https://web.esphome.io/"
Click "Install" 
Choose file: pick file downloaded
Click "Install" 

Wait for 100%: Configuration installed!

Unplug device (turn off power) and plug back in. Should appear ONLINE in ESPHome.

Now you should be able to deploy directly from ESPHome and should see the device in Home Assistant.