docker build . -t rain
docker run -it rain
---
VSCode terminal users:
If rendering is slow try the following settings in your settings.json.

`"terminal.integrated.allowChords": false,
"terminal.integratedexperimentalLinkProvider": false,
"terminal.integrated.rendererType": "experimentalWebgl"``
