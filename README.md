# Audio Serial

A simple project for outputting raw serial data over the audio port.
When the "Stream" switch is toggled on, a new byte will be sent out over the default audio stream whenever the slider is moved.
See [this blog post](http://davidawehr.com/how-to/2018/05/20/audio-serial) for more details, including the hardware requirements.

Note: It does not gracefully handle audio devices changing, so if something is plugged or removed from the headphone jack while streaming, the app will need to be restarted.

Clone this project with `git clone --recursive` to get the oboe library submodule.

