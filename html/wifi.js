function onKeyUpTextWepKey()
{
}

function onSelectEncryptionType()
{
}

function changeSettingsNetworkSettings(obj)
{
    var param = "";
    //param.packet[0].id = parent.header.getId();
	obj = document.getElementById("ssid");
	param += 'set,wifisetting,';
        param += obj.value;
	param += ',';
	var select     = document.getElementById("selectEncryptionType")
	if((select.value == "no_security") || (select.value == "")) {
		param += 'no_security,';
	}
	else if(select.value == "wep") {
		param += 'wep,';
	}
	else if(select.value == "wpa_psk_aes") {
		param += 'wpa_psk_aes,';
	}
	else if(select.value == "wpa_psk_tkip") {
		param += 'wpa_psk_tkip,';
	}
	else if(select.value == "wpa2_psk_aes") {
		param += 'wpa2_psk_aes,';
	}
	else if(select.value == "wpa2_psk_tkip") {
		param += 'wpa2_psk_tkip,';
	}
	else {
	}
	obj = document.getElementById("key");
	param += obj.value;

    var req = openXMLHttpRequest();
    var	xhr = req.xhr;
    xhr.onreadystatechange = function() {
		if (xhr.readyState == 4) {
			if (xhr.status == 503) {
			  parent.main.location.href = "unable1.html";
			} else if (xhr.status == 518) {
			  parent.main.location.href = "unable.html";
			} else if (xhr.status == 519) {
			  parent.main.location.href = "unable2.html";
			} else if (xhr.status == 200) {
			  alert(xhr.responseText);
	/*
			var	response = JSON.parse(xhr.responseText);
			if (response.type == "http_set_result") {
				if (response.packet) {
				if (response.packet.length > 0) {
					var tmp = response.packet[0];
					if (tmp) {
						switch(tmp.value) {
							case "ACK" :
								parent.main.location.href = "network_settings_change_setting.html";
						break;
							case "NAK" :
							case "ERR" :
							default:
								parent.main.location.href = "network_settings_error.html";
							break;
						}
					}
				}
				}
			}
	*/
			} else if (xhr.status == 0) {
			  console.log("http status code is 0");
			} else {
			  alert("unknown error: " + xhr.status);
			  console.log("unknown error: " + xhr.status);
			}
		}
    };
    req.param = param;
    xhr.send(param);
    console.log("send: " + param);
}
