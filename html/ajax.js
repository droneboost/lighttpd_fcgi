function createXMLHttpRequest()
{
    try {
	return new XMLHttpRequest();
    } catch(ex) {}

    // for IE
    try {
	return new ActiveXObject("MSXML2.XMLHTTP.6.0");
    } catch(ex) {}

    try {
	return new ActiveXObject("MSXML2.XMLHTTP.3.0");
    } catch(ex) {}

    return null;
}

function openXMLHttpRequest()
{
    var xhr = createXMLHttpRequest();
    if (xhr) {
	xhr.open("POST", "/cgi-bin/request.fcgi", true);
	xhr.setRequestHeader("Pragma", "no-cache");
	xhr.setRequestHeader("Cache-Control", "no-cache");
	xhr.setRequestHeader("Cache-Control", "no-store");
	xhr.setRequestHeader("If-Modified-Since", "Thu, 01 Jun 1970 00:00:00 GMT");
	xhr.setRequestHeader("Content-Type", "application/json");
    }

    var obj = new Object;
    obj.xhr = xhr;
    return obj;
}
